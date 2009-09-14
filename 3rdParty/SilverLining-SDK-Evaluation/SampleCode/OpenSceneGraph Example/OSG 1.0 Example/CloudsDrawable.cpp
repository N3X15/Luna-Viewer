// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#include "CloudsDrawable.h"
#include "SilverLining.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

using namespace SilverLining;

CloudsDrawable::CloudsDrawable()
        : osg::Drawable()
        , _view(0)
        , _atmosphere(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

CloudsDrawable::CloudsDrawable(osgProducer::Viewer* view, SilverLining::Atmosphere* atm)
        : osg::Drawable()
        , _view(view)
        , _atmosphere(atm)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

void CloudsDrawable::drawImplementation(osg::State& renderInfo) const
{
	renderInfo.disableAllVertexArrays();

    if (_atmosphere)
    {
        _atmosphere->EndFrame(true);
    }

	renderInfo.dirtyAllVertexArrays();
}
