// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#include "CloudsDrawable.h"
#include "SilverLining.h"
#include "AtmosphereReference.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

using namespace SilverLining;

CloudsDrawable::CloudsDrawable()
        : osg::Drawable()
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

CloudsDrawable::CloudsDrawable(osgViewer::Viewer* view)
        : osg::Drawable()
        , _view(view)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

void CloudsDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	AtmosphereReference *ar = dynamic_cast<AtmosphereReference *>(renderInfo.getCurrentCamera()->getUserData());
	SilverLining::Atmosphere *atmosphere = 0;
	if (ar) atmosphere = ar->atmosphere;

	renderInfo.getState()->disableAllVertexArrays();

    if (atmosphere)
    {
        atmosphere->EndFrame(true);
    }

	renderInfo.getState()->dirtyAllVertexArrays();
}
