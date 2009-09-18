#include "llviewerprecompiledheaders.h"
#include <algorithm>

#include "llviewercamera.h"
#include "llviewerregion.h"
#include "llerror.h"
#include "llgl.h"

#include "VolumeClouds.h"
#include "Vector.h"
#include "Color.h"

#define GL_CLAMP_TO_EDGE 0x812F

#define NUM_CLOUD_FILES		11		//number of cloud data files
#define DIST_MUL_IMPOSTOR	4		//used for impostor/3d distance interpolation
#define DIST_MUL_3D		2		//same as above
#define ALPHA_ADJUST		0.3f	//tweak the fading between impostors and 3d models so we 
									//don't get transparent clouds

VolumetricClouds::VolumetricClouds()
{
	Albedo = 0.95f;
	Extinction = 80.0f;
	SplatBufferSize = 32;		//size of the splat buffer (this is a good value)
	ImpostorSize = 512;		//maximum size of the impostor
}

// @brief Generate a cloud
//TODO Procedurally generate a cloud deck. (Gauss?)
void VolumetricClouds::GrowCloud(VolumetricCloud* Cloud, int level, float radius, Vector3 Position)
{
	FILE* fp = NULL;
	char strname[256];
#ifdef WIN32	
	sprintf(strname, "clouds\\cloud%d.dat", rand() % NUM_CLOUD_FILES);
#else
	sprintf(strname, "clouds/cloud%d.dat", rand() % NUM_CLOUD_FILES);
#endif

	fp = fopen(strname, "rb");
	if (!fp) return;

	int Num;
	fread(&Num, sizeof(int), 1, fp);

	CloudPuff	puff;
	for (int i = 0; i < Num; i++)
	{
		fread(&puff.Position, sizeof(Vector3), 1, fp);

		if (Magnitude(puff.Position) > Cloud->Radius)
			Cloud->Radius = Magnitude(puff.Position);
		
		//compute the bounding box, used for rendering impostors and frustum culling
		Cloud->BoundingBox1 = Vector3(-1000, -1000, -1000);
		Cloud->BoundingBox2 = Vector3(1000, 1000, 1000);
			
		if (puff.Position.x > Cloud->BoundingBox1.x) Cloud->BoundingBox1.x = puff.Position.x;				
		if (puff.Position.y > Cloud->BoundingBox1.y) Cloud->BoundingBox1.y = puff.Position.y;				
		if (puff.Position.z > Cloud->BoundingBox1.z) Cloud->BoundingBox1.z = puff.Position.z;				
		if (puff.Position.x < Cloud->BoundingBox2.x) Cloud->BoundingBox2.x = puff.Position.x;				
		if (puff.Position.y < Cloud->BoundingBox2.y) Cloud->BoundingBox2.y = puff.Position.y;				
		if (puff.Position.z < Cloud->BoundingBox2.z) Cloud->BoundingBox2.z = puff.Position.z;
				
		puff.Position += Position;
		puff.Angle = 0.0f;
		puff.ID = 0;
		puff.Life = 1.0f;
					
		Cloud->Puffs.push_back(puff);
	}
	for (int i = 0; i < Num; i++)
	{
		fread(&Cloud->Puffs[i].Size, sizeof(float), 1, fp);
	}
	for (int i = 0; i < Num; i++)
	{
		fread(&Cloud->Puffs[i].Color, sizeof(Color4), 1, fp);
	}
	fclose(fp);
	
	//allocate buffers for rendering
	Cloud->VertexBuffer = new Vector3[Num * 4];
	Cloud->TexCoordBuffer = new Vector2[Num * 4];
	Cloud->ColorBuffer = new Color4[Num * 4];
}

