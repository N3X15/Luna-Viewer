/** 
 * @file llsurface.cpp
 * @brief Implementation of LLSurface class
 *
 * $LicenseInfo:firstyear=2000&license=viewergpl$
 * 
 * Copyright (c) 2000-2009, Linden Research, Inc.
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

#include "llrender.h"

#include "llviewerimagelist.h"
//#include "llpatchvertexarray.h"
//#include "patch_dct.h"
//#include "patch_code.h"
#include "bitpack.h"
#include "llviewerobjectlist.h"
#include "llregionhandle.h"
#include "llagent.h"
#include "llappviewer.h"
#include "llworld.h"
#include "llviewercontrol.h"
#include "llviewerimage.h"
//#include "llsurfacepatch.h"
//#include "llvosurfacepatch.h"
#include "llvowater.h"
#include "pipeline.h"
#include "llviewerregion.h"
#include "llvlcomposition.h"
#include "noise.h"
#include "llviewercamera.h"
#include "llglheaders.h"
#include "lldrawpoolterrain.h"
#include "lldrawable.h"

enum MaterialType
{
	Soil,
	Sand,
	Igneous,
	Metamorphic,
	Sedimentary
};
	
const int DEPOSIT_LAYER		= 0; // Like a "base coat".  
const int DEPOSIT_SMALL_CLUSTER	= 1; // A "ball" of material (rare gems?)
const int DEPOSIT_LARGE_CLUSTER	= 2; // Larger ball
const int DEPOSIT_VEIN		= 3; // Ore vein

const U8 MATFLAG_SOLID	=0x01;
const U8 MATFLAG_FLUID	=0x02;
const U8 MATFLAG_DAMP	=0x04;
const U8 MATFLAG_TOXIC	=0x08;

struct VoxMaterial
{
public: 
	U8		ID;//		= 0x00;
	std::string	Name;//		= "Granite";
	MaterialType	Type;//		= MaterialType.Igneous;
	F32		Density;//		= 2.75f;
	LLUUID		Texture;//		= UUID.Zero;
	int		Deposit;//		= DepositType.Layer;
	U8		Flags;//		= (MatFlags)0x00;
};

class LunaVoxelSurface 
{
public:
	LunaVoxelSurface(LLViewerRegion *regionp = NULL);
	virtual ~LunaVoxelSurface();

	static void initClasses(); // Do class initialization for LLSurface and its child classes.

	void create(const U32 width,
		const U32 length,
		const U32 height,
		const LLVector3d &origin_global);	// Allocates and initializes surface

	void setRegion(LLViewerRegion *regionp);

	void setOriginGlobal(const LLVector3d &origin_global);

	void SetVoxel(int x,int y,int z,unsigned char matID);
	void SetVoxelsFromLayer(int z,unsigned char *matID);

	void connectNeighbor(LunaVoxelSurface *neighborp, U32 direction);
	void disconnectNeighbor(LunaVoxelSurface *neighborp);
	void disconnectAllNeighbors();

	void AddMaterial(VoxMaterial Material);
	VoxMaterial GetMaterial(unsigned char ID);
	void NukeMaterial(unsigned char ID);

	LLVector3 getOriginAgent() const;
	const LLVector3d &getOriginGlobal() const;

	F32 getHeight() const {return ZScale;}
	S32 getWidth() const  {return XScale;} 
	S32 getLength() const {return YScale;}

	// Returns the height of the surface immediately above (or below) location,
	// or if location is not above surface returns zero.
	F32 resolveHeightRegion(const F32 x, const F32 y) const;
	F32 resolveHeightRegion(const LLVector3 &location) const
			{ return resolveHeightRegion( location.mV[VX], location.mV[VY] ); }
	F32 resolveHeightGlobal(const LLVector3d &position_global) const;
	LLVector3 resolveNormalGlobal(const LLVector3d& v) const;				//  Returns normal to surface

	LLSurfacePatch *resolvePatchRegion(const F32 x, const F32 y) const;
	LLSurfacePatch *resolvePatchRegion(const LLVector3 &position_region) const;
	LLSurfacePatch *resolvePatchGlobal(const LLVector3d &position_global) const;

	// Update methods (called during idle, normally)
	BOOL idleUpdate(F32 max_update_time);

	BOOL containsPosition(const LLVector3 &position);

	void moveZ(const S32 x, const S32 y, const F32 delta);	

	LLViewerRegion *getRegion() const				{ return mRegionp; }

	F32 getMinZ() const								{ return mMinZ; }
	F32 getMaxZ() const								{ return mMaxZ; }

	LLViewerImage *getSTexture();
	LLViewerImage *getWaterTexture();
	BOOL hasData() const							{ return mHasData; }

	void dirtyAllLayers();	// Use this to dirty all patches when changing terrain parameters

	void dirtyLayer(int LID);

	static void setTextureSize(const S32 texture_size);
	static void HandleLayerPacket(LLMessageSystem *mesgsys, void **user_data);
	friend class LLSurfacePatch;
	friend std::ostream& operator<<(std::ostream &s, const LLSurface &S);
public:
	S32 XScale;
	S32 YScale;
	S32 ZScale;

	std::map<unsigned char,VoxMaterial> mMaterials;
	// Each surface points at 8 neighbors (or NULL)
	// +---+---+---+
	// |NW | N | NE|
	// +---+---+---+
	// | W | 0 | E |
	// +---+---+---+
	// |SW | S | SE|
	// +---+---+---+
	LunaVoxelSurface *mNeighbors[8]; // Adjacent patches
	
	F32 mDetailTextureScale;	//  Number of times to repeat detail texture across this surface 

	static F32 sTextureUpdateTime;
	static S32 sTexelsUpdated;
	static LLStat sTexelsUpdatedPerSecStat;

protected:
	void createSTexture();
	void createWaterTexture();
	void initTextures();
	void initWater();


	void createPatchData();		// Allocates memory for patches.
	void destroyPatchData();    // Deallocates memory for patches.

	BOOL generateWaterTexture(const F32 x, const F32 y,
						const F32 width, const F32 height);		// Generate texture from composition values.

	//F32 updateTexture(LLSurfacePatch *ppatch);
	
	LLSurfacePatch *getPatch(const S32 x, const S32 y) const;

protected:
	LLVector3d	mOriginGlobal;		// In absolute frame

	// Array of grid data, XScale*YScale*ZScale
	U8 *mVoxels;

	// Array of grid normals, XScale*YScale*ZScale
	LLVector3 *mNorm;

	std::set<int> mDirtyLayers;


	// The textures should never be directly initialized - use the setter methods!
	LLPointer<LLViewerImage> mSTexturep;		// Texture for surface

	LLPatchVertexArray mPVArray;

	BOOL			mHasData;				// We've received any patch data for this surface.
	F32			mMinZ;					// min z for this region (during the session)
	F32			mMaxZ;					// max z for this region (during the session)

	S32			mSurfacePatchUpdateCount;					// Number of frames since last update.

private:
	LLViewerRegion *mRegionp; // Patch whose coordinate system this surface is using.
	static S32	sTextureSize;				// Size of the surface texture
};

