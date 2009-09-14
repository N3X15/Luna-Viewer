// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#pragma once

#include <osg/Drawable>
#include <osgProducer/Viewer>
#include "SilverLining.h"

class SkyDrawable : public osg::Drawable
{
public:
	SkyDrawable();
    SkyDrawable(osgProducer::Viewer* view, SilverLining::Atmosphere* atm);

    virtual bool isSameKindAs(const Object* obj) const {
        return dynamic_cast<const SkyDrawable*>(obj)!=NULL;
    }
    virtual Object* cloneType() const {
        return new SkyDrawable();
    }
    virtual Object* clone(const osg::CopyOp& copyop) const {
        return new SkyDrawable();
    }

	void setSkyboxSize(double size) {_skyboxSize = size;}

    virtual void drawImplementation(osg::State& renderInfo) const;

protected:
    void setLighting(SilverLining::Atmosphere *atm, osg::State& state) const;
    void initializeSilverLining() const;

    osgProducer::Viewer* _view;
    SilverLining::Atmosphere* _atmosphere;
	double _skyboxSize;
};