void VolumetricClouds::GenerateTexture()
{
	int N = 64;
	unsigned char *B = new unsigned char[4*N*N];
	float X,Y,Dist;
	float Incr = 2.0f/N;
	int i = 0, j = 0;
	float value;

	Y = -1.0f;
	for (int y = 0; y < N; y++)
	{
		X = -1.0f;
		for (int x=0; x<N; x++, i++, j+=4)
		{
			Dist = (float)sqrt(X*X+Y*Y);
			if (Dist > 1) Dist=1;

			//our magical interpolation polynomical
			value = 2*Dist*Dist*Dist - 3*Dist*Dist + 1;
			value *= 0.4f;

			B[j+3] = B[j+2] = B[j+1] = B[j] = (unsigned char)(value * 255);
			
			X+=Incr;
		}
		Y+=Incr;
	}

	glGenTextures(1, &PuffTexture);
	glBindTexture(GL_TEXTURE_2D, PuffTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, N, N, 0, GL_RGBA, GL_UNSIGNED_BYTE, B);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	

	delete [] B;	
}

// @brief Generate a cloud deck.
int VolumetricClouds::Create(int Num, float PlaneSize, float PlaneHeight,LLViewerRegion *Region)
{
	int i;
	
	GenerateTexture();
	mRegionHandle=Region->getHandle();

	for (i = 0; i < Num; i++)
	{
		VolumetricCloud Cloud;
		Cloud.Center = LLVd2V(Region->getCenterGlobal());
		// Was: rand() % (int)(PlaneSize * 2) - PlaneSize;
		Cloud.Center.x+=rand() % (int)(PlaneSize * 2);
		Cloud.Center.y+=rand() % (int)(PlaneSize * 2);
		Cloud.Center.z = PlaneHeight;
		Cloud.Radius = 15.0f;
		Cloud.LastLight = Vector3(0, 0, 0);
		Cloud.LastCamera = Vector3(0, 0, 0);
		Cloud.ImpostorTex = 0;
		
		GrowCloud(&Cloud, 0, Cloud.Radius, Cloud.Center);

		Clouds.push_back(Cloud);
	}
	
	return 0;
}

void VolumetricClouds::Update(Vector3 Sun, Vector3 Camera)
{
	llinfos << "Init SunDir,ToCam" << llendl;
	Vector3 SunDir, ToCam;

	llinfos << "Normalize Sun" << llendl;
	SunDir = Normalize(Sun);
	
	for (unsigned i = 0; i < Clouds.size(); i++)
	{
		llinfos << "Update Cloud #" << i << llendl;
		//if the angle between the camera and the cloud center
		//has changed enough since the last lighting calculation
		//recalculate the lighting (very slow - don't move the sun :) )

		llinfos << "#" << i << ": Cam direction" << llendl;
		ToCam =  Normalize(Camera - Clouds[i].Center);

		llinfos << "#" << i << " Dot(SunDir,Cloud)" << llendl;
		if (Dot(SunDir, Clouds[i].LastLight) < 0.99f)
		{
			llinfos << "#" << i << " Light cloud" << llendl;
			LightCloud(&Clouds[i], Sun);
			MakeCloudImpostor(&Clouds[i], Sun, Camera);
			Clouds[i].LastLight = SunDir;
			Clouds[i].LastCamera = ToCam;
		}
		else
		{
			float dot = Dot(ToCam, Clouds[i].LastCamera);
			BOOL in_frustum = LLViewerCamera::getInstance()->sphereInFrustum(V2LLV(Clouds[i].Center), Clouds[i].Radius);
			int mip_size = GetImpostorSize(SqDist(Camera, Clouds[i].Center));
			
			//same as above only recreating the impostor
			//also recreates the impostor if the current mip level
			//isn't good enough (the camera has come too close)
			if ((dot < 0.99f || Clouds[i].ImpostorSize < mip_size) && in_frustum)
			{
				MakeCloudImpostor(&Clouds[i], Sun, Camera);
				Clouds[i].LastCamera = ToCam;
			}
		}
	}
	LLGLState::restoreGL();
}

int VolumetricClouds::GetImpostorSize(float d)
{
	//this is done rather arbitrarely
	//it all depends on the scale of the world/clouds
	//to make it more robust we should use the size of
	//the cloud
	int Size = ImpostorSize;

	if (d > 50 * 50) Size /= 2;
	if (d > 100 * 100) Size /= 2;
	if (d > 200 * 200) Size /= 2;
	if (d > 300 * 300) Size /= 2;

	if (Size < 32) Size = 32;
		
	return Size;
}

