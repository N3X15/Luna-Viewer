// OpenGLExample.cpp : Demonstrates usage of the SilverLining(tm) atmospheric rendering library.
// Copyright (c) 2006-2009 Sundog Software. All rights reserved worldwide.

#ifdef WIN32
#include "stdafx.h"
#endif
#include <stdlib.h>
#include "SilverLining.h"
#ifdef MAC
#include <GLUT/glut.h>
#else
#include "glut.h"
#endif
#include <assert.h>

// All SilverLining objects are in the SilverLining namespace.
using namespace SilverLining;

// Statics and defines for a simple, self-contained demo application
static Atmosphere *atm = 0;	// The Atmosphere object is the main interface to SilverLining.
static float aspectRatio, yaw = 0;

// Simulated visibility in meters, for fog effects.
#define kVisibility  20000.0f

// Stuff for the checkerboard texture map.
#define checkImageDim 64
static GLubyte checkImage[checkImageDim][checkImageDim][4];
static GLuint texName;

// Configure high cirrus clouds.
static void SetupCirrusClouds()
{
    CloudLayer *cirrusCloudLayer;

    cirrusCloudLayer = CloudLayerFactory::Create(CIRRUS_FIBRATUS);
    cirrusCloudLayer->SetBaseAltitude(8000);
    cirrusCloudLayer->SetThickness(0);
    cirrusCloudLayer->SetBaseLength(100000);
    cirrusCloudLayer->SetBaseWidth(100000);
    cirrusCloudLayer->SetLayerPosition(0, 0);
    cirrusCloudLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(cirrusCloudLayer);
}

// Add a cumulus congestus deck with 40% sky coverage.
static void SetupCumulusCongestusClouds()
{
    CloudLayer *cumulusCongestusLayer;

    cumulusCongestusLayer = CloudLayerFactory::Create(CUMULUS_CONGESTUS);
    cumulusCongestusLayer->SetBaseAltitude(1500);
    cumulusCongestusLayer->SetThickness(100);
    cumulusCongestusLayer->SetBaseLength(30000);
    cumulusCongestusLayer->SetBaseWidth(30000);
    cumulusCongestusLayer->SetDensity(0.4);
    cumulusCongestusLayer->SetLayerPosition(0, 0);
    cumulusCongestusLayer->SeedClouds(*atm);
    cumulusCongestusLayer->GenerateShadowMaps(false);

    atm->GetConditions()->AddCloudLayer(cumulusCongestusLayer);
}

// Sets up a solid stratus deck.
static void SetupStratusClouds()
{
    CloudLayer *stratusLayer;

    stratusLayer = CloudLayerFactory::Create(NIMBOSTRATUS);
    stratusLayer->SetBaseAltitude(1000);
    stratusLayer->SetThickness(600);
    stratusLayer->SetDensity(0.5);
    stratusLayer->SetLayerPosition(0, 0);
    stratusLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(stratusLayer);
}

// A thunderhead; note a Cumulonimbus cloud layer contains a single cloud.
static void SetupCumulonimbusClouds()
{
    CloudLayer *cumulonimbusLayer;

    cumulonimbusLayer = CloudLayerFactory::Create(CUMULONIMBUS_CAPPILATUS);
    cumulonimbusLayer->SetBaseAltitude(1000);
    cumulonimbusLayer->SetThickness(3000);
    cumulonimbusLayer->SetBaseLength(3000);
    cumulonimbusLayer->SetBaseWidth(5000);
    cumulonimbusLayer->SetLayerPosition(5000, 5000);
    cumulonimbusLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(cumulonimbusLayer);
}

