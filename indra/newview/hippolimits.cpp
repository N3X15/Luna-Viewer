#include "llviewerprecompiledheaders.h"

#include "hippolimits.h"

#include "hippogridmanager.h"

#include "llerror.h"

#include "llviewercontrol.h"		// gSavedSettings

HippoLimits *gHippoLimits = 0;


HippoLimits::HippoLimits()
{
	setLimits();
}


void HippoLimits::setLimits()
{
	if (gHippoGridManager->getConnectedGrid()->getPlatform() == HippoGridInfo::PLATFORM_SECONDLIFE) {
		setSecondLifeLimits();
	} else {
		setOpenSimLimits();
	}
}


void HippoLimits::setOpenSimLimits()
{
	mMaxAgentGroups = gHippoGridManager->getConnectedGrid()->getMaxAgentGroups();
	if (mMaxAgentGroups < 0) mMaxAgentGroups = 50;
	mMaxPrimScale = 256.0f;
	mMaxHeight = 10000.0f;
	if (gHippoGridManager->getConnectedGrid()->isRenderCompat()) {
		llinfos << "Using rendering compatible OpenSim limits." << llendl;
		mMinHoleSize = 0.05f;
		mMaxHollow = 0.95f;
	} else {
		llinfos << "Using Hippo OpenSim limits." << llendl;
		mMinHoleSize = 0.01f;
		mMaxHollow = 0.99f;
	}
}

void HippoLimits::setSecondLifeLimits()
{
	llinfos << "Using Second Life limits." << llendl;
	
	if(gSavedSettings.getBOOL("Phoenix40GroupsSupport")) {
		mMaxAgentGroups = 40;
	} else {
		mMaxAgentGroups = 25;
	}
	
	if(gSavedSettings.getBOOL("Phoenix64mPrimSupport")) {
		mMaxPrimScale = 64.0f;
	} else {
		mMaxPrimScale = 10.0f;
	}
	
	mMaxHeight = 4096.0f;
	mMinHoleSize = 0.05f;
	mMaxHollow = 0.95f;
}

