/**
 * @file Object.swig
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
 * $Id: Region.swig 230 2010-03-08 10:35:35Z NexisEntertainment@gmail.com $
 */

%module Object

//Fixed for Visual Studio
//Has to be in begin section. VS will choke if this include isn't the very
//first thing in the file. If GCC doesn't like it, encapsulate in 
//#ifdef _WIN32 and include it be in #infdef _WIN32 in the header section
//http://www.swig.org/Doc1.3/SWIG.html  5.6 Code Insertion
#ifdef _WIN32
%begin %{
#include "llviewerprecompiledheaders.h"
%}
#endif
%{
// Everything from here down to llviewerobject.h is a dependency of LLViewerObject(List).
//	Yes.  All of these.
//	Removing them causes all kinds of hell because of shitty dependency resolution.
//	-- Nexypoo
#include "linden_common.h"
#include "material_codes.h"
#include "imageids.h"
#include "stdtypes.h"

#include "llassetstorage.h"
#include "llbbox.h"
#include "llcamera.h"
#include "llfile.h"
#include "lldarrayptr.h"
#include "llhudtext.h"
#include "llhudicon.h"
#include "llinventory.h"
#include "llmemory.h"
#include "llmemtype.h"
#include "llprimitive.h"
#include "lluuid.h"
#include "llvoinventorylistener.h"
#include "object_flags.h"
#include "llquaternion.h"
#include "v3dmath.h"
#include "v3math.h"
#include "llvertexbuffer.h"
#include "llpartdata.h"
#include "xform.h"
#include "lldrawable.h"
#include "llviewerpartsource.h"

#include "llviewerobject.h"
#include "llviewerobjectlist.h"

#include "swig/Object/Object.h"
%}

%include <std_string.i>
%include "LLUUID.i"
//%include "LLVector.i" // Getting lots of redefinition errors...

// I have no idea why these are failing, but they are during compile.  
// Probably a missing dependency, but I have no idea /where/. -- Nexypoo
%ignore LLPrimitive::getVolumeType();
%ignore LLPrimitive::setVolumeType(const U8);

%include "../../llmath/xform.h"
%include "../../llprimitive/llprimitive.h"
%include "../../newview/llviewerobject.h"

%rename (Object) LLViewerObject;

%include "Object/Object.h"
