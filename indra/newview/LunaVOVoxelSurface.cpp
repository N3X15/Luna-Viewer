/** 
 * @file LunaVOVoxelSurface.cpp
 * @brief Viewer-object derived "surface patch", which is a piece of terrain
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

#include "LunaVOVoxelSurface.h"

#include "lldrawpoolterrain.h"

#include "lldrawable.h"
#include "llface.h"
#include "llprimitive.h"
#include "llsky.h"
#include "llsurfacepatch.h"
#include "llsurface.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llvlcomposition.h"
#include "llvovolume.h"
#include "pipeline.h"
#include "llspatialpartition.h"

F32 LunaVOVoxelSurface::sLODFactor = 1.f;

//============================================================================

class LunaVertexBufferTerrain : public LLVertexBuffer
{
public:
	LunaVertexBufferTerrain() :
		LLVertexBuffer(MAP_VERTEX | MAP_NORMAL | MAP_TEXCOORD0 | MAP_TEXCOORD1 | MAP_COLOR, GL_DYNAMIC_DRAW_ARB)
	{
		//texture coordinates 2 and 3 exist, but use the same data as texture coordinate 1
		mOffsets[TYPE_TEXCOORD3] = mOffsets[TYPE_TEXCOORD2] = mOffsets[TYPE_TEXCOORD1];
		mTypeMask |= MAP_TEXCOORD2 | MAP_TEXCOORD3;
	};

	/*// virtual
	void setupVertexBuffer(U32 data_mask) const
	{		
		if (LLDrawPoolTerrain::getDetailMode() == 0 || LLPipeline::sShadowRender)
		{
			LLVertexBuffer::setupVertexBuffer(data_mask);
		}
		else if (data_mask & LLVertexBuffer::MAP_TEXCOORD1)
		{
			LLVertexBuffer::setupVertexBuffer(data_mask);
		}
		else
		{
			LLVertexBuffer::setupVertexBuffer(data_mask);
		}
		llglassertok();		
	}*/
};

//============================================================================

LunaVOVoxelSurface::LunaVOVoxelSurface(const LLUUID &id, const LLPCode pcode, LLViewerRegion *regionp)
	:	LLStaticViewerObject(id, LL_VO_SURFACE_PATCH, regionp),
		mDirtiedPatch(FALSE),
		mPool(NULL),
		mBaseComp(0),
		mPatchp(NULL),
		mDirtyTexture(FALSE),
		mDirtyTerrain(FALSE),
		mLastStride(0),
		mLastLength(0)
{
	// Terrain must draw during selection passes so it can block objects behind it.
	mbCanSelect = TRUE;
	setScale(LLVector3(256.f, 256.f, 256.f)); // Hack for setting scale for bounding boxes/visibility.
}


LunaVOVoxelSurface::~LunaVOVoxelSurface()
{
	mPatchp = NULL;
}


void LunaVOVoxelSurface::markDead()
{
	LLViewerObject::markDead();
}


BOOL LunaVOVoxelSurface::isActive() const
{
	return FALSE;
}


void LunaVOVoxelSurface::setPixelAreaAndAngle(LLAgent &agent)
{
	mAppAngle = 50;
	mPixelArea = 500*500;
}


void LunaVOVoxelSurface::updateTextures(LLAgent &agent)
{
}


LLFacePool *LunaVOVoxelSurface::getPool()
{
	mPool = (LLDrawPoolTerrain*) gPipeline.getPool(LLDrawPool::POOL_TERRAIN, mPatchp->getSurface()->getSTexture());

	return mPool;
}


LLDrawable *LunaVOVoxelSurface::createDrawable(LLPipeline *pipeline)
{
	pipeline->allocDrawable(this);

	mDrawable->setRenderType(LLPipeline::RENDER_TYPE_TERRAIN);
	
	mBaseComp = llfloor(mPatchp->getMinComposition());
	S32 min_comp, max_comp, range;
	min_comp = llfloor(mPatchp->getMinComposition());
	max_comp = llceil(mPatchp->getMaxComposition());
	range = (max_comp - min_comp);
	range++;
	if (range > 3)
	{
		if ((mPatchp->getMinComposition() - min_comp) > (max_comp - mPatchp->getMaxComposition()))
		{
			// The top side runs over more
			mBaseComp++;
		}
		range = 3;
	}

	LLFacePool *poolp = getPool();

	mDrawable->addFace(poolp, NULL);

	return mDrawable;
}


