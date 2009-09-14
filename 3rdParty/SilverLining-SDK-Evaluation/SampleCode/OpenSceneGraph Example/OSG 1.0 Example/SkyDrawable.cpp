// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#include "SkyDrawable.h"
#include "SilverLining.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

using namespace SilverLining;

SkyDrawable::SkyDrawable()
        : osg::Drawable()
		, _view(0)
		, _skyboxSize(0)
        , _atmosphere(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

SkyDrawable::SkyDrawable(osgProducer::Viewer* view, SilverLining::Atmosphere* atm)
        : osg::Drawable()
        , _view(view)
		, _skyboxSize(0)
        , _atmosphere(atm)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

void SkyDrawable::setLighting(SilverLining::Atmosphere *atmosphere, osg::State& state) const
{
    const osg::Light *oldLight = dynamic_cast<const osg::Light*>(state.getGlobalDefaultAttribute(osg::StateAttribute::LIGHT));
    osg::Light *light = new osg::Light(*oldLight);

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
        if (_view)
        {
            osg::Matrixd rotate = _view->getViewMatrix();            
            rotate.translate(0, 0, 0);
            direction = direction * rotate;
            direction.normalize();

            light->setAmbient(ambient);
            light->setDiffuse(diffuse);
            light->setSpecular(osg::Vec4(0,0,0,1));
            light->setPosition(osg::Vec4(direction.x(), direction.y(), direction.z(), 0));
  
            state.setGlobalDefaultAttribute(light);
        }
    }
}

static bool atmosphereInited = false;

void SkyDrawable::initializeSilverLining() const
{
    if (!atmosphereInited)
    {
        atmosphereInited = true;

		if (_atmosphere)
        {
			srand(1234); // constant random seed to ensure consistent clouds across windows

            // Update the path below to where you installed SilverLining's resources folder.

            char *slPath = getenv("SILVERLINING_PATH");
            std::string resourcePath(slPath);
            resourcePath += "\\resources\\";
            int ret = _atmosphere->Initialize(SilverLining::Atmosphere::OPENGL, resourcePath.c_str(),
                                             true, 0);
            if (ret != SilverLining::Atmosphere::E_NOERROR)
            {
                printf("SilverLining failed to initialize; error code %d.\n", ret);
                printf("Check that the path to the SilverLining installation directory is set properly ");
                printf("in SkyDrawable.cpp (in SkyDrawable::initializeSilverLining)\n");
                exit(0);
            }

            // Let SilverLining know which way is up. OSG usually has Z going up.
            _atmosphere->SetUpVector(0, 0, 1);
            _atmosphere->SetRightVector(1, 0, 0);

            //_atmosphere->ShowFramerate(true);

            // Set our location (change this to your own latitude and longitude)
            SilverLining::Location loc;
            loc.SetAltitude(0);
            loc.SetLatitude(45);
            loc.SetLongitude(-122);
            _atmosphere->GetConditions()->SetLocation(loc);

            // Set the time to noon in PST
            SilverLining::LocalTime t;
            t.SetFromSystemTime();
			t.SetHour(12);
			t.SetTimeZone(PST);
            _atmosphere->GetConditions()->SetTime(t);

            SilverLining::CloudLayer *cumulusCongestusLayer;
            cumulusCongestusLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS_INFINITE);
            cumulusCongestusLayer->SetBaseAltitude(4000);
            cumulusCongestusLayer->SetThickness(500);
            cumulusCongestusLayer->SetBaseLength(40000);
            cumulusCongestusLayer->SetBaseWidth(40000);
            cumulusCongestusLayer->SetDensity(0.3);
            cumulusCongestusLayer->SeedClouds(*_atmosphere);
            cumulusCongestusLayer->GenerateShadowMaps(false);

            _atmosphere->GetConditions()->AddCloudLayer(cumulusCongestusLayer);
        }
    }
}

void SkyDrawable::drawImplementation(osg::State& state) const
{
	state.disableAllVertexArrays();

    if (_atmosphere)
    {
        initializeSilverLining();
        _atmosphere->BeginFrame(true, false, _skyboxSize);
        setLighting(_atmosphere, state);
    }

	state.dirtyAllVertexArrays();
}
