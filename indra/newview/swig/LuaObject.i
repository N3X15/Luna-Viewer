/**
 * @file LuaObject.swig
 * @brief FlexLife Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 N3X15
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
 * $Id: LuaBase.swig 189 2010-01-10 22:42:29Z n3x15 $
 */

%{
#include "llviewerobject.h"
%}

%include "llviewerobject.h"

%rename (Object) LLViewerObject;
