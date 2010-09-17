/**
 * @file LuaBase.swig
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

%{


#include "stdtypes.h"

#include "llviewerregion.h"

//#include "llprimitive.h"
//#include "llviewerobject.h"
//#include "llvovolume.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"
#include "llhttpclient.h"
#include "m4math.h"
#include "llvoclouds.h"

#include "swig/Region/Region.h"
%}
%include <std_string.i>
%include "LLUUID.i"
//%include "LLVector.i" // Getting lots of redefinition errors...

%include "../llviewerregion.h"

%rename (Region) LLViewerRegion;

%include "Region/Region.h"