// Cumulus mediocris are little, puffy clouds. Keep the density low for realism, otherwise
// you'll have a LOT of clouds because they are small.
static void SetupCumulusMediocrisClouds()
{
    CloudLayer *cumulusMediocrisLayer;

    cumulusMediocrisLayer = CloudLayerFactory::Create(CUMULUS_MEDIOCRIS);
    cumulusMediocrisLayer->SetBaseAltitude(1000);
    cumulusMediocrisLayer->SetThickness(100);
    cumulusMediocrisLayer->SetBaseLength(20000);
    cumulusMediocrisLayer->SetBaseWidth(20000);
    cumulusMediocrisLayer->SetDensity(0.2);
    cumulusMediocrisLayer->SetLayerPosition(0, 0);
    cumulusMediocrisLayer->SeedClouds(*atm);
    cumulusMediocrisLayer->GenerateShadowMaps(false);

    atm->GetConditions()->AddCloudLayer(cumulusMediocrisLayer);
}

// Configure SilverLining for the desired wind, clouds, and visibility.
static void SetupAtmosphericConditions()
{
    assert(atm);

    // Set up wind blowing south at 50 meters/sec
    WindVolume wv;
    wv.SetDirection(180);
    wv.SetMinAltitude(0);
    wv.SetMaxAltitude(10000);
    wv.SetWindSpeed(50);
    atm->GetConditions()->SetWind(wv);

    // Set up the desired cloud types.
    SetupCirrusClouds();
    SetupCumulusCongestusClouds();
    //SetupStratusClouds();
    //SetupCumulonimbusClouds();
    //SetupCumulusMediocrisClouds();

    // Set visibility
    atm->GetConditions()->SetVisibility(kVisibility);
}

