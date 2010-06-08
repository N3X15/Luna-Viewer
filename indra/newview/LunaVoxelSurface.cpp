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

#include "LunaVoxelSurface.h"

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


class LunaMaterialsFetched : public LLHTTPClient::Responder
{
	LOG_CLASS(LunaMaterialsFetched);
public:
    LunaMaterialsFetched(LLViewerRegion* region)
		: mRegion(region)
    { }
	virtual ~LunaMaterialsFetched()
	{
	}

	void setRegion(LLViewerRegion* region)
	{
		mRegion = region ;
	}

	void error(U32 statusNum, const std::string& reason)
	{
		LL_WARNS2("Voxels", "Materials") << statusNum << ": " << reason << LL_ENDL;
	}

	void result(const LLSD& content)
	{
		if(!mRegion || LLHTTPClient::ResponderPtr(this) != mRegion->getHttpResponderPtr()) //region is removed or responder is not created.
		{
			return ;
		}

		LLSD::map_const_iterator iter;
		for(iter = content.beginMap(); iter != content.endMap(); ++iter)
		{
			LLSD matSD = iter->second;
			VoxMaterial mat;
			mat.ID		= (U8)matSD["ID"].asInteger();
			mat.Name	= matSD["Name"].asString();
			//mat.Type	= matSD["Type"].asString();
			mat.Density	= matSD["Density"].asSingle();
			mat.Texture	= matSD["Texture"].asUUID();
			mat.Deposit	= matSD["Deposit"].asInteger();
			mat.Flags	= (U8)matSD["Flags"].asInteger();
			mRegion->mVoxels->AddMaterial(mat);
			LL_DEBUGS2("Voxels", "Materials") << "Adding material: " << mat.Name << LL_ENDL;
		}
	}

	static boost::intrusive_ptr<LunaMaterialsFetched> build(LLViewerRegion* region)
	{
		return boost::intrusive_ptr<LunaMaterialsFetched>(new LunaMaterialsFetched(region));
	}

private:
	LLViewerRegion* mRegion;
};

LunaVoxelSurface::LunaVoxelSurface(LLViewerRegion *regionp = NULL):
	XScale(0),
	YScale(0),
	ZScale(0),
	mDetailTextureScale(0.f),
	mOriginGlobal(0.0, 0.0, 0.0),
	mSTexturep(NULL),
	mRegionp(regionp)
{
	mVoxels=NULL;
	
	// "uninitialized" min/max z
	mMinZ = 10000.f;
	mMaxZ = -10000.f;

	for (S32 i = 0; i < 8; i++)
	{
		mNeighbors[i] = NULL;
	}
}

LunaVoxelSurface::~LunaVoxelSurface()
{
	delete [] mVoxels;
	mVoxels=NULL;

	delete [] mNorm;
	mNorm=NULL;
}

void LunaVoxelSurface::initClasses()
{
}

// static
void LunaVoxelSurface::HandleLayerPacket(LLMessageSystem *mesgsys, void **user_data)
{
	LLViewerRegion *regionp = LLWorld::getInstance()->getRegion(mesgsys->getSender());

	if (!regionp || gNoRender)
	{
		return;
	}

	if(!regionp->mVoxels)
	{
		regionp->mVoxels=LunaVoxelSurface::create(256,256,100,regionp->getOriginGlobal());
	}

	U32 x,y,z,f;

	mesgsys->getU32("LayerData","Z", z);
	mesgsys->getU32("LayerData","Flags", f);
	if((f&1)==1) // Clear voxels
	{
		regionp->mVoxels=LunaVoxelSurface::create(256,256,100,regionp->getOriginGlobal());
	}
	S32 size = mesgsys->getNumberOfBlocks("VoxelData");
	if (0 == size)
	{
		LL_WARNS("Messaging") << "Layer data has zero size." << LL_ENDL;
		return;
	}
	if (size < 0)
	{
		// getSizeFast() is probably trying to tell us about an error
		LL_WARNS("Messaging") << "getSizeFast() returned negative result: "
			<< size
			<< LL_ENDL;
		return;
	}

	for(S32 i = 0; i < size; ++i)
	{
		unsigned char m = 0x00;
		mesgsys->getU32("VoxelData","X",x,i);
		mesgsys->getU32("VoxelData","Y",y,i);
		mesgsys->getU32("VoxelData","Material",m,i);
		!regionp->mVoxels->SetVoxel(x,y,z,m);
	}
}