void VolumetricClouds::Render(Vector3 Camera, Vector3 Sun)
{
	unsigned i, j;

	NumSprites = NumImpostors = 0;

	for (i = 0; i < Clouds.size(); i++)
	{
		Clouds[i].DistanceFromCamera = SqDist(Clouds[i].Center, Camera);
	}
	//we sort our clouds (impostors) when doing rendering since we're alpha blending
	bool done = false;
	while (!done)
	{
		done = true;	
		for (i = 0; i < Clouds.size(); i++)
			for (j = i+1; j < Clouds.size(); j++)
			{
				if (Clouds[i].DistanceFromCamera < Clouds[j].DistanceFromCamera)
				{
					std::swap(Clouds[i], Clouds[j]);
					done = false;
				}
			}
	}

	float dist_impostor, dist_3D;
	float alpha;

	for (i = 0; i < Clouds.size(); i++)
	{
		dist_impostor = Clouds[i].Radius * DIST_MUL_IMPOSTOR; //beyond this render only the impostor
		dist_impostor *= dist_impostor;	//square this since the camera distance is also squared
		dist_3D = Clouds[i].Radius * DIST_MUL_3D;//closer than this render only the 3D model
		dist_3D *= dist_3D; //square ourselves

		//if we're in between we need to interpolate

		if (!LLViewerCamera::getInstance()->sphereInFrustum(V2LLV(Clouds[i].Center), Clouds[i].Radius)) continue;

		if (Clouds[i].DistanceFromCamera > dist_impostor)
			//fully impostor
			RenderCloudImpostor(&Clouds[i], 1.0f);
		else
			if (Clouds[i].DistanceFromCamera < dist_3D)
				//fully 3D
				RenderCloud3D(&Clouds[i], Camera, Sun, 1.0f);
			else
			{
				//in between, interpolate nicely and tweak the alpha to make it look prettier
				alpha = (Clouds[i].DistanceFromCamera - dist_3D) / (dist_impostor - dist_3D);		
				RenderCloudImpostor(&Clouds[i], ALPHA_ADJUST + alpha);
				RenderCloud3D(&Clouds[i], Camera, Sun, 1.0f - alpha);
			}
	}
}