BOOL LunaVOVoxelSurface::updateGeometry(LLDrawable *drawable)
{
	LLFastTimer ftm(LLFastTimer::FTM_UPDATE_TERRAIN);

	dirtySpatialGroup();
	
	S32 min_comp, max_comp, range;
	min_comp = lltrunc(mPatchp->getMinComposition());
	max_comp = lltrunc(ceil(mPatchp->getMaxComposition()));
	range = (max_comp - min_comp);
	range++;
	S32 new_base_comp = lltrunc(mPatchp->getMinComposition());
	if (range > 3)
	{
		if ((mPatchp->getMinComposition() - min_comp) > (max_comp - mPatchp->getMaxComposition()))
		{
			// The top side runs over more
			new_base_comp++;
		}
		range = 3;
	}

	// Pick the two closest detail textures for this patch...
	// Then create the draw pool for it.
	// Actually, should get the average composition instead of the center.
	mBaseComp = new_base_comp;

	/*
	//////////////////////////
	//
	// Figure out the strides
	//
	//

	U32 patch_width, render_stride, north_stride, east_stride, length;
	render_stride = mPatchp->getRenderStride();
	patch_width = mPatchp->getSurface()->getGridsPerPatchEdge();

	length = patch_width / render_stride;

	if (mPatchp->getNeighborPatch(NORTH))
	{
		north_stride = mPatchp->getNeighborPatch(NORTH)->getRenderStride();
	}
	else
	{
		north_stride = render_stride;
	}

	if (mPatchp->getNeighborPatch(EAST))
	{
		east_stride = mPatchp->getNeighborPatch(EAST)->getRenderStride();
	}
	else
	{
		east_stride = render_stride;
	}

	mLastLength = length;
	mLastStride = render_stride;
	mLastNorthStride = north_stride;
	mLastEastStride = east_stride;*/

	return TRUE;
}

void LunaVOVoxelSurface::updateFaceSize(S32 idx)
{
	if (idx != 0)
	{
		llwarns << "Terrain partition requested invalid face!!!" << llendl;
		return;
	}

	LLFace* facep = mDrawable->getFace(idx);

	S32 num_vertices = 0;
	S32 num_indices = 0;
	
	if (mLastStride)
	{
		getGeomSizesMain(mLastStride, num_vertices, num_indices);
		//getGeomSizesNorth(mLastStride, mLastNorthStride, num_vertices, num_indices);
		//getGeomSizesEast(mLastStride, mLastEastStride, num_vertices, num_indices);
	}

	facep->setSize(num_vertices, num_indices);	
}

BOOL LunaVOVoxelSurface::updateLOD()
{
	return TRUE;
}

void LunaVOVoxelSurface::getGeometry(
				LLStrider<LLVector3> &verticesp,
				LLStrider<LLVector3> &normalsp,
				LLStrider<LLColor4U> &colorsp,
				LLStrider<LLVector2> &texCoords0p,
				LLStrider<LLVector2> &texCoords1p,
				LLStrider<U16> &indicesp)
{
	LLFace* facep = mDrawable->getFace(0);

	U32 index_offset = facep->getGeomIndex();

	for(int x=0;x<mSurface->XScale;x++)
	for(int y=0;y<mSurface->YScale;y++)
	for(int z=0;z<mSurface->ZScale;z++)
	MarchCube(x,y,z,facep, 
		verticesp,
		normalsp,
		colorsp,
		texCoords0p,
		texCoords1p,
		indicesp,
		index_offset);
}

float LunaVOVoxelSurface::GetOffset(const float &fValue1, const float &fValue2, const float &fValueDesired)
{
        GLdouble fDelta = fValue2 - fValue1;
        if(fDelta == 0.0) { return 0.5; }
        return (fValueDesired - fValue1)/fDelta;
}

