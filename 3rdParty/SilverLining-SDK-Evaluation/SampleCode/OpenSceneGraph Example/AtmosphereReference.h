// Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.

#pragma once

#include "SilverLining.h"
#include <osg/Referenced>

class AtmosphereReference : public osg::Referenced
{
public:
	SilverLining::Atmosphere *atmosphere;
	bool atmosphereInitialized;

	AtmosphereReference() : atmosphereInitialized(false), atmosphere(0) {}
};
