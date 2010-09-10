/** 
 * @file llworld.cpp
 * @brief Initial test structure to organize viewer regions
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llworld.h"
#include "llrender.h"

#include "indra_constants.h"
#include "llstl.h"

#include "llagent.h"
#include "llviewercontrol.h"
#include "lldrawpool.h"
#include "llglheaders.h"
#include "llhttpnode.h"
#include "llregionhandle.h"
#include "llsurface.h"
#include "llviewercamera.h"
#include "llviewerimage.h"
#include "llviewerimagelist.h"
#include "llviewernetwork.h"
#include "llviewerobjectlist.h"
#include "llviewerparceloverlay.h"
#include "llviewerregion.h"
#include "llviewerstats.h"
#include "llvlcomposition.h"
#include "llvoavatar.h"
#include "llvowater.h"
#include "message.h"
#include "pipeline.h"
#include "llappviewer.h"		// for do_disconnect()

#include <deque>
#include <queue>
#include <map>
#include <cstring>

//#include "flsky.h"

//
// Globals
//
U32			gAgentPauseSerialNum = 0;

//
// Constants
//
const S32 MAX_NUMBER_OF_CLOUDS	= 750;
const S32 WORLD_PATCH_SIZE = 16;

extern LLColor4U MAX_WATER_COLOR;

const U32 LLWorld::mWidth = 256;

// meters/point, therefore mWidth * mScale = meters per edge
const F32 LLWorld::mScale = 1.f;

const F32 LLWorld::mWidthInMeters = mWidth * mScale;

//
// Functions
//

// allocate the stack
LLWorld::LLWorld() :
	mLandFarClip(DEFAULT_FAR_PLANE),
	mLastPacketsIn(0),
	mLastPacketsOut(0),
	mLastPacketsLost(0),
	mSpaceTimeUSec(0)
{
	for (S32 i = 0; i < 8; i++)
	{
		mEdgeWaterObjects[i] = NULL;
	}

	if (gNoRender)
	{
		return;
	}

	LLPointer<LLImageRaw> raw = new LLImageRaw(1,1,4);
	U8 *default_texture = raw->getData();
	*(default_texture++) = MAX_WATER_COLOR.mV[0];
	*(default_texture++) = MAX_WATER_COLOR.mV[1];
	*(default_texture++) = MAX_WATER_COLOR.mV[2];
	*(default_texture++) = MAX_WATER_COLOR.mV[3];
	
	mDefaultWaterTexturep = new LLViewerImage(raw, FALSE);
	gGL.getTexUnit(0)->bind(mDefaultWaterTexturep.get());
	mDefaultWaterTexturep->setAddressMode(LLTexUnit::TAM_CLAMP);

}


void LLWorld::destroyClass()
{
	gObjectList.destroy();
	for(region_list_t::iterator region_it = mRegionList.begin(); region_it != mRegionList.end(); )
	{
		LLViewerRegion* region_to_delete = *region_it++;
		removeRegion(region_to_delete->getHost());
	}
	LLViewerPartSim::getInstance()->destroyClass();
}


LLViewerRegion* LLWorld::addRegion(const U64 &region_handle, const LLHost &host)
{
	LLMemType mt(LLMemType::MTYPE_REGIONS);
	
	LLViewerRegion *regionp = getRegionFromHandle(region_handle);
	if (regionp)
	{
		LLHost old_host = regionp->getHost();
		// region already exists!
		if (host == old_host && regionp->isAlive())
		{
			// This is a duplicate for the same host and it's alive, don't bother.
			return regionp;
		}

		if (host != old_host)
		{
			llwarns << "LLWorld::addRegion exists, but old host " << old_host
					<< " does not match new host " << host << llendl;
		}
		if (!regionp->isAlive())
		{
			llwarns << "LLWorld::addRegion exists, but isn't alive" << llendl;
		}

		// Kill the old host, and then we can continue on and add the new host.  We have to kill even if the host
		// matches, because all the agent state for the new camera is completely different.
		removeRegion(old_host);
	}

	U32 iindex = 0;
	U32 jindex = 0;
	from_region_handle(region_handle, &iindex, &jindex);
	S32 x = (S32)(iindex/mWidth);
	S32 y = (S32)(jindex/mWidth);
	llinfos << "Adding new region (" << x << ":" << y << ")" << llendl;
	llinfos << "Host: " << host << llendl;

	LLVector3d origin_global;

	origin_global = from_region_handle(region_handle);

	regionp = new LLViewerRegion(region_handle,
								    host,
									mWidth,
									WORLD_PATCH_SIZE,
									getRegionWidthInMeters() );
	if (!regionp)
	{
		llerrs << "Unable to create new region!" << llendl;
	}

	//regionp->mVolumeClouds=FLSky::Create(regionp);
//	regionp->mVolumeClouds->setWidth((F32)mWidth);
//	regionp->mVolumeClouds->setWindPointer(&regionp->mWind);

	regionp->mCloudLayer.create(regionp);
	regionp->mCloudLayer.setWidth((F32)mWidth);
	regionp->mCloudLayer.setWindPointer(&regionp->mWind);

	mRegionList.push_back(regionp);
	mActiveRegionList.push_back(regionp);
	mCulledRegionList.push_back(regionp);


	// Find all the adjacent regions, and attach them.
	// Generate handles for all of the adjacent regions, and attach them in the correct way.
	// connect the edges
	F32 adj_x = 0.f;
	F32 adj_y = 0.f;
	F32 region_x = 0.f;
	F32 region_y = 0.f;
	U64 adj_handle = 0;

	F32 width = getRegionWidthInMeters();

	LLViewerRegion *neighborp;
	from_region_handle(region_handle, &region_x, &region_y);

	// Iterate through all directions, and connect neighbors if there.
	S32 dir;
	for (dir = 0; dir < 8; dir++)
	{
		adj_x = region_x + width * gDirAxes[dir][0];
		adj_y = region_y + width * gDirAxes[dir][1];
		to_region_handle(adj_x, adj_y, &adj_handle);

		neighborp = getRegionFromHandle(adj_handle);
		if (neighborp)
		{
			//llinfos << "Connecting " << region_x << ":" << region_y << " -> " << adj_x << ":" << adj_y << llendl;
			regionp->connectNeighbor(neighborp, dir);
		}
	}

	updateWaterObjects();

	return regionp;
}


void LLWorld::removeRegion(const LLHost &host)
{
	F32 x, y;

	LLViewerRegion *regionp = getRegion(host);
	if (!regionp)
	{
		llwarns << "Trying to remove region that doesn't exist!" << llendl;
		return;
	}
	
	if (regionp == gAgent.getRegion())
	{
		for (region_list_t::iterator iter = mRegionList.begin();
			 iter != mRegionList.end(); ++iter)
		{
			LLViewerRegion* reg = *iter;
			llwarns << "RegionDump: " << reg->getName()
				<< " " << reg->getHost()
				<< " " << reg->getOriginGlobal()
				<< llendl;
		}

		llwarns << "Agent position global " << gAgent.getPositionGlobal() 
			<< " agent " << gAgent.getPositionAgent()
			<< llendl;

		llwarns << "Regions visited " << gAgent.getRegionsVisited() << llendl;

		llwarns << "gFrameTimeSeconds " << gFrameTimeSeconds << llendl;

		llwarns << "Disabling region " << regionp->getName() << " that agent is in!" << llendl;
		LLAppViewer::instance()->forceDisconnect("You have been disconnected from the region you were in.");
		return;
	}

	from_region_handle(regionp->getHandle(), &x, &y);
	llinfos << "Removing region " << x << ":" << y << llendl;

	mRegionList.remove(regionp);
	mActiveRegionList.remove(regionp);
	mCulledRegionList.remove(regionp);
	mVisibleRegionList.remove(regionp);
	
	delete regionp;

	updateWaterObjects();
}


LLViewerRegion* LLWorld::getRegion(const LLHost &host)
{
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		if (regionp->getHost() == host)
		{
			return regionp;
		}
	}
	return NULL;
}

LLViewerRegion* LLWorld::getRegionFromPosAgent(const LLVector3 &pos)
{
	return getRegionFromPosGlobal(gAgent.getPosGlobalFromAgent(pos));
}

LLViewerRegion* LLWorld::getRegionFromPosGlobal(const LLVector3d &pos)
{
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		if (regionp->pointInRegionGlobal(pos))
		{
			return regionp;
		}
	}
	return NULL;
}


LLVector3d	LLWorld::clipToVisibleRegions(const LLVector3d &start_pos, const LLVector3d &end_pos)
{
	if (positionRegionValidGlobal(end_pos))
	{
		return end_pos;
	}

	LLViewerRegion* regionp = getRegionFromPosGlobal(start_pos);
	if (!regionp) 
	{
		return start_pos;
	}

	LLVector3d delta_pos = end_pos - start_pos;
	LLVector3d delta_pos_abs;
	delta_pos_abs.setVec(delta_pos);
	delta_pos_abs.abs();

	LLVector3 region_coord = regionp->getPosRegionFromGlobal(end_pos);
	F64 clip_factor = 1.0;
	F32 region_width = regionp->getWidth();
	if (region_coord.mV[VX] < 0.f)
	{
		if (region_coord.mV[VY] < region_coord.mV[VX])
		{
			// clip along y -
			clip_factor = -(region_coord.mV[VY] / delta_pos_abs.mdV[VY]);
		}
		else
		{
			// clip along x -
			clip_factor = -(region_coord.mV[VX] / delta_pos_abs.mdV[VX]);
		}
	}
	else if (region_coord.mV[VX] > region_width)
	{
		if (region_coord.mV[VY] > region_coord.mV[VX])
		{
			// clip along y +
			clip_factor = (region_coord.mV[VY] - region_width) / delta_pos_abs.mdV[VY];
		}
		else
		{
			//clip along x +
			clip_factor = (region_coord.mV[VX] - region_width) / delta_pos_abs.mdV[VX];
		}
	}
	else if (region_coord.mV[VY] < 0.f)
	{
		// clip along y -
		clip_factor = -(region_coord.mV[VY] / delta_pos_abs.mdV[VY]);
	}
	else if (region_coord.mV[VY] > region_width)
	{ 
		// clip along y +
		clip_factor = (region_coord.mV[VY] - region_width) / delta_pos_abs.mdV[VY];
	}

	// clamp to < 256 to stay in sim
	LLVector3d final_region_pos = LLVector3d(region_coord) - (delta_pos * clip_factor);
	final_region_pos.clamp(0.0, 255.999);
	return regionp->getPosGlobalFromRegion(LLVector3(final_region_pos));
}

LLViewerRegion* LLWorld::getRegionFromHandle(const U64 &handle)
{
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		if (regionp->getHandle() == handle)
		{
			return regionp;
		}
	}
	return NULL;
}


void LLWorld::updateAgentOffset(const LLVector3d &offset_global)
{
#if 0
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		regionp->setAgentOffset(offset_global);
	}
#endif
}


BOOL LLWorld::positionRegionValidGlobal(const LLVector3d &pos_global)
{
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		if (regionp->pointInRegionGlobal(pos_global))
		{
			return TRUE;
		}
	}
	return FALSE;
}


// Allow objects to go up to their radius underground.
F32 LLWorld::getMinAllowedZ(LLViewerObject* object)
{
	F32 land_height = resolveLandHeightGlobal(object->getPositionGlobal());
	F32 radius = 0.5f * object->getScale().length();
	return land_height - radius;
}


LLViewerRegion* LLWorld::resolveRegionGlobal(LLVector3 &pos_region, const LLVector3d &pos_global)
{
	LLViewerRegion *regionp = getRegionFromPosGlobal(pos_global);

	if (regionp)
	{
		pos_region = regionp->getPosRegionFromGlobal(pos_global);
		return regionp;
	}

	return NULL;
}


LLViewerRegion* LLWorld::resolveRegionAgent(LLVector3 &pos_region, const LLVector3 &pos_agent)
{
	LLVector3d pos_global = gAgent.getPosGlobalFromAgent(pos_agent);
	LLViewerRegion *regionp = getRegionFromPosGlobal(pos_global);

	if (regionp)
	{
		pos_region = regionp->getPosRegionFromGlobal(pos_global);
		return regionp;
	}

	return NULL;
}


F32 LLWorld::resolveLandHeightAgent(const LLVector3 &pos_agent)
{
	LLVector3d pos_global = gAgent.getPosGlobalFromAgent(pos_agent);
	return resolveLandHeightGlobal(pos_global);
}


F32 LLWorld::resolveLandHeightGlobal(const LLVector3d &pos_global)
{
	LLViewerRegion *regionp = getRegionFromPosGlobal(pos_global);
	if (regionp)
	{
		return regionp->getLand().resolveHeightGlobal(pos_global);
	}
	return 0.0f;
}


// Takes a line defined by "point_a" and "point_b" and determines the closest (to point_a) 
// point where the the line intersects an object or the land surface.  Stores the results
// in "intersection" and "intersection_normal" and returns a scalar value that represents
// the normalized distance along the line from "point_a" to "intersection".
//
// Currently assumes point_a and point_b only differ in z-direction, 
// but it may eventually become more general.
F32 LLWorld::resolveStepHeightGlobal(const LLVOAvatar* avatarp, const LLVector3d &point_a, const LLVector3d &point_b, 
							   LLVector3d &intersection, LLVector3 &intersection_normal,
							   LLViewerObject **viewerObjectPtr)
{
	// initialize return value to null
	if (viewerObjectPtr)
	{
		*viewerObjectPtr = NULL;
	}

	LLViewerRegion *regionp = getRegionFromPosGlobal(point_a);
	if (!regionp)
	{
		// We're outside the world 
		intersection = 0.5f * (point_a + point_b);
		intersection_normal.setVec(0.0f, 0.0f, 1.0f);
		return 0.5f;
	}
	
	// calculate the length of the segment
	F32 segment_length = (F32)((point_a - point_b).length());
	if (0.0f == segment_length)
	{
		intersection = point_a;
		intersection_normal.setVec(0.0f, 0.0f, 1.0f);
		return segment_length;
	}

	// get land height	
	// Note: we assume that the line is parallel to z-axis here
	LLVector3d land_intersection = point_a;
	F32 normalized_land_distance;

	land_intersection.mdV[VZ] = regionp->getLand().resolveHeightGlobal(point_a);
	normalized_land_distance = (F32)(point_a.mdV[VZ] - land_intersection.mdV[VZ]) / segment_length;
	intersection = land_intersection;
	intersection_normal = resolveLandNormalGlobal(land_intersection);

	if (avatarp && !avatarp->mFootPlane.isExactlyClear())
	{
		LLVector3 foot_plane_normal(avatarp->mFootPlane.mV);
		LLVector3 start_pt = avatarp->getRegion()->getPosRegionFromGlobal(point_a);
		// added 0.05 meters to compensate for error in foot plane reported by Havok
		F32 norm_dist_from_plane = ((start_pt * foot_plane_normal) - avatarp->mFootPlane.mV[VW]) + 0.05f;
		norm_dist_from_plane = llclamp(norm_dist_from_plane / segment_length, 0.f, 1.f);
		if (norm_dist_from_plane < normalized_land_distance)
		{
			// collided with object before land
			normalized_land_distance = norm_dist_from_plane;
			intersection = point_a;
			intersection.mdV[VZ] -= norm_dist_from_plane * segment_length;
			intersection_normal = foot_plane_normal;
		}
		else
		{
			intersection = land_intersection;
			intersection_normal = resolveLandNormalGlobal(land_intersection);
		}
	}

	return normalized_land_distance;
}


LLSurfacePatch * LLWorld::resolveLandPatchGlobal(const LLVector3d &pos_global)
{
	//  returns a pointer to the patch at this location
	LLViewerRegion *regionp = getRegionFromPosGlobal(pos_global);
	if (!regionp)
	{
		return NULL;
	}

	return regionp->getLand().resolvePatchGlobal(pos_global);
}


LLVector3 LLWorld::resolveLandNormalGlobal(const LLVector3d &pos_global)
{
	LLViewerRegion *regionp = getRegionFromPosGlobal(pos_global);
	if (!regionp)
	{
		return LLVector3::z_axis;
	}

	return regionp->getLand().resolveNormalGlobal(pos_global);
}


void LLWorld::updateVisibilities()
{
	F32 cur_far_clip = LLViewerCamera::getInstance()->getFar();

	LLViewerCamera::getInstance()->setFar(mLandFarClip);

	F32 diagonal_squared = F_SQRT2 * F_SQRT2 * mWidth * mWidth;
	// Go through the culled list and check for visible regions
	for (region_list_t::iterator iter = mCulledRegionList.begin();
		 iter != mCulledRegionList.end(); )
	{
		region_list_t::iterator curiter = iter++;
		LLViewerRegion* regionp = *curiter;
		F32 height = regionp->getLand().getMaxZ() - regionp->getLand().getMinZ();
		F32 radius = 0.5f*fsqrtf(height * height + diagonal_squared);
		if (!regionp->getLand().hasZData()
			|| LLViewerCamera::getInstance()->sphereInFrustum(regionp->getCenterAgent(), radius))
		{
			mCulledRegionList.erase(curiter);
			mVisibleRegionList.push_back(regionp);
		}
	}
	
	// Update all of the visible regions 
	for (region_list_t::iterator iter = mVisibleRegionList.begin();
		 iter != mVisibleRegionList.end(); )
	{
		region_list_t::iterator curiter = iter++;
		LLViewerRegion* regionp = *curiter;
		if (!regionp->getLand().hasZData())
		{
			continue;
		}

		F32 height = regionp->getLand().getMaxZ() - regionp->getLand().getMinZ();
		F32 radius = 0.5f*fsqrtf(height * height + diagonal_squared);
		if (LLViewerCamera::getInstance()->sphereInFrustum(regionp->getCenterAgent(), radius))
		{
			regionp->calculateCameraDistance();
			if (!gNoRender)
			{
				regionp->getLand().updatePatchVisibilities(gAgent);
			}
		}
		else
		{
			mVisibleRegionList.erase(curiter);
			mCulledRegionList.push_back(regionp);
		}
	}

	// Sort visible regions
	mVisibleRegionList.sort(LLViewerRegion::CompareDistance());
	
	LLViewerCamera::getInstance()->setFar(cur_far_clip);
}

void LLWorld::updateRegions(F32 max_update_time)
{
	LLTimer update_timer;
	BOOL did_one = FALSE;
	
	// Perform idle time updates for the regions (and associated surfaces)
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		F32 max_time = max_update_time - update_timer.getElapsedTimeF32();
		if (did_one && max_time <= 0.f)
			break;
		max_time = llmin(max_time, max_update_time*.1f);
		did_one |= regionp->idleUpdate(max_update_time);
	}
}

void LLWorld::updateParticles()
{
	LLViewerPartSim::getInstance()->updateSimulation();
}

void LLWorld::updateClouds(const F32 dt)
{
	if (gSavedSettings.getBOOL("FreezeTime") ||
		!gSavedSettings.getBOOL("SkyUseClassicClouds"))
	{
		// don't move clouds in snapshot mode
		return;
	}
	if (mActiveRegionList.size())
	{
		// Update all the cloud puff positions, and timer based stuff
		// such as death decay
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			regionp->mCloudLayer.updatePuffs(dt);
		}

		// Reshuffle who owns which puffs
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			regionp->mCloudLayer.updatePuffOwnership();
		}

		// Add new puffs
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			regionp->mCloudLayer.updatePuffCount();
		}
	}
}

LLCloudGroup* LLWorld::findCloudGroup(const LLCloudPuff &puff)
{
	if (mActiveRegionList.size())
	{
		// Update all the cloud puff positions, and timer based stuff
		// such as death decay
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			LLCloudGroup *groupp = regionp->mCloudLayer.findCloudGroup(puff);
			if (groupp)
			{
				return groupp;
			}
		}
	}
	return NULL;
}

LLViewerRegion* LLWorld::findByName(std::string name)
{
	if (mActiveRegionList.size())
	{
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			if(regionp->getName()==name)
				return regionp;
		}
	}
	return NULL;
}
LLViewerRegion* LLWorld::findByUUID(LLUUID id)
{
	if (mActiveRegionList.size())
	{
		for (region_list_t::iterator iter = mActiveRegionList.begin();
			 iter != mActiveRegionList.end(); ++iter)
		{
			LLViewerRegion* regionp = *iter;
			if(regionp->getRegionID()==id)
				return regionp;
		}
	}
	return NULL;
}

void LLWorld::renderPropertyLines()
{
	S32 region_count = 0;
	S32 vertex_count = 0;

	for (region_list_t::iterator iter = mVisibleRegionList.begin();
		 iter != mVisibleRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		region_count++;
		vertex_count += regionp->renderPropertyLines();
	}
}


void LLWorld::updateNetStats()
{
	F32 bits = 0.f;
	U32 packets = 0;

	for (region_list_t::iterator iter = mActiveRegionList.begin();
		 iter != mActiveRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		regionp->updateNetStats();
		bits += regionp->mBitStat.getCurrent();
		packets += llfloor( regionp->mPacketsStat.getCurrent() );
	}

	S32 packets_in = gMessageSystem->mPacketsIn - mLastPacketsIn;
	S32 packets_out = gMessageSystem->mPacketsOut - mLastPacketsOut;
	S32 packets_lost = gMessageSystem->mDroppedPackets - mLastPacketsLost;

	S32 actual_in_bits = gMessageSystem->mPacketRing.getAndResetActualInBits();
	S32 actual_out_bits = gMessageSystem->mPacketRing.getAndResetActualOutBits();
	LLViewerStats::getInstance()->mActualInKBitStat.addValue(actual_in_bits/1024.f);
	LLViewerStats::getInstance()->mActualOutKBitStat.addValue(actual_out_bits/1024.f);
	LLViewerStats::getInstance()->mKBitStat.addValue(bits/1024.f);
	LLViewerStats::getInstance()->mPacketsInStat.addValue(packets_in);
	LLViewerStats::getInstance()->mPacketsOutStat.addValue(packets_out);
	LLViewerStats::getInstance()->mPacketsLostStat.addValue(gMessageSystem->mDroppedPackets);
	if (packets_in)
	{
		LLViewerStats::getInstance()->mPacketsLostPercentStat.addValue(100.f*((F32)packets_lost/(F32)packets_in));
	}
	else
	{
		LLViewerStats::getInstance()->mPacketsLostPercentStat.addValue(0.f);
	}

	mLastPacketsIn = gMessageSystem->mPacketsIn;
	mLastPacketsOut = gMessageSystem->mPacketsOut;
	mLastPacketsLost = gMessageSystem->mDroppedPackets;
}


void LLWorld::printPacketsLost()
{
	llinfos << "Simulators:" << llendl;
	llinfos << "----------" << llendl;

	LLCircuitData *cdp = NULL;
	for (region_list_t::iterator iter = mActiveRegionList.begin();
		 iter != mActiveRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		cdp = gMessageSystem->mCircuitInfo.findCircuit(regionp->getHost());
		if (cdp)
		{
			LLVector3d range = regionp->getCenterGlobal() - gAgent.getPositionGlobal();
				
			llinfos << regionp->getHost() << ", range: " << range.length()
					<< " packets lost: " << cdp->getPacketsLost() << llendl;
		}
	}
}

void LLWorld::processCoarseUpdate(LLMessageSystem* msg, void** user_data)
{
	LLViewerRegion* region = LLWorld::getInstance()->getRegion(msg->getSender());
	if( region )
	{
		region->updateCoarseLocations(msg);
	}
}

F32 LLWorld::getLandFarClip() const
{
	return mLandFarClip;
}

void LLWorld::setLandFarClip(const F32 far_clip)
{
	static S32 const rwidth = (S32)REGION_WIDTH_U32;
	S32 const n1 = (llceil(mLandFarClip) - 1) / rwidth;
	S32 const n2 = (llceil(far_clip) - 1) / rwidth;
	bool need_water_objects_update = n1 != n2;

	mLandFarClip = far_clip;

	if (need_water_objects_update)
	{
		updateWaterObjects();
	}
}

// Some region that we're connected to, but not the one we're in, gave us
// a (possibly) new water height. Update it in our local copy.
void LLWorld::waterHeightRegionInfo(std::string const& sim_name, F32 water_height)
{
	for (region_list_t::iterator iter = mRegionList.begin(); iter != mRegionList.end(); ++iter)
	{
		if ((*iter)->getName() == sim_name)
		{
			(*iter)->setWaterHeight(water_height);
			break;
		}
	}
}

// There are three types of water objects:
// Region water objects: the water in a region.
// Hole water objects: water in the void but within current draw distance.
// Edge water objects: the water outside the draw distance, up till the horizon.
//
// For example:
//
// -----------------------horizon-------------------------
// |                 |                 |                 |
// |  Edge Water     |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |      rwidth     |                 |
// |                 |     <----->     |                 |
// -------------------------------------------------------
// |                 |Hole |other|     |                 |
// |                 |Water|reg. |     |                 |
// |                 |-----------------|                 |
// |                 |other|cur. |<--> |                 |
// |                 |reg. | reg.|  \__|_ draw distance  |
// |                 |-----------------|                 |
// |                 |     |     |<--->|                 |
// |                 |     |     |  \__|_ range          |
// -------------------------------------------------------
// |                 |<----width------>|<--horizon ext.->|
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// |                 |                 |                 |
// -------------------------------------------------------
//
void LLWorld::updateWaterObjects()
{
	if (!gAgent.getRegion())
	{
		return;
	}
	if (mRegionList.empty())
	{
		llwarns << "No regions!" << llendl;
		return;
	}

	// Region width in meters.
	S32 const rwidth = (S32)REGION_WIDTH_U32;

	// The distance we might see into the void
	// when standing on the edge of a region, in meters.
	S32 const draw_distance = llceil(mLandFarClip);

	// We can only have "holes" in the water (where there no region) if we
	// can have existing regions around it. Taking into account that this
	// code is only executed when we enter a region, and not when we walk
	// around in it, we (only) need to take into account regions that fall
	// within the draw_distance.
	//
	// Set 'range' to draw_distance, rounded up to the nearest multiple of rwidth.
	S32 const nsims = (draw_distance + rwidth - 1) / rwidth;
	S32 const range = nsims * rwidth;

	// Get South-West corner of current region.
	LLViewerRegion const* regionp = gAgent.getRegion();
	U32 region_x, region_y;
	from_region_handle(regionp->getHandle(), &region_x, &region_y);

	// The min. and max. coordinates of the South-West corners of the Hole water objects.
	S32 const min_x = (S32)region_x - range;
	S32 const min_y = (S32)region_y - range;
	S32 const max_x = (S32)region_x + range;
	S32 const max_y = (S32)region_y + range;

	// Attempt to determine a sensible water height for all the
	// Hole Water objects.
	//
	// It make little sense to try to guess what the best water
	// height should be when that isn't completely obvious: if it's
	// impossible to satisfy every region's water height without
	// getting a jump in the water height.
	//
	// In order to keep the reasoning simple, we assume something
	// logical as a group of connected regions, where the coastline
	// is at the outer edge. Anything more complex that would "break"
	// under such an assumption would probably break anyway (would
	// depend on terrain editing and existing mega prims, say, if
	// anything would make sense at all).
	//
	// So, what we do is find all connected regions within the
	// draw distance that border void, and then pick the lowest
	// water height of those (coast) regions.
	S32 const n = 2 * nsims + 1;
	S32 const origin = nsims + nsims * n;
	std::vector<F32> water_heights(n * n);
	std::vector<U8> checked(n * n, 0);		// index = nx + ny * n + origin;
	U8 const region_bit = 1;
	U8 const hole_bit = 2;
	U8 const bordering_hole_bit = 4;
	U8 const bordering_edge_bit = 8;
	// Use the legacy waterheight for the Edge water in the case
	// that we don't find any Hole water at all.
	F32 water_height = DEFAULT_WATER_HEIGHT;
	int max_count = 0;
	LL_DEBUGS("WaterHeight") << "Current region: " << regionp->getName() << "; water height: " << regionp->getWaterHeight() << " m." << LL_ENDL;
	std::map<S32, int> water_height_counts;
	typedef std::queue<std::pair<S32, S32>, std::deque<std::pair<S32, S32> > > nxny_pairs_type;
	nxny_pairs_type nxny_pairs;
	nxny_pairs.push(nxny_pairs_type::value_type(0, 0));
	water_heights[origin] = regionp->getWaterHeight();
	checked[origin] = region_bit;
	// For debugging purposes.
	int number_of_connected_regions = 1;
	int uninitialized_regions = 0;
	int bordering_hole = 0;
	int bordering_edge = 0;
	while(!nxny_pairs.empty())
	{
		S32 const nx = nxny_pairs.front().first;
		S32 const ny = nxny_pairs.front().second;
		LL_DEBUGS("WaterHeight") << "nx,ny = " << nx << "," << ny << LL_ENDL;
		S32 const index = nx + ny * n + origin;
		nxny_pairs.pop();
		for (S32 dir = 0; dir < 4; ++dir)
		{
			S32 const cnx = nx + gDirAxes[dir][0];
			S32 const cny = ny + gDirAxes[dir][1];
			LL_DEBUGS("WaterHeight") << "dir = " << dir << "; cnx,cny = " << cnx << "," << cny << LL_ENDL;
			S32 const cindex = cnx + cny * n + origin;
			bool is_hole = false;
			bool is_edge = false;
			LLViewerRegion* new_region_found = NULL;
			if (cnx < -nsims || cnx > nsims ||
			    cny < -nsims || cny > nsims)
			{
				LL_DEBUGS("WaterHeight") << "  Edge Water!" << LL_ENDL;
				// Bumped into Edge water object.
				is_edge = true;
			}
			else if (checked[cindex])
			{
				LL_DEBUGS("WaterHeight") << "  Already checked before!" << LL_ENDL;
				// Already checked.
				is_hole = (checked[cindex] & hole_bit);
			}
			else
			{
				S32 x = (S32)region_x + cnx * rwidth;
				S32 y = (S32)region_y + cny * rwidth;
				U64 region_handle = to_region_handle(x, y);
				new_region_found = getRegionFromHandle(region_handle);
				is_hole = !new_region_found;
				checked[cindex] = is_hole ? hole_bit : region_bit;
			}
			if (is_hole)
			{
				// This was a region that borders at least one 'hole'.
				// Count the found coastline.
				F32 new_water_height = water_heights[index];
				LL_DEBUGS("WaterHeight") << "  This is void; counting coastline with water height of " << new_water_height << LL_ENDL;
				S32 new_water_height_cm = llround(new_water_height * 100);
				int count = (water_height_counts[new_water_height_cm] += 1);
				// Just use the lowest water height: this is mainly about the horizon water,
				// and whatever we do, we don't want it to be possible to look under the water
				// when looking in the distance: it is better to make a step downwards in water
				// height when going away from the avie than a step upwards. However, since
				// everyone is used to DEFAULT_WATER_HEIGHT, don't allow a single region
				// to drag the water level below DEFAULT_WATER_HEIGHT on it's own.
				if (bordering_hole == 0 ||			// First time we get here.
				    (new_water_height >= DEFAULT_WATER_HEIGHT &&
					 new_water_height < water_height) ||
				    (new_water_height < DEFAULT_WATER_HEIGHT &&
					 count > max_count)
				   )
				{
					water_height = new_water_height;
				}
				if (count > max_count)
				{
					max_count = count;
				}
				if (!(checked[index] & bordering_hole_bit))
				{
					checked[index] |= bordering_hole_bit;
					++bordering_hole;
				}
			}
			else if (is_edge && !(checked[index] & bordering_edge_bit))
			{
				checked[index] |= bordering_edge_bit;
				++bordering_edge;
			}
			if (!new_region_found)
			{
				// Dead end, there is no region here.
				continue;
			}
			// Found a new connected region.
			++number_of_connected_regions;
			if (new_region_found->getName().empty())
			{
				// Uninitialized LLViewerRegion, don't use it's water height.
				LL_DEBUGS("WaterHeight") << "  Uninitialized region." << LL_ENDL;
				++uninitialized_regions;
				continue;
			}
			nxny_pairs.push(nxny_pairs_type::value_type(cnx, cny));
			water_heights[cindex] = new_region_found->getWaterHeight();
			LL_DEBUGS("WaterHeight") << "  Found a new region (name: " << new_region_found->getName() << "; water height: " << water_heights[cindex] << " m)!" << LL_ENDL;
		}
	}
	llinfos << "Number of connected regions: " << number_of_connected_regions << " (" << uninitialized_regions <<
		" uninitialized); number of regions bordering Hole water: " << bordering_hole <<
		"; number of regions bordering Edge water: " << bordering_edge << llendl;
	llinfos << "Coastline count (height, count): ";
	bool first = true;
	for (std::map<S32, int>::iterator iter = water_height_counts.begin(); iter != water_height_counts.end(); ++iter)
	{
		if (!first) llcont << ", ";
		llcont << "(" << (iter->first / 100.f) << ", " << iter->second << ")";
		first = false;
	}
	llcont << llendl;
	llinfos << "Water height used for Hole and Edge water objects: " << water_height << llendl;

	// Update all Region water objects.
	for (region_list_t::iterator iter = mRegionList.begin(); iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		LLVOWater* waterp = regionp->getLand().getWaterObj();
		if (waterp)
		{
			gObjectList.updateActive(waterp);
		}
	}

	// Clean up all existing Hole water objects.
	for (std::list<LLVOWater*>::iterator iter = mHoleWaterObjects.begin();
		 iter != mHoleWaterObjects.end(); ++iter)
	{
		LLVOWater* waterp = *iter;
		gObjectList.killObject(waterp);
	}
	mHoleWaterObjects.clear();

	// Let the Edge and Hole water boxes be 1024 meter high so that they
	// are never too small to be drawn (A LL_VO_*_WATER box has water
	// rendered on it's bottom surface only), and put their bottom at
	// the current regions water height.
	F32 const box_height = 1024;
	F32 const water_center_z = water_height + box_height / 2;

	// Create new Hole water objects within 'range' where there is no region.
	for (S32 x = min_x; x <= max_x; x += rwidth)
	{
		for (S32 y = min_y; y <= max_y; y += rwidth)
		{
			U64 region_handle = to_region_handle(x, y);
			if (!getRegionFromHandle(region_handle))
			{
				LLVOWater* waterp = (LLVOWater*)gObjectList.createObjectViewer(LLViewerObject::LL_VO_VOID_WATER, gAgent.getRegion());
				waterp->setUseTexture(FALSE);
				waterp->setPositionGlobal(LLVector3d(x + rwidth / 2, y + rwidth / 2, water_center_z));
				waterp->setScale(LLVector3((F32)rwidth, (F32)rwidth, box_height));
				gPipeline.createObject(waterp);
				mHoleWaterObjects.push_back(waterp);
			}
		}
	}

	// Center of the region.
	S32 const center_x = region_x + rwidth / 2;
	S32 const center_y = region_y + rwidth / 2;
	// Width of the area with Hole water objects.
	S32 const width = rwidth + 2 * range;
	S32 const horizon_extend = 2048 + 512 - range;	// Legacy value.
	// The overlap is needed to get rid of sky pixels being visible between the
	// Edge and Hole water object at greater distances (due to floating point
	// round off errors).
	S32 const edge_hole_overlap = 1;		// Twice the actual overlap.
		
	for (S32 dir = 0; dir < 8; ++dir)
	{
		// Size of the Edge water objects.
		S32 const dim_x = (gDirAxes[dir][0] == 0) ? width : (horizon_extend + edge_hole_overlap);
		S32 const dim_y = (gDirAxes[dir][1] == 0) ? width : (horizon_extend + edge_hole_overlap);
		// And their position.
		S32 const water_center_x = center_x + (width + horizon_extend) / 2 * gDirAxes[dir][0];
		S32 const water_center_y = center_y + (width + horizon_extend) / 2 * gDirAxes[dir][1];

		LLVOWater* waterp = mEdgeWaterObjects[dir];
		if (!waterp || waterp->isDead())
		{
			// The edge water objects can be dead because they're attached to the region that the
			// agent was in when they were originally created.
			mEdgeWaterObjects[dir] = (LLVOWater *)gObjectList.createObjectViewer(LLViewerObject::LL_VO_VOID_WATER, gAgent.getRegion());
			waterp = mEdgeWaterObjects[dir];
			waterp->setUseTexture(FALSE);
			waterp->setIsEdgePatch(TRUE);		// Mark that this is edge water and not hole water.
			gPipeline.createObject(waterp);
		}

		waterp->setRegion(gAgent.getRegion());
		LLVector3d water_pos(water_center_x, water_center_y, water_center_z);
		LLVector3 water_scale((F32) dim_x, (F32) dim_y, box_height);

		waterp->setPositionGlobal(water_pos);
		waterp->setScale(water_scale);

		gObjectList.updateActive(waterp);
	}
}

void LLWorld::shiftRegions(const LLVector3& offset)
{
	for (region_list_t::const_iterator i = getRegionList().begin(); i != getRegionList().end(); ++i)
	{
		LLViewerRegion* region = *i;
		region->updateRenderMatrix();
	}

	LLViewerPartSim::getInstance()->shift(offset);
}

LLViewerImage* LLWorld::getDefaultWaterTexture()
{
	return mDefaultWaterTexturep;
}

void LLWorld::setSpaceTimeUSec(const U64 space_time_usec)
{
	mSpaceTimeUSec = space_time_usec;
}

U64 LLWorld::getSpaceTimeUSec() const
{
	return mSpaceTimeUSec;
}

void LLWorld::requestCacheMisses()
{
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		regionp->requestCacheMisses();
	}
}

void LLWorld::getInfo(LLSD& info)
{
	LLSD region_info;
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{	
		LLViewerRegion* regionp = *iter;
		regionp->getInfo(region_info);
		info["World"].append(region_info);
	}
}

void LLWorld::disconnectRegions()
{
	LLMessageSystem* msg = gMessageSystem;
	for (region_list_t::iterator iter = mRegionList.begin();
		 iter != mRegionList.end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		if (regionp == gAgent.getRegion())
		{
			// Skip the main agent
			continue;
		}

		llinfos << "Sending AgentQuitCopy to: " << regionp->getHost() << llendl;
		msg->newMessageFast(_PREHASH_AgentQuitCopy);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->nextBlockFast(_PREHASH_FuseBlock);
		msg->addU32Fast(_PREHASH_ViewerCircuitCode, gMessageSystem->mOurCircuitCode);
		msg->sendMessage(regionp->getHost());
	}
}


void process_enable_simulator(LLMessageSystem *msg, void **user_data)
{
	// enable the appropriate circuit for this simulator and 
	// add its values into the gSimulator structure
	U64		handle;
	U32		ip_u32;
	U16		port;

	msg->getU64Fast(_PREHASH_SimulatorInfo, _PREHASH_Handle, handle);
	msg->getIPAddrFast(_PREHASH_SimulatorInfo, _PREHASH_IP, ip_u32);
	msg->getIPPortFast(_PREHASH_SimulatorInfo, _PREHASH_Port, port);

	// which simulator should we modify?
	LLHost sim(ip_u32, port);

	// Viewer trusts the simulator.
	msg->enableCircuit(sim, TRUE);
	LLWorld::getInstance()->addRegion(handle, sim);

	// give the simulator a message it can use to get ip and port
	llinfos << "simulator_enable() Enabling " << sim << " with code " << msg->getOurCircuitCode() << llendl;
	msg->newMessageFast(_PREHASH_UseCircuitCode);
	msg->nextBlockFast(_PREHASH_CircuitCode);
	msg->addU32Fast(_PREHASH_Code, msg->getOurCircuitCode());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addUUIDFast(_PREHASH_ID, gAgent.getID());
	msg->sendReliable(sim);
}

class LLEstablishAgentCommunication : public LLHTTPNode
{
	LOG_CLASS(LLEstablishAgentCommunication);
public:
 	virtual void describe(Description& desc) const
	{
		desc.shortInfo("seed capability info for a region");
		desc.postAPI();
		desc.input(
			"{ seed-capability: ..., sim-ip: ..., sim-port }");
		desc.source(__FILE__, __LINE__);
	}

	virtual void post(ResponsePtr response, const LLSD& context, const LLSD& input) const
	{
		if (!input["body"].has("agent-id") ||
			!input["body"].has("sim-ip-and-port") ||
			!input["body"].has("seed-capability"))
		{
			llwarns << "invalid parameters" << llendl;
            return;
		}

		LLHost sim(input["body"]["sim-ip-and-port"].asString());
	
		LLViewerRegion* regionp = LLWorld::getInstance()->getRegion(sim);
		if (!regionp)
		{
			llwarns << "Got EstablishAgentCommunication for unknown region "
					<< sim << llendl;
			return;
		}
		regionp->setSeedCapability(input["body"]["seed-capability"]);
	}
};

// disable the circuit to this simulator
// Called in response to "DisableSimulator" message.
void process_disable_simulator(LLMessageSystem *mesgsys, void **user_data)
{	
	LLHost host = mesgsys->getSender();

	//llinfos << "Disabling simulator with message from " << host << llendl;
	LLWorld::getInstance()->removeRegion(host);

	mesgsys->disableCircuit(host);
}


void process_region_handshake(LLMessageSystem* msg, void** user_data)
{
	LLHost host = msg->getSender();
	LLViewerRegion* regionp = LLWorld::getInstance()->getRegion(host);
	if (!regionp)
	{
		llwarns << "Got region handshake for unknown region "
			<< host << llendl;
		return;
	}

	regionp->unpackRegionHandshake();
}


void send_agent_pause()
{
	// *NOTE:Mani Pausing the mainloop timeout. Otherwise a long modal event may cause
	// the thread monitor to timeout.
	LLAppViewer::instance()->pauseMainloopTimeout();
	
	// Note: used to check for LLWorld initialization before it became a singleton.
	// Rather than just remove this check I'm changing it to assure that the message 
	// system has been initialized. -MG
	if (!gMessageSystem)
	{
		return;
	}
	
	gMessageSystem->newMessageFast(_PREHASH_AgentPause);
	gMessageSystem->nextBlockFast(_PREHASH_AgentData);
	gMessageSystem->addUUIDFast(_PREHASH_AgentID, gAgentID);
	gMessageSystem->addUUIDFast(_PREHASH_SessionID, gAgentSessionID);

	gAgentPauseSerialNum++;
	gMessageSystem->addU32Fast(_PREHASH_SerialNum, gAgentPauseSerialNum);

	for (LLWorld::region_list_t::const_iterator iter = LLWorld::getInstance()->getRegionList().begin();
		 iter != LLWorld::getInstance()->getRegionList().end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		gMessageSystem->sendReliable(regionp->getHost());
	}

	gObjectList.mWasPaused = TRUE;
}


void send_agent_resume()
{
	// Note: used to check for LLWorld initialization before it became a singleton.
	// Rather than just remove this check I'm changing it to assure that the message 
	// system has been initialized. -MG
	if (!gMessageSystem)
	{
		return;
	}

	gMessageSystem->newMessageFast(_PREHASH_AgentResume);
	gMessageSystem->nextBlockFast(_PREHASH_AgentData);
	gMessageSystem->addUUIDFast(_PREHASH_AgentID, gAgentID);
	gMessageSystem->addUUIDFast(_PREHASH_SessionID, gAgentSessionID);

	gAgentPauseSerialNum++;
	gMessageSystem->addU32Fast(_PREHASH_SerialNum, gAgentPauseSerialNum);
	

	for (LLWorld::region_list_t::const_iterator iter = LLWorld::getInstance()->getRegionList().begin();
		 iter != LLWorld::getInstance()->getRegionList().end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		gMessageSystem->sendReliable(regionp->getHost());
	}

	// Reset the FPS counter to avoid an invalid fps
	LLViewerStats::getInstance()->mFPSStat.start();

	LLAppViewer::instance()->resumeMainloopTimeout();
}

static LLVector3d unpackLocalToGlobalPosition(U32 compact_local, const LLVector3d& region_origin)
{
	LLVector3d pos_global;
	LLVector3 pos_local;
	U8 bits;

	bits = compact_local & 0xFF;
	pos_local.mV[VZ] = F32(bits) * 4.f;
	compact_local >>= 8;

	bits = compact_local & 0xFF;
	pos_local.mV[VY] = (F32)bits;
	compact_local >>= 8;

	bits = compact_local & 0xFF;
	pos_local.mV[VX] = (F32)bits;

	pos_global.setVec( pos_local );
	pos_global += region_origin;
	return pos_global;
}

void LLWorld::getAvatars(std::vector<LLUUID>* avatar_ids, std::vector<LLVector3d>* positions, const LLVector3d& relative_to, F32 radius) const
{
	if(avatar_ids != NULL)
	{
		avatar_ids->clear();
	}
	if(positions != NULL)
	{
		positions->clear();
	}
	for (LLWorld::region_list_t::const_iterator iter = LLWorld::getInstance()->getRegionList().begin();
		iter != LLWorld::getInstance()->getRegionList().end(); ++iter)
	{
		LLViewerRegion* regionp = *iter;
		const LLVector3d& origin_global = regionp->getOriginGlobal();
		S32 count = regionp->mMapAvatars.count();
		for (S32 i = 0; i < count; i++)
		{
			LLVector3d pos_global = unpackLocalToGlobalPosition(regionp->mMapAvatars.get(i), origin_global);
			if(dist_vec(pos_global, relative_to) <= radius)
			{
				if(positions != NULL)
				{
					positions->push_back(pos_global);
				}
				if(avatar_ids != NULL)
				{
					avatar_ids->push_back(regionp->mMapAvatarIDs.get(i));
				}
			}
		}
	}
}


LLHTTPRegistration<LLEstablishAgentCommunication>
	gHTTPRegistrationEstablishAgentCommunication(
							"/message/EstablishAgentCommunication");