////////////////////////////////////
// Adapted from QT_MARCHING_CUBES //
////////////////////////////////////
/*
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 */
//MarchCube performs the Marching Cubes algorithm on a single cube
void LunaVOVoxelSurface::MarchCube(
	const int x, 
	const int y, 
	const int z, 
	LLStrider<LLVector3> &verticesp,
	LLStrider<LLVector3> &normalsp,
	LLStrider<LLColor4U> &colorsp,
	LLStrider<LLVector2> &texCoords0p,
	LLStrider<LLVector2> &texCoords1p,
	LLStrider<U16> &indicesp,
	U32 &index_offset)
{
	float fScale=1.f;
        int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        F32 fOffset;
        LLColor4 sColor;
        bool CubeValue[8];
        LLVector3d asEdgeVertex[12];
        LLVector3d asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            CubeValue[iVertex] = mVoxels->IsSolid(x+gLunaVertexOffsets[iVertex][0],y+gLunaVertexOffsets[iVertex][1],z+gLunaVertexOffsets[iVertex][2]);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(CubeValue[iVertexTest])     iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = gLunaCubeEdgeFlags[iFlagIndex];

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
                fOffset = GetOffset(CubeValue[ gLunaEdgeConnection[iEdge][0] ] ? 1.f : 0.f,CubeValue[ gLunaEdgeConnection[iEdge][1] ] ? 1.f : 0.f, fTv);
		
                asEdgeVertex[iEdge].mV[VX] = fX + (gLunaVertexOffset[ gLunaEdgeConnection[iEdge][0] ][0]  +  fOffset * gLunaEdgeDirection[iEdge][0]) * fScale;
                asEdgeVertex[iEdge].mV[VY] = fY + (gLunaVertexOffset[ gLunaEdgeConnection[iEdge][0] ][1]  +  fOffset * gLunaEdgeDirection[iEdge][1]) * fScale;
                asEdgeVertex[iEdge].mV[VZ] = fZ + (gLunaVertexOffset[ gLunaEdgeConnection[iEdge][0] ][2]  +  fOffset * gLunaEdgeDirection[iEdge][2]) * fScale;

                GetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].mV[VX], asEdgeVertex[iEdge].mV[VY], asEdgeVertex[iEdge].mV[VZ]);

		// Really wish I knew what the fuck I'm doing -- Fred
		*(verticesp++)=	gLunaEdgeVertex[iEdge];
		*(normalsp++)=	gLunaNormalsVertex[iEdge];
		*(colorsp++)=LLColor4U::white;
		*(texcoords0p++)=LLVector2(0.f,0.f); // Texture offsets, fix this
		*(texcoords1p++)=LLVector2(0.f,0.f); // This, too
		//*(indicesp++)=iEdge; 			// I seriously don't fucking know, maybe something to do with gLunaTriangleConnectionTable?
            }
	}
        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
            if(gLunaTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = gLunaTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                *(indicesp++)=iVertex;
		index_offset++;
            }
        }
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
void LunaVOVoxelSurface::MarchCube2(const int x, const int y, const int z, LLStrider<LLVector3> &verticesp,
										LLStrider<LLVector3> &normalsp,
										LLStrider<LLColor4U> &colorsp,
										LLStrider<LLVector2> &texCoords0p,
										LLStrider<LLVector2> &texCoords1p,
										LLStrider<U16> &indicesp,
										U32 &index_offset)
{
        /*GLint iVertex, iTetrahedron, iVertexInACube;
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
        }*/
}

