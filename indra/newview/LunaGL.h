/**
 * @file GL.h
 * @brief Luna Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2010 Luna Contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id$
 */

/* Provides GL drawing functions:

GL.vertex2i 1 2
GL.flush()
*/
#ifndef GL4LUA_H
#define GL4LUA_H

#include "v2math.h"
#include "v3math.h"
#include "v4coloru.h"
#include "llstrider.h"
#include "llmemory.h"
#include "llglheaders.h"
#include "llrender.h"


enum GeomModes {
	GEOM_TRIANGLES = 0,
	GEOM_TRIANGLE_STRIP,
	GEOM_TRIANGLE_FAN,
	GEOM_POINTS,
	GEOM_LINES,
	GEOM_LINE_STRIP,
	GEOM_QUADS,
	GEOM_LINE_LOOP,
	GEOM_NUM_MODES
};

enum CompareFunc
{
	CF_NEVER = 0,
	CF_ALWAYS,
	CF_LESS,
	CF_LESS_EQUAL,
	CF_EQUAL,
	CF_NOT_EQUAL,
	CF_GREATER_EQUAL,
	CF_GREATER,
	CF_DEFAULT
};

enum BlendType
{
	BT_ALPHA = 0,
	BT_ADD,
	BT_ADD_WITH_ALPHA,	// Additive blend modulated by the fragment's alpha.
	BT_MULT,
	BT_MULT_X2,
	BT_REPLACE
};

enum BlendFactor
{
	BF_ONE = 0,
	BF_ZERO,
	BF_DEST_COLOR,
	BF_SOURCE_COLOR,
	BF_ONE_MINUS_DEST_COLOR,
	BF_ONE_MINUS_SOURCE_COLOR,
	BF_DEST_ALPHA,
	BF_SOURCE_ALPHA,
	BF_ONE_MINUS_DEST_ALPHA,
	BF_ONE_MINUS_SOURCE_ALPHA
};

// Refreshes renderer state to the cached values
// Needed when the render context has changed and invalidated the current state
void refreshState();

void translatef(const float& x, const float& y, const float& z);
void scalef(	const float& x, const float& y, const float& z);

void pushMatrix();
void popMatrix();

void flush();

void begin(const int mode);
void end();
void vertex2i(const int& x, const int& y);
void vertex2f(const float& x, const float& y);
void vertex3f(const float& x, const float& y, const float& z);
void vertex2fv(const float* v);
void vertex3fv(const float* v);

void texCoord2i(const int& x, const int& y);
void texCoord2f(const float& x, const float& y);
void texCoord2fv(const float* tc);

void color4ub(const int& r, const int& g, const int& b, const int& a);
void color4f(const float& r, const float& g, const float& b, const float& a);
void color4fv(const float* c);
void color3f(const float& r, const float& g, const float& b);
void color3fv(const float* c);
void color4ubv(const GLubyte* c);

void setColorMask(bool writeColor, bool writeAlpha);
void setColorMask(bool writeColorR, bool writeColorG, bool writeColorB, bool writeAlpha);
void setSceneBlendType(BlendType type);

void setAlphaRejectSettings(CompareFunc func, float value = 0.01f);

void blendFunc(BlendFactor sfactor, BlendFactor dfactor);

LLTexUnit* getTexUnit(int index);

int getCurrentTexUnitIndex();

bool verifyTexUnitActive(int unitToVerify);

void debugTexUnits();

void clearErrors();


// Image shit
LLViewerImage * getImage(const LLUUID &image_id,
	   bool usemipmaps=true,
	   bool level_immediate=false,
	   int internal_format=0,
	   int primary_format=0,
	   LLHost request_from_host=LLHost());

LLViewerImage * getImageFromUrl(const std::string& url,
	   bool usemipmaps=true,
	   bool level_immediate=false,
	   int internal_format=0,
	   int primary_format=0,
	   const LLUUID& force_id=LLUUID::null);

LLViewerImage * getImageFromFile(const std::string& filename,
	   bool usemipmaps=true,
	   bool level_immediate=false,
	   int internal_format=0,
	   int primary_format=0,
	   const LLUUID& force_id=LLUUID::null);

LLImageGL* getViewerImage2GL(LLViewerImage* in);
#endif
