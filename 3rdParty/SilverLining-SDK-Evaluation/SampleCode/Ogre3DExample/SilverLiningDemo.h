#include "ExampleApplication.h"
#include "SilverLining.h"

// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

/**
SUPER IMPORTANT!!!!!!!!!!!

Since both SilverLining and Ogre3D use NVidia's Cg libraries, it's critical that both Ogre3D and
SilverLining are using the same version of the Cg DLL's. As of this writing (7/13/08), the latest
Ogre3D build uses the December 2007 build of Cg, and SilverLining uses the May 2008 build, and
they do NOT play nice with each other. The older Ogre DLL's get linked in dynamically, but SilverLining
expects the newer DLL's.

Fortunately, new versions of Cg DLL's are backward compatible, so the solution is simple:

COPY CG.DLL and CGGL.DLL FROM THE SILVERLINING SDK'S THIRD-PARTY DIRECTORY INTO THE OGRE BIN
DIRECTORIES FOR BOTH DEBUG AND RELEASE, IF SILVERLINING'S VERSIONS OF THESE FILES ARE NEWER.

As soon as Ogre3D updates to the May 2008 build of Cg or newer, this problem will go away.
*/

/**
This is an example of integrating SilverLining's library for 3D real time volumetric clouds, sky
simulation, and precipitation into Ogre3D. More info on SilverLining may be found at 
http://www.sundog-soft.com/

This source is based on the Ogre3D demo applications, and you should use the same project settings
from the demos with a few additions:

- You must link in winmm.lib (a system library) and SilverLining-MTD-DLL.lib for debug builds or
  SilverLining-MT-DLL.lib for release builds. Refer to the SilverLining documentation
  (http://www.sundog-soft.com/docs/html/index.html) for the proper library names to use for
  other code generation types.

- Add the "public headers" directory of the SilverLining SDK to your include path.

- Add the "lib/win32" directory of the SilverLining SDK to your library path. x64 libraries are also
  available.
*/

// Substitute your own registration name and license code here
static SilverLining::Atmosphere *atm = new SilverLining::Atmosphere("foo", "bar");

#define kVisibility 100000.0

// We use a RenderQueueListener in order to get the hooks we need into the beginning and end of the frame
// (well, almost the end - we do let overlays draw after us.) Ogre3D already has handy "skies" render
// queues.
class MyRenderQueueListener : public Ogre::RenderQueueListener
{
public:
    MyRenderQueueListener(Ogre::SceneManager *pSceneMgr, SilverLining::Atmosphere *atm) : RenderQueueListener(), 
        sceneMgr(pSceneMgr), atmosphere(atm) {}

     void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation) 
     { 
        if(queueGroupId == RENDER_QUEUE_SKIES_EARLY) 
        {
            if (atm)
            {
                SetLighting();
                SetFog();
                atm->BeginFrame(true);
            }
        }
     } 

    void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation) 
     { 
        if(queueGroupId == RENDER_QUEUE_SKIES_LATE) 
        {
            if (atm)
            {
                atm->EndFrame();
            }
        }
     }

protected:
    
    void SetLighting(void)
    {
        Ogre::Light *light = sceneMgr->getLight("MainLight");
        if (atmosphere && light)
        {
            float ra, ga, ba, rd, gd, bd, x, y, z;
            atmosphere->GetAmbientColor(&ra, &ga, &ba);
            atmosphere->GetSunOrMoonColor(&rd, &gd, &bd);
            atmosphere->GetSunOrMoonPosition(&x, &y, &z);

            light->setDiffuseColour(rd, gd, bd);
            light->setSpecularColour(0, 0, 0);
            sceneMgr->setAmbientLight(ColourValue(ra, ga, ba));
            light->setDirection(x, y, z);
        }
    }

    void SetFog(void)
    {
        if (!atmosphere || !sceneMgr) return;

        float density, r, g, b;
        if (atmosphere->GetFogEnabled())
        {
            float ar, ag, ab;
            atmosphere->GetSunOrMoonColor(&ar, &ag, &ab);
            atmosphere->GetFogSettings(&density, &r, &g, &b);
            r *= ar; g *= ag; b *= ab;
        }
        else
        {
            atm->GetHorizonColor(&r, &g, &b);
            density = 1.0 / kVisibility;
        }
        
        sceneMgr->setFog(Ogre::FOG_EXP, ColourValue(r, g, b), density);
    }

    Ogre::SceneManager *sceneMgr;
    SilverLining::Atmosphere *atmosphere;
};