/*
//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
GLvoid GL_Widget::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv)
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
*/
void LunaVOVoxelSurface::updateMainGeometry(LLFace *facep,
										LLStrider<LLVector3> &verticesp,
										LLStrider<LLVector3> &normalsp,
										LLStrider<LLColor4U> &colorsp,
										LLStrider<LLVector2> &texCoords0p,
										LLStrider<LLVector2> &texCoords1p,
										LLStrider<U16> &indicesp,
										U32 &index_offset)
{
	S32 i, j, x, y;

	U32 patch_size, render_stride;
	S32 num_vertices, num_indices;
	U32 index;

	render_stride = mLastStride;
	patch_size = mPatchp->getSurface()->getGridsPerPatchEdge();
	S32 vert_size = patch_size / render_stride;

	///////////////////////////
	//
	// Render the main patch
	//
	//

	num_vertices = 0;
	num_indices = 0;
	// First, figure out how many vertices we need...
	getGeomSizesMain(render_stride, num_vertices, num_indices);

	if (num_vertices > 0)
	{
		facep->mCenterAgent = mPatchp->getPointAgent(8, 8);

		// Generate patch points first
		for (j = 0; j < vert_size; j++)
		{
			for (i = 0; i < vert_size; i++)
			{
				x = i * render_stride;
				y = j * render_stride;
				mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
				*colorsp++ = LLColor4U::white;
				verticesp++;
				normalsp++;
				texCoords0p++;
				texCoords1p++;
			}
		}

		for (j = 0; j < (vert_size - 1); j++)
		{
			if (j % 2)
			{
				for (i = (vert_size - 1); i > 0; i--)
				{
					index = (i - 1)+ j*vert_size;
					*(indicesp++) = index_offset + index;

					index = i + (j+1)*vert_size;
					*(indicesp++) = index_offset + index;

					index = (i - 1) + (j+1)*vert_size;
					*(indicesp++) = index_offset + index;

					index = (i - 1) + j*vert_size;
					*(indicesp++) = index_offset + index;

					index = i + j*vert_size;
					*(indicesp++) = index_offset + index;

					index = i + (j+1)*vert_size;
					*(indicesp++) = index_offset + index;
				}
			}
			else
			{
				for (i = 0; i < (vert_size - 1); i++)
				{
					index = i + j*vert_size;
					*(indicesp++) = index_offset + index;

					index = (i + 1) + (j+1)*vert_size;
					*(indicesp++) = index_offset + index;

					index = i + (j+1)*vert_size;
					*(indicesp++) = index_offset + index;

					index = i + j*vert_size;
					*(indicesp++) = index_offset + index;

					index = (i + 1) + j*vert_size;
					*(indicesp++) = index_offset + index;

					index = (i + 1) + (j + 1)*vert_size;
					*(indicesp++) = index_offset + index;
				}
			}
		}
	}
	index_offset += num_vertices;
}


void LunaVOVoxelSurface::updateNorthGeometry(LLFace *facep,
										LLStrider<LLVector3> &verticesp,
										LLStrider<LLVector3> &normalsp,
										LLStrider<LLColor4U> &colorsp,
										LLStrider<LLVector2> &texCoords0p,
										LLStrider<LLVector2> &texCoords1p,
										LLStrider<U16> &indicesp,
										U32 &index_offset)
{
	S32 vertex_count = 0;
	S32 i, x, y;

	S32 num_vertices, num_indices;

	U32 render_stride = mLastStride;
	S32 patch_size = mPatchp->getSurface()->getGridsPerPatchEdge();
	S32 length = patch_size / render_stride;
	S32 half_length = length / 2;
	U32 north_stride = mLastNorthStride;
	
	///////////////////////////
	//
	// Render the north strip
	//
	//

	// Stride lengths are the same
	if (north_stride == render_stride)
	{
		num_vertices = 2 * length + 1;
		num_indices = length * 6 - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(8, 15) + mPatchp->getPointAgent(8, 16))*0.5f;

		// Main patch
		for (i = 0; i < length; i++)
		{
			x = i * render_stride;
			y = 16 - render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			*colorsp++ = LLColor4U::white;
			verticesp++;
			normalsp++;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}

		// North patch
		for (i = 0; i <= length; i++)
		{
			x = i * render_stride;
			y = 16;
			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}


		for (i = 0; i < length; i++)
		{
			// Generate indices
			*(indicesp++) = index_offset + i;
			*(indicesp++) = index_offset + length + i + 1;
			*(indicesp++) = index_offset + length + i;

			if (i != length - 1)
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + i + 1;
				*(indicesp++) = index_offset + length + i + 1;
			}
		}
	}
	else if (north_stride > render_stride)
	{
		// North stride is longer (has less vertices)
		num_vertices = length + length/2 + 1;
		num_indices = half_length*9 - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(7, 15) + mPatchp->getPointAgent(8, 16))*0.5f;

		// Iterate through this patch's points
		for (i = 0; i < length; i++)
		{
			x = i * render_stride;
			y = 16 - render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}

		// Iterate through the north patch's points
		for (i = 0; i <= length; i+=2)
		{
			x = i * render_stride;
			y = 16;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}


		for (i = 0; i < length; i++)
		{
			if (!(i % 2))
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + i + 1;
				*(indicesp++) = index_offset + length + (i/2);

				*(indicesp++) = index_offset + i + 1;
				*(indicesp++) = index_offset + length + (i/2) + 1;
				*(indicesp++) = index_offset + length + (i/2);
			}
			else if (i < (length - 1))
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + i + 1;
				*(indicesp++) = index_offset + length + (i/2) + 1;
			}
		}
	}
	else
	{
		// North stride is shorter (more vertices)
		length = patch_size / north_stride;
		half_length = length / 2;
		num_vertices = length + half_length + 1;
		num_indices = 9*half_length - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(15, 7) + mPatchp->getPointAgent(16, 8))*0.5f;

		// Iterate through this patch's points
		for (i = 0; i < length; i+=2)
		{
			x = i * north_stride;
			y = 16 - render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			*colorsp++ = LLColor4U::white;
			verticesp++;
			normalsp++;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}

		// Iterate through the north patch's points
		for (i = 0; i <= length; i++)
		{
			x = i * north_stride;
			y = 16;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
			vertex_count++;
		}

		for (i = 0; i < length; i++)
		{
			if (!(i%2))
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + i/2;
				*(indicesp++) = index_offset + half_length + i + 1;
			}
			else if (i < (length - 2))
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + i/2;
				*(indicesp++) = index_offset + i/2 + 1;

				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + i/2 + 1;
				*(indicesp++) = index_offset + half_length + i + 1;
			}
			else
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + i/2;
				*(indicesp++) = index_offset + half_length + i + 1;
			}
		}
	}
	index_offset += num_vertices;
}