void VolumetricClouds::MakeCloudImpostor(VolumetricCloud* Cloud, Vector3 Sun, Vector3 Camera)
{
	unsigned i;
	
	//puffs are in world space
	for (i = 0; i < Cloud->Puffs.size(); i++)
		Cloud->Puffs[i].DistanceToCam = SqDist(Camera, Cloud->Puffs[i].Position);

	SortParticles(Cloud, SORT_TOWARD);

	int 	ViewportSize = ImpostorSize;
		
	float d = Dist(Camera, Cloud->Center);
	float r = Cloud->Radius;
	float pr = Cloud->Puffs[0].Size;
		
	ViewportSize = GetImpostorSize(d * d);
	
	//if it's the first time we render this, use full resolution to create a big
	//texture. Otherwise, when we get a bigger impostor size than the original one,
	//glCopyTexSubImage2D will fail
	if (glIsTexture(Cloud->ImpostorTex) == GL_FALSE)
		ViewportSize = ImpostorSize;
			
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-Cloud->Radius-pr, Cloud->Radius+pr, -Cloud->Radius-pr, Cloud->Radius+pr, d - r, d + r);
		
	//we setup the camera to look at the cloud center from the camera position
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(Camera.x, Camera.y, Camera.z, Cloud->Center.x, Cloud->Center.y, Cloud->Center.z, 0, 1, 0);
	
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, ViewportSize, ViewportSize);
		
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		
	Vector3 vx(mat[0], mat[4], mat[8] );
	Vector3 vy(mat[1], mat[5], mat[9] );	
	
	Cloud->vx = vx;		Cloud->vy = vy; //store for rendering

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, PuffTexture);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	Vector3 Light = Normalize(Camera - Sun);
	Vector3 Omega;
	Color4  ParticleColor;
				
	std::vector<CloudPuff>::iterator PuffIter;
	CloudPuff* Puff;
	int index = 0;
	
	Vector3* vp; Vector2* tp; Color4* cp;
	vp = Cloud->VertexBuffer;
	tp = Cloud->TexCoordBuffer;
	cp = Cloud->ColorBuffer;
	
	Vector2 v1(1.0f, 0.0f), v2(0.0f, 0.0f), v3(0.0f, 1.0f), v4(1.0f, 1.0f);
	float costheta, phase;
	float omega2;

	Vector3 Corner1, Corner2, Corner3, Corner4;
	Corner1 = -vx - vy; Corner2 = vx - vy; Corner3 = vx + vy; Corner4 = vy - vx;
	
	for (PuffIter = Cloud->Puffs.begin(); PuffIter != Cloud->Puffs.end(); ++PuffIter)
	{		
		Puff = &*PuffIter;
		Omega = Puff->Position - Camera;
		
		//we want to compute the phase function
		omega2 = Omega.x * Omega.x + Omega.y * Omega.y + Omega.z * Omega.z;
		omega2 = carmack_func(omega2);
		
		//omega2 is now 1 / Mag(Omega)
		Omega.x *= omega2;	Omega.y *= omega2;	Omega.z *= omega2;
		
		//and this is the phase function
		costheta = Dot(Omega, Light);	
		phase = 0.75f * (1.0f + costheta * costheta);
	
		ParticleColor.R = 0.3f + Puff->Color.R * phase;
		ParticleColor.G = 0.3f + Puff->Color.G * phase;
		ParticleColor.B = 0.3f + Puff->Color.B * phase;
		ParticleColor.A = Puff->Color.A * Puff->Life;
				
		//add everything to the buffers
		*(vp++) = Puff->Position + Corner1 * Puff->Size;
		*(tp++) = v1;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner2 * Puff->Size;
		*(tp++) = v2;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner3 * Puff->Size;
		*(tp++) = v3;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner4 * Puff->Size;
		*(tp++) = v4;
		*(cp++) = ParticleColor;
	}
	
	//render using vertex arrays
	//homework: 1. try using dynamic vertex buffers and see the difference
	//(it probably won't be much since we're fillrate limited
	//homework: 2. also try using triangles instead of quads to see the difference
	//(i guess that depends on the card)

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, Cloud->VertexBuffer);
	glColorPointer(4, GL_FLOAT, 0, Cloud->ColorBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, Cloud->TexCoordBuffer);
	
	glDrawArrays(GL_QUADS, 0, Cloud->Puffs.size() * 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glPopAttrib();
		
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();			
	
	//if we haven't yet created an impostor texture, well, create one
	//anyway, copy from the framebuffer to the impostor texture
	//glCopyTexSubImage2D is faster so we use that if we're reloading the impostor
	if (glIsTexture(Cloud->ImpostorTex) == GL_FALSE)
	{
		glGenTextures(1, &Cloud->ImpostorTex);
		glBindTexture(GL_TEXTURE_2D, Cloud->ImpostorTex);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, ViewportSize, ViewportSize, 0);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, Cloud->ImpostorTex);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, ViewportSize, ViewportSize);
	}	

	Cloud->ImpostorSize = ViewportSize;
}

