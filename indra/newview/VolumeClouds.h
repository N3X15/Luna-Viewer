// From http://www.gamedev.net/columns/hardcore/cloudrendering/default.asp
// Appears to be public domain.
//
#include "llviewerprecompiledheaders.h"
#include "v3math.h"
#include "v4math.h"
#include "llviewerregion.h"

#include <vector>
#include "Vector.h"
#include "Color.h"

#ifndef _VOLUMETRIC_H
#define _VOLUMETRIC_H

#define SORT_TOWARD 0
#define SORT_AWAY 1

// FROM Common.h
#define PI 3.141592653589f
#define SQR(x) ( (x) * (x) )

struct CloudPuff
{
	float 	Size;
	int 	ID;
	float 	Angle;
	Vector3 Position;
	float	DistanceToCam;
	Color4 	Color;
	float	Life;	
};

struct VolumetricCloud
{
	std::vector<CloudPuff> Puffs;
	unsigned 	ImpostorTex;
	Vector3		BoundingBox1, BoundingBox2;
	Vector3 	Center;
	float		Radius;
	Vector3*	VertexBuffer;
	Color4*		ColorBuffer;
	Vector2*	TexCoordBuffer;
	
	Vector3		LastCamera;
	Vector3		LastLight;
	float		DistanceFromCamera;
	int			ImpostorSize;
	Vector3		vx, vy;	//up and right vectors for impostor rendering
};

class VolumetricClouds
{
public:
	VolumetricClouds();
	
	int 	Create(int NumClouds, float PlaneSize, float PlaneHeight, LLViewerRegion *Region);	
	void	Update(Vector3 Sun, Vector3 Camera);
	void	Render(Vector3 Sun, Vector3 Camera);
	void	Destroy();
	void	GetInfo(int* Sprites, int* Impostors)
	{
		*Sprites = NumSprites;
		*Impostors = NumImpostors;
	}
	const U64 GetRegionHandle() {return mRegionHandle;}

private:

	void	UpdateCloud(VolumetricCloud* Cloud, Vector3 Sun, Vector3 Camera);
	void	RenderCloudImpostor(VolumetricCloud* Cloud, float alpha); 
	void	RenderCloud3D(VolumetricCloud* Cloud, Vector3 Camera, Vector3 Sun, float alpha);

	void	MakeCloudImpostor(VolumetricCloud* Cloud, Vector3 Sun, Vector3 Camera);
	
	void	LightCloud(VolumetricCloud* Cloud, Vector3 Sun);
	void	GrowCloud(VolumetricCloud* Cloud, int level, float radius, Vector3 Position);

	int		GetImpostorSize(float distance2);	
	void	GenerateTexture();
	void	SortParticles(VolumetricCloud* Cloud, int mode);

	class	SortAwayComparison
	{
	public:
		bool operator () (CloudPuff puff1, CloudPuff puff2)
		{
			return puff1.DistanceToCam < puff2.DistanceToCam;
		}
	} SortAway;

	class	SortTowardComparison
	{
	public:
		bool operator () (CloudPuff puff1, CloudPuff puff2)
		{
			return puff1.DistanceToCam > puff2.DistanceToCam;
		}
	} SortToward;

	class	SortCloudsTowardComparison
	{
	public:
		bool operator () (VolumetricCloud cloud1, VolumetricCloud cloud2)
		{
			return cloud1.DistanceFromCamera > cloud2.DistanceFromCamera;
		};
	} SortCloudToward;

	unsigned PuffTexture;
	unsigned PuffImage;

	std::vector<VolumetricCloud> Clouds;
	
	int SplatBufferSize;
	int ImpostorSize;
	int NumSprites, NumImpostors;
	float Albedo, Extinction;
	U64 mRegionHandle;
};

// Needed to use Vector3 instead of LLVectors due to storage of Vector3s in a binary file.
//	Will eventually convert, but right now I'm more concerned about basic functionality.

inline LLVector3 V2LLV(const Vector3 i)
{
	return LLVector3(i.x,i.y,i.z);
}
inline Vector3 LLV2V(const LLVector3 i)
{
	return Vector3(i.mV[VX],i.mV[VY],i.mV[VZ]);
}
inline Vector3 LLVd2V(const LLVector3d i)
{
	return Vector3((float)i.mdV[VX],(float)i.mdV[VY],(float)i.mdV[VZ]);
}

// __fastcall (M$-specific)
inline float carmack_func(float x)
{
	int carmack;
	float isx, halfx;	//Inverse Squareroot of x

	halfx = 0.5f*x;
	carmack = *(int*)&x; 
	carmack = 0x5f3759df - (carmack>>1); 
	isx = *(float*)&carmack; 

	isx = isx*(1.5f-halfx*isx*isx);  //Newton-Rhapson step, add more for accuracy

	return isx;
}
#endif