void LunaVOVoxelSurface::updateEastGeometry(LLFace *facep,
										  LLStrider<LLVector3> &verticesp,
										  LLStrider<LLVector3> &normalsp,
										  LLStrider<LLColor4U> &colorsp,
										  LLStrider<LLVector2> &texCoords0p,
										  LLStrider<LLVector2> &texCoords1p,
										  LLStrider<U16> &indicesp,
										  U32 &index_offset)
{
	S32 i, x, y;

	S32 num_vertices, num_indices;

	U32 render_stride = mLastStride;
	S32 patch_size = mPatchp->getSurface()->getGridsPerPatchEdge();
	S32 length = patch_size / render_stride;
	S32 half_length = length / 2;

	U32 east_stride = mLastEastStride;

	// Stride lengths are the same
	if (east_stride == render_stride)
	{
		num_vertices = 2 * length + 1;
		num_indices = length * 6 - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(8, 15) + mPatchp->getPointAgent(8, 16))*0.5f;

		// Main patch
		for (i = 0; i < length; i++)
		{
			x = 16 - render_stride;
			y = i * render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}

		// East patch
		for (i = 0; i <= length; i++)
		{
			x = 16;
			y = i * render_stride;
			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}


		for (i = 0; i < length; i++)
		{
			// Generate indices
			*(indicesp++) = index_offset + i;
			*(indicesp++) = index_offset + length + i;
			*(indicesp++) = index_offset + length + i + 1;

			if (i != length - 1)
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + length + i + 1;
				*(indicesp++) = index_offset + i + 1;
			}
		}
	}
	else if (east_stride > render_stride)
	{
		// East stride is longer (has less vertices)
		num_vertices = length + half_length + 1;
		num_indices = half_length*9 - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(7, 15) + mPatchp->getPointAgent(8, 16))*0.5f;

		// Iterate through this patch's points
		for (i = 0; i < length; i++)
		{
			x = 16 - render_stride;
			y = i * render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}
		// Iterate through the east patch's points
		for (i = 0; i <= length; i+=2)
		{
			x = 16;
			y = i * render_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}

		for (i = 0; i < length; i++)
		{
			if (!(i % 2))
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + length + (i/2);
				*(indicesp++) = index_offset + i + 1;

				*(indicesp++) = index_offset + i + 1;
				*(indicesp++) = index_offset + length + (i/2);
				*(indicesp++) = index_offset + length + (i/2) + 1;
			}
			else if (i < (length - 1))
			{
				*(indicesp++) = index_offset + i;
				*(indicesp++) = index_offset + length + (i/2) + 1;
				*(indicesp++) = index_offset + i + 1;
			}
		}
	}
	else
	{
		// East stride is shorter (more vertices)
		length = patch_size / east_stride;
		half_length = length / 2;
		num_vertices = length + length/2 + 1;
		num_indices = 9*(length/2) - 3;

		facep->mCenterAgent = (mPatchp->getPointAgent(15, 7) + mPatchp->getPointAgent(16, 8))*0.5f;

		// Iterate through this patch's points
		for (i = 0; i < length; i+=2)
		{
			x = 16 - render_stride;
			y = i * east_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}
		// Iterate through the east patch's points
		for (i = 0; i <= length; i++)
		{
			x = 16;
			y = i * east_stride;

			mPatchp->eval(x, y, render_stride, verticesp.get(), normalsp.get(), texCoords0p.get(), texCoords1p.get());
			verticesp++;
			normalsp++;
			*colorsp++ = LLColor4U::white;
			texCoords0p++;
			texCoords1p++;
		}

		for (i = 0; i < length; i++)
		{
			if (!(i%2))
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + half_length + i + 1;
				*(indicesp++) = index_offset + i/2;
			}
			else if (i < (length - 2))
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + i/2 + 1;
				*(indicesp++) = index_offset + i/2;

				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + half_length + i + 1;
				*(indicesp++) = index_offset + i/2 + 1;
			}
			else
			{
				*(indicesp++) = index_offset + half_length + i;
				*(indicesp++) = index_offset + half_length + i + 1;
				*(indicesp++) = index_offset + i/2;
			}
		}
	}
	index_offset += num_vertices;
}

