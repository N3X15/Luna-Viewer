// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#pragma once

#include <osg/Drawable>
#include <osgProducer/Viewer>
#include "SilverLining.h"

class SilverLining::Atmosphere;

class CloudsDrawable : public osg::Drawable
{
public:
    CloudsDrawable();
    CloudsDrawable(osgProducer::Viewer* view, SilverLining::Atmosphere* atm);

    virtual bool isSameKindAs(const Object* obj) const {
        return dynamic_cast<const CloudsDrawable*>(obj)!=NULL;
    }
    virtual Object* cloneType() const {
        return new CloudsDrawable();
    }
    virtual Object* clone(const osg::CopyOp& copyop) const {
        return new CloudsDrawable();
    }

    virtual void drawImplementation(osg::State& renderInfo) const;

protected:
    osgProducer::Viewer* _view;
    SilverLining::Atmosphere* _atmosphere;
};