class SilverLiningApplication : public ExampleApplication
{
public:
    SilverLiningApplication() {}

protected:

    void SetupSilverLining(void)
    {
        // Obtain the D3D device, if we're using D3D.
        void *pD3DDevice = 0; 
        mWindow->getCustomAttribute("D3DDEVICE",&pD3DDevice);

        // Substitute the path to your SilverLining resources directory below
        int err = atm->Initialize(pD3DDevice == NULL ? SilverLining::Atmosphere::OPENGL : SilverLining::Atmosphere::DIRECTX9, 
            "c:\\program files\\SilverLining SDK\\resources\\",
            true, pD3DDevice);

        if (err == SilverLining::Atmosphere::E_NOERROR)
        {
            // Necessary, since we can't do pixel read-back from Ogre's backbuffer
            atm->SetConfigOption("render-offscreen", "yes");

            atm->GetConditions()->SetVisibility(kVisibility);

            // Set our location (change this to your own latitude and longitude)
            SilverLining::Location loc;
            loc.SetAltitude(0);
            loc.SetLatitude(45);
            loc.SetLongitude(-122);
            atm->GetConditions()->SetLocation(loc);

            // Set the time to the current system time (or you could set it explicitly to a fixed time)
            SilverLining::LocalTime t;
            t.SetFromSystemTime();
            t.SetTimeZone(PST); // It's important that your time zone is consistent with your location above
            atm->GetConditions()->SetTime(t);

            // Throw in some cumulus congestus clouds
            SilverLining::CloudLayer *cumulusCongestusLayer;
            cumulusCongestusLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS);
            cumulusCongestusLayer->SetBaseAltitude(4000);
            cumulusCongestusLayer->SetThickness(500);
            cumulusCongestusLayer->SetBaseLength(40000);
            cumulusCongestusLayer->SetBaseWidth(40000);
            cumulusCongestusLayer->SetDensity(0.3);
            cumulusCongestusLayer->SetLayerPosition(0, 0);
            cumulusCongestusLayer->SeedClouds(*atm);
            cumulusCongestusLayer->GenerateShadowMaps(false);
            atm->GetConditions()->AddCloudLayer(cumulusCongestusLayer);

            // A little cirrus too
            SilverLining::CloudLayer *cirrusCloudLayer;
            cirrusCloudLayer = SilverLining::CloudLayerFactory::Create(CIRRUS_FIBRATUS);
            cirrusCloudLayer->SetBaseAltitude(8000);
            cirrusCloudLayer->SetThickness(0);
            cirrusCloudLayer->SetBaseLength(200000);
            cirrusCloudLayer->SetBaseWidth(200000);
            cirrusCloudLayer->SetLayerPosition(0, 0);
            cirrusCloudLayer->SeedClouds(*atm);
            atm->GetConditions()->AddCloudLayer(cirrusCloudLayer);
        }
    }

    void createViewports(void)
    {
        Viewport* vp = mWindow->addViewport(mCamera);

        // Since the sky will clear the frame, don't waste time by clearing 
        // the viewport.
        vp->setClearEveryFrame(false, 0);

        mCamera->setAspectRatio(
            Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    }

    void createScene(void)
    {
        // Initialize SilverLining and set up some clouds and conditions.
        SetupSilverLining();

        // Inject hooks for SilverLining
        MyRenderQueueListener *myListener = 0;
        myListener = new MyRenderQueueListener(mSceneMgr, atm);
        mSceneMgr->addRenderQueueListener(myListener);

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setType(Ogre::Light::LT_DIRECTIONAL);

        // Add in a model to illustrate lighting
        Entity *ent = mSceneMgr->createEntity( "razor", "razor.mesh" );
        mSceneMgr->getRootSceneNode()->attachObject( ent );
    }

};