void LunaVOVoxelSurface::setPatch(LunaVoxelSurface *surfacep)
{
	mSurfacep = surfacep;

	dirtyPatch();
};


void LunaVOVoxelSurface::dirtyPatch()
{
	mDirtiedPatch = TRUE;
	dirtyGeom();
	mDirtyTerrain = TRUE;
	LLVector3 center = mSurfacep->getCenterRegion();

	setPositionRegion(center);

	setScale(LLVector3((float)mSurfacep->XScale, (float)mSurfacep->YScale, (float)mSurfacep->ZScale));
}

void LunaVOVoxelSurface::dirtyGeom()
{
	if (mDrawable)
	{
		gPipeline.markRebuild(mDrawable, LLDrawable::REBUILD_ALL, TRUE);
		mDrawable->getFace(0)->mVertexBuffer = NULL;
		mDrawable->movePartition();
	}
}

void LunaVOVoxelSurface::getGeomSizesMain(const S32 stride, S32 &num_vertices, S32 &num_indices)
{
	S32 sz_x = mSurfacep->XScale;
	S32 sz_y = mSurfacep->YScale;
	S32 sz_z = mSurfacep->ZScale;

	// First, figure out how many vertices we need...
	S32 vert_size = (sz_x * sz_y * sz_z) / stride;
	if (vert_size >= 2)
	{
		num_vertices += vert_size * vert_size;
		num_indices += 6 * (vert_size - 1)*(vert_size - 1);
	}
}

void LunaVOVoxelSurface::getGeomSizesNorth(const S32 stride, const S32 north_stride,
										 S32 &num_vertices, S32 &num_indices)
{
	//
}

void LunaVOVoxelSurface::getGeomSizesEast(const S32 stride, const S32 east_stride,
										S32 &num_vertices, S32 &num_indices)
{
	//
}

/// TODO: Fix
BOOL LunaVOVoxelSurface::lineSegmentIntersect(
	const LLVector3& start, 
	const LLVector3& end, 
	S32 face, 
	BOOL pick_transparent, 
	S32 *face_hitp,
	LLVector3* intersection,
	LLVector2* tex_coord, 
	LLVector3* normal, 
	LLVector3* bi_normal)	
{

	if (!lineSegmentBoundingBox(start, end))
	{
		return FALSE;
	}

	LLVector3 delta = end-start;
		
	LLVector3 pdelta = delta;
	pdelta.mV[2] = 0;

	F32 plength = pdelta.length();
	
	F32 tdelta = 1.f/plength;

	LLVector3 origin = start - mRegionp->getOriginAgent();

	if (mRegionp->getLandHeightRegion(origin) > origin.mV[2])
	{
		//origin is under ground, treat as no intersection
		return FALSE;
	}

	//step one meter at a time until intersection point found

	const LLVector3* ext = mDrawable->getSpatialExtents();
	F32 rad = (delta*tdelta).magVecSquared();

	F32 t = 0.f;
	while ( t <= 1.f)
	{
		LLVector3 sample = origin + delta*t;
		
		if (AABBSphereIntersectR2(ext[0], ext[1], sample+mRegionp->getOriginAgent(), rad))
		{
			F32 height = mRegionp->getLandHeightRegion(sample);
			if (height > sample.mV[2])
			{ //ray went below ground, positive intersection
				//quick and dirty binary search to get impact point
				tdelta = -tdelta*0.5f;
				F32 err_dist = 0.001f;
				F32 dist = fabsf(sample.mV[2] - height);

				while (dist > err_dist && tdelta*tdelta > 0.0f)
				{
					t += tdelta;
					sample = origin+delta*t;
					height = mRegionp->getLandHeightRegion(sample);
					if ((tdelta < 0 && height < sample.mV[2]) ||
						(height > sample.mV[2] && tdelta > 0))
					{ //jumped over intersection point, go back
						tdelta = -tdelta;
					}
					tdelta *= 0.5f;
					dist = fabsf(sample.mV[2] - height);
				}

				if (intersection)
				{
					F32 height = mRegionp->getLandHeightRegion(sample);
					if (fabsf(sample.mV[2]-height) < delta.length()*tdelta)
					{
						sample.mV[2] = mRegionp->getLandHeightRegion(sample);
					}
					*intersection = sample + mRegionp->getOriginAgent();
				}

				if (normal)
				{
					*normal = mRegionp->getLand().resolveNormalGlobal(mRegionp->getPosGlobalFromRegion(sample));
				}

				return TRUE;
			}
		}

		t += tdelta;
		if (t > 1 && t < 1.f+tdelta*0.99f)
		{ //make sure end point is checked (saves vertical lines coming up negative)
			t = 1.f;
		}
	}


	return FALSE;
}