void LunaVoxelSurface::create(const U32 width
		const U32 length,
		const U32 height,
		const LLVector3d &origin_global)
{
	XScale=width;
	YScale=length;
	ZScale=height;

	mVoxels=new unsigned char[XScale*YScale*ZScale];
	for(int x=0;x<XScale;x++)
	{
		for(int y=0;x<XScale;x++)
		{
			SetVoxel(x,y,0,0x01);
		}
	}

	mOriginGlobal.setVec(origin_global);
}

void LunaVoxelSurface::SetVoxel(int x,int y, int z, unsigned char material)
{
	if(x<0) x=0;
	if(x>XScale-1) x = XScale-1;
	if(y<0) y=0;
	if(y>YScale-1) y = YScale-1;
	if(z<0) z=0;
	if(z>ZScale-1) z = ZScale-1;

	mVoxels[x+(y*YScale)+(z*YScale*ZScale)]=material;
}

void LunaVoxelSurface::SetVoxelsFromLayer(int z,unsigned char *material)
{
	if(z<0 || z>ZScale-1) return;
	for(int x=0;x<XScale;x++)
	{
		for(int y=0;y<YScale;y++)
		{
			SetVoxel(x,y,z,material[x+(YScale*y)]);
		}
	}
}

void LunaVoxelSurface::setRegion(LLViewerRegion *regionp)
{
	mRegionp = regionp;
}

void LunaVoxelSurface::setOriginGlobal(const LLVector3d &origin_global)
{
	LLVector3d new_origin_global;
	mOriginGlobal = origin_global;
}

void LunaVoxelSurface::connectNeighbor(LunaVoxelSurface *neighborp, U32 direction)
{
	S32 i;

	if (gNoRender)
	{
		return;
	}

	mNeighbors[direction] = neighborp;
	neighborp->mNeighbors[gDirOpposite[direction]] = this;
}

void LunaVoxelSurface::disconnectNeighbor(LunaVoxelSurface *neighborp)
{
	S32 i;
	for (i = 0; i < 8; i++)
	{
		if (neighborp == mNeighbors[i])
		{
			mNeighbors[i] = NULL;
		}
	}
}

void LunaVoxelSurface::disconnectAllNeighbors()
{
	S32 i;
	for (i = 0; i < 8; i++)
	{
		if (mNeighbors[i])
		{
			mNeighbors[i]->disconnectNeighbor(this);
			mNeighbors[i] = NULL;
		}
	}
}

void LunaVoxelSurface::AddMaterial(VoxMaterial mat)
{
	mMaterials.insert(std::pair<byte,VoxMaterial>(mat.ID,mat));
}

VoxMaterial LunaVoxelSurface::GetMaterial(byte ID)
{
	return mMaterials[ID];
}
void LunaVoxelSurface::NukeMaterial(byte ID)
{
	mMaterials.erase(ID);
}

// what the fuck
LLVector3 LunaVoxelSurface::getOriginAgent() const
{
	return gAgent.getPosAgentFromGlobal(mOriginGlobal);
}

const LLVector3d &LunaVoxelSurface::getOriginGlobal() const
{
	return mOriginGlobal;
}

// Returns the height of the surface immediately above (or below) location,
// or if location is not above surface returns zero.
F32 LunaVoxelSurface::resolveHeightRegion(const int x, const int y) const
{
	F32 h = 0.0f;
	for(int z = 0;z<ZScale;z++)
	{
		if(IsSolid(x,y,z))
			h=(F32)z;
	}
	return h;
}
F32 LunaVoxelSurface::resolveHeightGlobal(const LLVector3d &position_global) const
{
	if (!mRegionp)
	{
		return 0.0f;
	}
	
	LLVector3 pos_region = mRegionp->getPosRegionFromGlobal(v);

	return resolveHeightRegion((int)pos_region.mdV[VX],(int)pos_region.mdV[VY]);
}

// Returns normal to surface.
LLVector3 LunaVoxelSurface::resolveNormalGlobal(const LLVector3d& v) const
{
	// No fucking idea.
	return new LLVector3(0.0f,0.0f,0.0f);
}

