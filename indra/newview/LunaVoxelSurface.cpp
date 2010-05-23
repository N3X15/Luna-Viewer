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

void LunaVoxelSurface::create(const U32 width
		const U32 length,
		const U32 height,
		const LLVector3d &origin_global)
{
	XScale=width;
	YScale=length;
	ZScale=height;

	mVoxels=new byte[XScale*YScale*ZScale];
	for(int x=0;x<XScale;x++)
	{
		for(int y=0;x<XScale;x++)
		{
			SetVoxel(x,y,0,0x01);
		}
	}

	mOriginGlobal.setVec(origin_global);
}

void LunaVoxelSurface::SetVoxel(int x,int y, int z, byte material)
{
	if(x<0) x=0;
	if(x>XScale-1) x = XScale-1;
	if(y<0) y=0;
	if(y>YScale-1) y = YScale-1;
	if(z<0) z=0;
	if(z>ZScale-1) z = ZScale-1;

	mVoxels[x+(y*YScale)+(z*YScale*ZScale)]=material;
}

void LunaVoxelSurface::SetVoxelsFromLayer(int z,byte *material)
{
	if(z<0 || z>ZScale-1) return;
	for(int x=0;x<XScale;x++)
	{
		for(int y=0;y<YScale;y++)
		{
			SetVoxel(x,y,z,byte[x+(YScale*y)]);
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
	F32 h = 0f;
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
		return 0.f;
	}
	
	LLVector3 pos_region = mRegionp->getPosRegionFromGlobal(v);

	return resolveHeightRegion((int)pos_region.mdV[VX],(int)pos_region.mdV[VY]);
}

// Returns normal to surface.
LLVector3 LunaVoxelSurface::resolveNormalGlobal(const LLVector3d& v) const
{
	// No fucking idea.
	return new LLVector3(0f,0f,0f);
}
//U32 LunaVoxelSurface::getRenderStride(const U32 render_level) const;
//U32 LunaVoxelSurface::getRenderLevel(const U32 render_stride) const;

	// Update methods (called during idle, normally)
	BOOL idleUpdate(F32 max_update_time);

	BOOL containsPosition(const LLVector3 &position);

	void moveZ(const S32 x, const S32 y, const F32 delta);	

	LLViewerRegion *getRegion() const				{ return mRegionp; }

	F32 getMinZ() const								{ return mMinZ; }
	F32 getMaxZ() const								{ return mMaxZ; }

	LLViewerImage *getSTexture();
	LLViewerImage *getWaterTexture();
	BOOL hasZData() const							{ return mHasZData; }

	void dirtyAllLayers();	// Use this to dirty all patches when changing terrain parameters

	void dirtyLayer(int LID);

	static void setTextureSize(const S32 texture_size);

	friend class LLSurfacePatch;
	friend std::ostream& operator<<(std::ostream &s, const LLSurface &S);
public:
	S32 XScale;
	S32 YScale;
	S32 ZScale;

	
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
	LLSurfacePatch *mPatchList;		// Array of all patches

	// Array of grid data, XScale*YScale*ZScale
	byte *mVoxels

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
GLvoid LunaVoxelSurface::vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
        rfNormal.fX = (this->*fSample)(fX-0.01, fY, fZ) - (this->*fSample)(fX+0.01, fY, fZ);
        rfNormal.fY = (this->*fSample)(fX, fY-0.01, fZ) - (this->*fSample)(fX, fY+0.01, fZ);
        rfNormal.fZ = (this->*fSample)(fX, fY, fZ-0.01) - (this->*fSample)(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal, rfNormal);
}

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid LunaVoxelSurface::vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
	GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
	GLfloat fOffset;
	GLvector sColor;
	GLfloat afCubeValue[8];
	GLvector asEdgeVertex[12];
	GLvector asEdgeNorm[12];

	//Make a local copy of the values at the cube's corners
	for(iVertex = 0; iVertex < 8; iVertex++)
	{
		afCubeValue[iVertex] = (this->*fSample)(fX + a2fVertexOffset[iVertex][0]*fScale,fY + a2fVertexOffset[iVertex][1]*fScale,fZ + a2fVertexOffset[iVertex][2]*fScale);
	}

	//Find which vertices are inside of the surface and which are outside
	iFlagIndex = 0;
	for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
	{
		if(afCubeValue[iVertexTest] <= fTv)     iFlagIndex |= 1<<iVertexTest;
	}

	//Find which edges are intersected by the surface
	iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

	//If the cube is entirely inside or outside of the surface, then there will be no intersections
	if(iEdgeFlags == 0)
	{
		return;
	}

	//Find the point of intersection of the surface with each edge
	//Then find the normal to the surface at those points
	for(iEdge = 0; iEdge < 12; iEdge++)
	{
		//if there is an intersection on this edge
		if(iEdgeFlags & (1<<iEdge))
		{
			fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTv);

			asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
			asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
			asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

			vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
		}
	}


	//Draw the triangles that were found.  There can be up to five per cube
	for(iTriangle = 0; iTriangle < 5; iTriangle++)
	{
		if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

		for(iCorner = 0; iCorner < 3; iCorner++)
		{
			iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

			vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
			glColor4f(sColor.fX, sColor.fY, sColor.fZ, 0.6);
			glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
			glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
		}
	}
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
GLvoid LunaVoxelSurface::vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iVertex, iTetrahedron, iVertexInACube;
        GLvector asCubePosition[8];
        GLfloat  afCubeValue[8];
        GLvector asTetrahedronPosition[4];
        GLfloat  afTetrahedronValue[4];

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].fX = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].fY = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].fZ = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(asCubePosition[iVertex].fX,asCubePosition[iVertex].fY,asCubePosition[iVertex].fZ);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
            for(iVertex = 0; iVertex < 4; iVertex++)
            {
                iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                asTetrahedronPosition[iVertex].fX = asCubePosition[iVertexInACube].fX;
                asTetrahedronPosition[iVertex].fY = asCubePosition[iVertexInACube].fY;
                asTetrahedronPosition[iVertex].fZ = asCubePosition[iVertexInACube].fZ;
                afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
            }
            vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue, fTv);
        }
}


//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
GLvoid LunaVoxelSurface::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv)
{
        GLint iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        GLfloat fOffset, fInvOffset;
        GLvector asEdgeVertex[6];
        GLvector asEdgeNorm[6];
        GLvector sColor;

        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTv)     iFlagIndex |= 1<<iVertex;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

        //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }
        //Find the point of intersection of the surface with each edge
        // Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTv);
                fInvOffset = 1.0 - fOffset;

                asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX  +  fOffset*pasTetrahedronPosition[iVert1].fX;
                asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY  +  fOffset*pasTetrahedronPosition[iVert1].fY;
                asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ  +  fOffset*pasTetrahedronPosition[iVert1].fZ;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
        }
        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
            if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

                vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                glColor4f(sColor.fX, sColor.fY, sColor.fZ, 0.6);
                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}