void LunaVOVoxelSurface::updateSpatialExtents(LLVector3& newMin, LLVector3 &newMax)
{
	LLVector3 posAgent = getPositionAgent();
	LLVector3 scale = getScale();
	newMin = posAgent-scale*0.5f; // Changing to 2.f makes the culling a -little- better, but still wrong
	newMax = posAgent+scale*0.5f;
	mDrawable->setPositionGroup((newMin+newMax)*0.5f);
}

U32 LunaVOVoxelSurface::getPartitionType() const
{ 
	return LLViewerRegion::PARTITION_TERRAIN; 
}

LLTerrainPartition::LLTerrainPartition()
: LLSpatialPartition(LLDrawPoolTerrain::VERTEX_DATA_MASK)
{
	mOcclusionEnabled = FALSE;
	mRenderByGroup = FALSE;
	mInfiniteFarClip = TRUE;
	mBufferUsage = GL_DYNAMIC_DRAW_ARB;
	mDrawableType = LLPipeline::RENDER_TYPE_TERRAIN;
	mPartitionType = LLViewerRegion::PARTITION_TERRAIN;
}

LLVertexBuffer* LLTerrainPartition::createVertexBuffer(U32 type_mask, U32 usage)
{
	return new LunaVertexBufferTerrain();
}

void LLTerrainPartition::getGeometry(LLSpatialGroup* group)
{
	LLFastTimer ftm(LLFastTimer::FTM_REBUILD_TERRAIN_VB);

	LLVertexBuffer* buffer = group->mVertexBuffer;

	//get vertex buffer striders
	LLStrider<LLVector3> vertices;
	LLStrider<LLVector3> normals;
	LLStrider<LLVector2> texcoords2;
	LLStrider<LLVector2> texcoords;
	LLStrider<LLColor4U> colors;
	LLStrider<U16> indices;

	llassert_always(buffer->getVertexStrider(vertices));
	llassert_always(buffer->getNormalStrider(normals));
	llassert_always(buffer->getTexCoord0Strider(texcoords));
	llassert_always(buffer->getTexCoord1Strider(texcoords2));
	llassert_always(buffer->getColorStrider(colors));
	llassert_always(buffer->getIndexStrider(indices));

	U32 indices_index = 0;
	U32 index_offset = 0;

	for (std::vector<LLFace*>::iterator i = mFaceList.begin(); i != mFaceList.end(); ++i)
	{
		LLFace* facep = *i;

		facep->setIndicesIndex(indices_index);
		facep->setGeomIndex(index_offset);
		facep->mVertexBuffer = buffer;

		LunaVOVoxelSurface* patchp = (LunaVOVoxelSurface*) facep->getViewerObject();
		patchp->getGeometry(vertices, normals, colors, texcoords, texcoords2, indices);

		indices_index += facep->getIndicesCount();
		index_offset += facep->getGeomCount();
	}

	buffer->setBuffer(0);
	mFaceList.clear();
}