// For consistently lit scenes, query SilverLining for lighting information based on its
// internal lighting model. This will give you tone-mapped lighting based on the
// current simulated sun and moon positions and cloud cover.
void SetSceneLighting()
{
    float x, y, z, r, g, b, ra, ga, ba;
    atm->GetSunOrMoonPosition(&x, &y, &z);
    atm->GetSunOrMoonColor(&r, &g, &b);
    atm->GetAmbientColor(&ra, &ga, &ba);

    GLfloat light_ambient[] = {ra, ga, ba, 1.0};
    GLfloat light_diffuse[] = {r, g, b, 1.0};
    GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_position[] = {x, y, z, 0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    GLfloat mat_amb_diff[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat no_mat[] = {0, 0, 0, 0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

// For effects inside stratus decks, it's important to honor any requests from SilverLining
// for fog. SilverLining can also provide guidance for a good fog color to use to blend
// distant terrain into the sky.
void SetSceneFog()
{
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glHint(GL_FOG_HINT, GL_NICEST);

    // Need to light the fog, or it will glow in the dark...
    float ar, ag, ab;
    atm->GetSunOrMoonColor(&ar, &ag, &ab);

    if (atm->GetFogEnabled())
    {
        float density, r, g, b;
        atm->GetFogSettings(&density, &r, &g, &b);
        glFogf(GL_FOG_DENSITY, density);

        GLfloat fogColor[4] = {r * ar, g * ag, b * ab, 1.0};
        glFogfv(GL_FOG_COLOR, fogColor);
    }
    else
    {
        GLfloat fogColor[4];
        atm->GetHorizonColor(yaw, &fogColor[0], &fogColor[1], &fogColor[2]);
        glFogfv(GL_FOG_COLOR, fogColor);

        float density = 1.0 / kVisibility;
        glFogf(GL_FOG_DENSITY, density);
    }
}

// Draws a checkerboard-textured plane to fill in for terrain.
void DrawGroundPlane()
{
    const float dim = 100000;

    glColor3f(1, 1, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texName);

    glBegin(GL_QUADS);
    glTexCoord2f(  0,   0);
    glNormal3f(0, 1, 0);
    glVertex3f(-dim, 0, dim);
    glTexCoord2f(  0, 100);
    glNormal3f(0, 1, 0);
    glVertex3f(dim, 0, dim);
    glTexCoord2f(100, 100);
    glNormal3f(0, 1, 0);
    glVertex3f(dim, 0, -dim);
    glTexCoord2f(100,   0);
    glNormal3f(0, 1, 0);
    glVertex3f(-dim, 0, -dim);
    glEnd();
}

// The main rendering loop! Note how SilverLining is integrated here.
void Display()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspectRatio, 2, 100000);

    // Increment the yaw each frame to spin the camera around
    yaw += 0.05;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(-10, 1, 0, 0);
    glRotatef(yaw, 0, 1, 0);
    glTranslatef(0, -100, 0);

    // Pass in the view and projection matrices to SilverLining.
    double mv[16], proj[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    atm->SetCameraMatrix(mv);
    atm->SetProjectionMatrix(proj);

    // After setting up your projection and modelview matrices to reflect the current
    // camera position, call Atmosphere::BeginFrame() to draw the sky and do the lighting
    // pass on the clouds, if necessary.
    atm->BeginFrame(true);

    // Now, do all your own drawing...
    SetSceneLighting();
    SetSceneFog();
    DrawGroundPlane();

    // When you're done, call Atmosphere::EndFrame() to draw all the clouds from back to front.
    atm->EndFrame();

    // Now swap the back and front buffers.
    glutSwapBuffers();

    glutPostRedisplay();
}

// Handle the user resizing the window.
void Reshape(int x, int y)
{
    aspectRatio = (float)x / (float)y;

    glViewport(0, 0, x, y);
}

// Creates the checkerboard image for the ground texture.
void makeCheckImage()
{
    int i, j, c;

    for (i = 0; i < checkImageDim; i++)
    {
        for (j = 0; j < checkImageDim; j++)
        {
            c = ((((i&0x8)==0)^((j&0x8))==0))*255;
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
            checkImage[i][j][3] = (GLubyte) 255;
        }
    }
}

// Initialize glut and the ground texture.
void SetupOpenGL()
{
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    aspectRatio = 4.0 / 3.0;

    glutCreateWindow("SilverLining OpenGL Example");

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);

    makeCheckImage();
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageDim, checkImageDim, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, checkImage);
}

// Sets the simulated location and local time.
// Note, it's important that your longitude in the Location agrees with
// the time zone in the LocalTime.
void SetTimeAndLocation()
{
    Location loc;
    loc.SetLatitude(45);
    loc.SetLongitude(-122);

    LocalTime tm;
    tm.SetYear(1971);
    tm.SetMonth(8);
    tm.SetDay(7);
    tm.SetHour(30);
    tm.SetMinutes(30);
    tm.SetSeconds(0);
    tm.SetObservingDaylightSavingsTime(true);
    tm.SetTimeZone(PST);

    atm->GetConditions()->SetTime(tm);
    atm->GetConditions()->SetLocation(loc);
}

// Main entry point:
#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{
    // If you want different clouds to be generated every time, remember to seed the
    // random number generator.
    srand(time(NULL));

    // Set up GLUT and the ground texture.
    glutInit(&argc, argv);
    SetupOpenGL();

    // Instantiate an Atmosphere object. Substitute your own purchased license name and code here.
    atm = new Atmosphere("Your Company Name", "Your License Code");

    atm->ShowFramerate(true);

    // Tell SilverLining we're rendering in OpenGL and the Resources directory is 2 directories
    // above the working directory.
	int err;

#ifdef WIN32
    err = atm->Initialize(Atmosphere::OPENGL, "..\\..\\Resources\\", true, 0);
#else
    err = atm->Initialize(Atmosphere::OPENGL, "../../Resources/", true, 0);
#endif

	if (err == Atmosphere::E_NOERROR)
    {
        // Set up all the clouds
        SetupAtmosphericConditions();

        // Configure where and when we want to be
        SetTimeAndLocation();

        // Start rendering.
        glutMainLoop();
    }
	else
	{
		printf("Error was %d\n", err);
	}
    return 0;
}

