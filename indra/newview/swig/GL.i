/**
 * @file GL.swig
 * @brief Luna Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2010 N3X15
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

// Utilities functions the user interface needs

#include "linden_common.h"

#include <string>
#include <map>

// Linden library includes
// MIA: #include "audioengine.h"

#include "llglheaders.h"
#include "v2math.h"
#include "v4color.h"
#include "llrender.h"
#include "llrect.h"
#include "llimagegl.h"
#include "lldir.h"
#include "llfontgl.h"

#include "LunaGL.h"
%}

%include <std_string.i>
%include "LLUUID.i"

//%include "../../llrender/llrender.h" // GL drawing methods
%include "../../llrender/llglheaders.h" // GL drawing methods

%include "../LunaGL.h"