void VolumetricClouds::LightCloud(VolumetricCloud* Cloud, Vector3 Sun)
{
//	llinfos << "Lighting cloud..." << llendl;

//	llinfos << "Setup sun..." << llendl;	
//	assume Sun is a point!!
	Sun = Normalize(Sun) * Cloud->Radius * 1.5f + Cloud->Center;
	
//	llinfos << "Set cloud puff distances..." << llendl;
//	puffs are now in world space
	unsigned i, j;	
	for (i = 0; i < Cloud->Puffs.size(); i++)
		Cloud->Puffs[i].DistanceToCam = SqDist(Sun, Cloud->Puffs[i].Position);
	
//	llinfos << "Sort cloud puffs" << llendl;
	SortParticles(Cloud, SORT_AWAY);
	
//	llinfos << "Assign d, r, and pr" << llendl;
//	llinfos << " * d" << llendl;
	float d = Dist(Sun, Cloud->Center);
//	llinfos << " * r" << llendl;
	float r = Cloud->Radius;
//	llinfos << " * pr (Cloud->Puffs.size()=" << Cloud->Puffs.size()<< ")" << llendl;
	float pr = Cloud->Puffs[0].Size;
			
//	llinfos << "Setup orthographic projection" << llendl;
	//we setup an orthographic projection
	//the view volume will thus be a box and it will fit the cloud perfectly
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-Cloud->Radius-pr, Cloud->Radius+pr, -Cloud->Radius-pr, Cloud->Radius+pr, d - r, d + r);
		
//	llinfos << "Setup camera" << llendl;
	//setup the camera to lookat the cloud center and be positioned on the sun
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();		
	gluLookAt(Sun.x, Sun.y, Sun.z, Cloud->Center.x, Cloud->Center.y, Cloud->Center.z, 0, 1, 0);
		
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, SplatBufferSize, SplatBufferSize);
		
//	llinfos << "clear buffer" << llendl;
	//clear our buffer make it fully bright
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	llinfos << "Extract up & right vectors" << llendl;
	//your standard up and right vector extraction
	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		
	Vector3 vx(mat[0], mat[4], mat[8] );
	Vector3 vy(mat[1], mat[5], mat[9] );	
	
//	llinfos << "Setup texture?" << llendl;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, PuffTexture);

	float SolidAngle = 0.09f, Area;
	unsigned Pixels;
	double mp[16], mm[16];
	int vp[4];
	float *buf, avg;
		
//	llinfos << "Setup projection (again)" << llendl;
	//used for projection
	glGetDoublev(GL_MODELVIEW_MATRIX, mm);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);		

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, PuffTexture);
		
	//llinfos << "Blend..." << llendl;
	//our blending is enabled
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	llinfos << "Setup light..." << llendl;
	Color4 LightColor(1.0f, 1.0f, 1.0f, 1.0f), ParticleColor;
	float factor = 1.0f;
	int ReadX, ReadY;
	
	Vector3 Light = Normalize(Sun);

	factor = SolidAngle / (4 * PI);
	
	for (i = 0; i < Cloud->Puffs.size(); i++)
	{		
		//llinfos << "Display puff #" << i << llendl;
		//find the particle's projected coordinates
		double CenterX, CenterY, CenterZ;
		gluProject(Cloud->Puffs[i].Position.x, 
			Cloud->Puffs[i].Position.y, 
			Cloud->Puffs[i].Position.z, 
			mm, mp, vp, &CenterX, &CenterY, &CenterZ);
		
		//area = dist * dist * angle
		Area = Cloud->Puffs[i].DistanceToCam * SolidAngle;
		Pixels = (int)(sqrt(Area) * SplatBufferSize / (2 * Cloud->Radius));
		if (Pixels < 1) Pixels = 1;
		
	    //make sure we're not reading from outside the viewport, that's Undefined
		ReadX = (int)(CenterX-Pixels/2);
		if (ReadX < 0) ReadX = 0;
		ReadY = (int)(CenterY-Pixels/2);
		if (ReadY < 0) ReadY = 0;
		
		buf = new float[Pixels * Pixels];
		//we only need the red component since this is greyscale
		glReadBuffer(GL_BACK);
		glReadPixels(ReadX, ReadY, Pixels, Pixels, GL_RED, GL_FLOAT, buf);
			
		avg = 0.0f;
		for (j = 0; j < Pixels * Pixels; j++) avg += buf[j];
		avg /= (Pixels * Pixels);
		
		delete [] buf;
		
		//Light color * 
		// average color from solid angle (sum * solidangle / (pixels^2 * 4pi)) 
		// * albedo * extinction 
		// * rayleigh scattering in the direction of the sun (1.5f) (only for rendering, don't store)
		
		ParticleColor.R = LightColor.R * Albedo * Extinction * avg * factor;
		ParticleColor.G = LightColor.G * Albedo * Extinction * avg * factor;
		ParticleColor.B = LightColor.B * Albedo * Extinction * avg * factor;
		ParticleColor.A = 1.0f - exp(-Extinction);
		
		Cloud->Puffs[i].Color = ParticleColor;
		Cloud->Puffs[i].Color.Clamp();

		//the phase function (it's always 1.5f when we're looking from the sun)
		ParticleColor = ParticleColor * 1.5f;
		ParticleColor.Clamp();
				
		glColor4fv(!ParticleColor);
		
		glBegin(GL_QUADS);
				
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(!(Cloud->Puffs[i].Position + (vx + vy) * -Cloud->Puffs[i].Size));		
		glTexCoord2f(0.0f, 0.0f); 		
		glVertex3fv(!(Cloud->Puffs[i].Position + (vx - vy) *  Cloud->Puffs[i].Size));		
		glTexCoord2f(0.0f, 1.0f); 		
		glVertex3fv(!(Cloud->Puffs[i].Position + (vx + vy) *  Cloud->Puffs[i].Size)); 		
		glTexCoord2f(1.0f, 1.0f); 		
		glVertex3fv(!(Cloud->Puffs[i].Position + (vy - vx) *  Cloud->Puffs[i].Size));
		
		glEnd();				
	}	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopAttrib();
		
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
	LLGLState::restoreGL();			
}

