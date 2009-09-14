// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#pragma once

#include <osg/Drawable>
#include <osgViewer/Viewer>
#include "SilverLining.h"

class AtmosphereReference;

class SkyDrawable : public osg::Drawable
{
public:
	SkyDrawable();
	SkyDrawable(osgViewer::Viewer* view);

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

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:
	void setLighting(SilverLining::Atmosphere *atm) const;
    void initializeSilverLining(AtmosphereReference *ar) const;

    osgViewer::Viewer* _view;
	double _skyboxSize;
};
