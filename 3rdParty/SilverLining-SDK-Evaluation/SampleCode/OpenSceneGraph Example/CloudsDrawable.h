// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#pragma once

#include <osg/Drawable>
#include <osgViewer/Viewer>
#include "SilverLining.h"

class SilverLining::Atmosphere;

class CloudsDrawable : public osg::Drawable
{
public:
    CloudsDrawable();
	CloudsDrawable(osgViewer::Viewer* view);

    virtual bool isSameKindAs(const Object* obj) const {
        return dynamic_cast<const CloudsDrawable*>(obj)!=NULL;
    }
    virtual Object* cloneType() const {
        return new CloudsDrawable();
    }
    virtual Object* clone(const osg::CopyOp& copyop) const {
        return new CloudsDrawable();
    }

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:
    osgViewer::Viewer* _view;
};