void VolumetricClouds::RenderCloud3D(VolumetricCloud* Cloud, Vector3 Camera, Vector3 Sun, float alpha)
{
	unsigned i;
	
	//puffs are in world space
	for (i = 0; i < Cloud->Puffs.size(); i++)
		Cloud->Puffs[i].DistanceToCam = SqDist(Camera, Cloud->Puffs[i].Position);

	SortParticles(Cloud, SORT_TOWARD);

	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		
	Vector3 vx(mat[0], mat[4], mat[8] );
	Vector3 vy(mat[1], mat[5], mat[9] );	
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, PuffTexture);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	Vector3 Light = Normalize(Camera - Sun);
	Vector3 Omega;
	Color4  ParticleColor;
				
	std::vector<CloudPuff>::iterator PuffIter;
	CloudPuff* Puff;
	int index = 0;
	
	Vector3* vp; Vector2* tp; Color4* cp;
	vp = Cloud->VertexBuffer;
	tp = Cloud->TexCoordBuffer;
	cp = Cloud->ColorBuffer;
	
	Vector2 v1(1.0f, 0.0f), v2(0.0f, 0.0f), v3(0.0f, 1.0f), v4(1.0f, 1.0f);
	float costheta, phase;
	float omega2;

	Vector3 Corner1, Corner2, Corner3, Corner4;
	Corner1 = -vx - vy; Corner2 = vx - vy; Corner3 = vx + vy; Corner4 = vy - vx;
	
	for (PuffIter = Cloud->Puffs.begin(); PuffIter != Cloud->Puffs.end(); ++PuffIter)
	{		
		Puff = &*PuffIter;
		Omega = Puff->Position - Camera;
		
		omega2 = Omega.x * Omega.x + Omega.y * Omega.y + Omega.z * Omega.z;
		omega2 = carmack_func(omega2);
		
		//omega2 is now 1 / Mag(Omega)
		Omega.x *= omega2;	Omega.y *= omega2;	Omega.z *= omega2;
		
		costheta = Dot(Omega, Light);	
		phase = 0.75f * (1.0f + costheta * costheta);
	
		ParticleColor.R = (0.3f + Puff->Color.R * phase) * alpha;
		ParticleColor.G = (0.3f + Puff->Color.G * phase) * alpha;
		ParticleColor.B = (0.3f + Puff->Color.B * phase) * alpha;
		ParticleColor.A = Puff->Color.A * Puff->Life * alpha;
				
		*(vp++) = Puff->Position + Corner1 * Puff->Size;
		*(tp++) = v1;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner2 * Puff->Size;
		*(tp++) = v2;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner3 * Puff->Size;
		*(tp++) = v3;
		*(cp++) = ParticleColor;
		
		*(vp++) = Puff->Position + Corner4 * Puff->Size;
		*(tp++) = v4;
		*(cp++) = ParticleColor;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, Cloud->VertexBuffer);
	glColorPointer(4, GL_FLOAT, 0, Cloud->ColorBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, Cloud->TexCoordBuffer);
	
	glDrawArrays(GL_QUADS, 0, Cloud->Puffs.size() * 4);
	NumSprites += Cloud->Puffs.size();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);	

	glDisable(GL_TEXTURE_2D);	
	glDisable(GL_BLEND);
	LLGLState::restoreGL();
}

