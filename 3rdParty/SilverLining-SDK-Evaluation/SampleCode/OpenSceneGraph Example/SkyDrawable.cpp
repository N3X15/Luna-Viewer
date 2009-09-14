// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#include "SkyDrawable.h"
#include "SilverLining.h"
#include "AtmosphereReference.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

using namespace SilverLining;

SkyDrawable::SkyDrawable()
        : osg::Drawable()
		, _view(0)
		, _skyboxSize(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

SkyDrawable::SkyDrawable(osgViewer::Viewer* view)
        : osg::Drawable()
        , _view(view)
		, _skyboxSize(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

void SkyDrawable::setLighting(SilverLining::Atmosphere *atmosphere) const
{
    osg::Light *light = _view->getLight();
    osg::Vec4 ambient, diffuse;
    osg::Vec3 direction;

    if (atmosphere && light)
    {
        float ra, ga, ba, rd, gd, bd, x, y, z;
        atmosphere->GetAmbientColor(&ra, &ga, &ba);
        atmosphere->GetSunOrMoonColor(&rd, &gd, &bd);
        atmosphere->GetSunOrMoonPosition(&x, &y, &z);

        direction = osg::Vec3(x, y, z);
        ambient = osg::Vec4(ra, ga, ba, 1.0);
        diffuse = osg::Vec4(rd, gd, bd, 1.0);

        // xform the light direction into camera coordinates
        osg::Quat view = _view->getCamera()->getViewMatrix().getRotate();
        direction = view * direction;
        direction.normalize();

        light->setAmbient(ambient);
        light->setDiffuse(diffuse);
        light->setSpecular(osg::Vec4(0,0,0,1));
        light->setPosition(osg::Vec4(direction.x(), direction.y(), direction.z(), 0));
    }
}

void SkyDrawable::initializeSilverLining(AtmosphereReference *ar) const
{
    if (ar && !ar->atmosphereInitialized)
    {
        ar->atmosphereInitialized = true; // only try once.
		SilverLining::Atmosphere *atmosphere = ar->atmosphere;

		if (atmosphere)
        {
			srand(1234); // constant random seed to ensure consistent clouds across windows

            // Update the path below to where you installed SilverLining's resources folder.
            int ret = atmosphere->Initialize(SilverLining::Atmosphere::OPENGL, "C:\\Program Files\\SilverLining SDK\\resources\\",
                                             true, 0);
            if (ret != SilverLining::Atmosphere::E_NOERROR)
            {
                printf("SilverLining failed to initialize; error code %d.\n", ret);
                printf("Check that the path to the SilverLining installation directory is set properly ");
                printf("in SkyDrawable.cpp (in SkyDrawable::initializeSilverLining)\n");
                exit(0);
            }

            // Let SilverLining know which way is up. OSG usually has Z going up.
            atmosphere->SetUpVector(0, 0, 1);
            atmosphere->SetRightVector(1, 0, 0);

            //atmosphere->ShowFramerate(true);

            // Set our location (change this to your own latitude and longitude)
            SilverLining::Location loc;
            loc.SetAltitude(0);
            loc.SetLatitude(45);
            loc.SetLongitude(-122);
            atmosphere->GetConditions()->SetLocation(loc);

            // Set the time to noon in PST
            SilverLining::LocalTime t;
            t.SetFromSystemTime();
			t.SetHour(12);
			t.SetTimeZone(PST);
            atmosphere->GetConditions()->SetTime(t);

            // Center the clouds around the camera's initial position
            osg::Vec3d pos = _view->getCameraManipulator()->getMatrix().getTrans();

            SilverLining::CloudLayer *cumulusCongestusLayer;
            cumulusCongestusLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS);
            cumulusCongestusLayer->SetBaseAltitude(4000);
            cumulusCongestusLayer->SetThickness(500);
            cumulusCongestusLayer->SetBaseLength(40000);
            cumulusCongestusLayer->SetBaseWidth(40000);
            cumulusCongestusLayer->SetDensity(0.3);

            // Note, we pass in X and -Y since this accepts "east" and "south" coordinates.
            cumulusCongestusLayer->SetLayerPosition(pos.x(), -pos.y());
            cumulusCongestusLayer->SeedClouds(*atmosphere);
            cumulusCongestusLayer->GenerateShadowMaps(false);

            atmosphere->GetConditions()->AddCloudLayer(cumulusCongestusLayer);
        }
    }
}

void SkyDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	SilverLining::Atmosphere *atmosphere = 0;

	AtmosphereReference *ar = dynamic_cast<AtmosphereReference *>(renderInfo.getCurrentCamera()->getUserData());
	if (ar) atmosphere = ar->atmosphere;

	renderInfo.getState()->disableAllVertexArrays();

    if (atmosphere)
    {
        initializeSilverLining(ar);
        atmosphere->BeginFrame(true, false, _skyboxSize);
        setLighting(atmosphere);
    }

	renderInfo.getState()->dirtyAllVertexArrays();
}