void VolumetricClouds::RenderCloudImpostor(VolumetricCloud* Cloud, float alpha)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Cloud->ImpostorTex);
		
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.0f);
	
	//we might get some hard edges because we don't clear the whole texture when
	//redrawing the impostor so a small adjustment of tex coords will fix this
	float texcoord = (float)(Cloud->ImpostorSize) / (float)ImpostorSize - 0.001f;
	
	//and here we make the cloud transparent depending on the alpha value
	//we use alpha in all components because of our special blending parameters
	glColor4f(alpha, alpha, alpha, alpha);
	
	glBegin(GL_QUADS);
				
	glTexCoord2f(0.0f, 0.0f); 			
	glVertex3fv(!(Cloud->Center + (Cloud->vx + Cloud->vy) * -Cloud->Radius));
		
	glTexCoord2f(texcoord, 0.0f); 		
	glVertex3fv(!(Cloud->Center + (Cloud->vx - Cloud->vy) *  Cloud->Radius));
		
	glTexCoord2f(texcoord, texcoord); 		
	glVertex3fv(!(Cloud->Center + (Cloud->vx + Cloud->vy) *  Cloud->Radius)); 
		
	glTexCoord2f(0.0f, texcoord); 		
	glVertex3fv(!(Cloud->Center + (Cloud->vy - Cloud->vx) *  Cloud->Radius));
		
	glEnd();

	glDisable(GL_TEXTURE_2D);		
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
	NumImpostors++;
}

void VolumetricClouds::SortParticles(VolumetricCloud* Cloud, int mode)
{
	bool done = false, do_swap, useSTL = true;
	unsigned i, j;	
	
	if (useSTL)
	{
		switch (mode)
		{
			case SORT_AWAY:
				sort(Cloud->Puffs.begin(), Cloud->Puffs.end(), SortAway);
				break;
			case SORT_TOWARD:
				sort(Cloud->Puffs.begin(), Cloud->Puffs.end(), SortToward);
				break;
		}			
	}
	else
	{
		while (!done)
		{
			done = true;
			for (i = 0; i < Cloud->Puffs.size(); i++)
				for (j = i + 1; j < Cloud->Puffs.size(); j++)
				{
					do_swap = false;
					switch (mode)
					{
						case SORT_AWAY:
							if (Cloud->Puffs[i].DistanceToCam < Cloud->Puffs[j].DistanceToCam)
								do_swap = true;
							break;
						case SORT_TOWARD:
							if (Cloud->Puffs[i].DistanceToCam > Cloud->Puffs[j].DistanceToCam)
								do_swap = true;					
							break;
					}
					if (do_swap)
					{
						std::swap(Cloud->Puffs[i], Cloud->Puffs[j]);
						done = false;
					}
				}
		}
	}	
}

void VolumetricClouds::Destroy()
{
	int i,n;
	n=Clouds.size();
	for(i=0;i<n;i++)
	{
		Clouds[i].Puffs.clear();
		delete [] Clouds[i].VertexBuffer;
		delete [] Clouds[i].ColorBuffer;
		delete [] Clouds[i].TexCoordBuffer;
	}
				
	Clouds.clear();
}
