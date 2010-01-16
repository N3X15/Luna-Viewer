/**
 * @file LuaBase.swig
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
 * $Id$
 */

%module SL

//Fixed for Visual Studio
//Has to be in begin section. VS will choke if this include isn't the very
//first thing in the file. If GCC doesn't like it, encapsulate in 
//#ifdef _WIN32 and include it be in #infdef _WIN32 in the header section
//http://www.swig.org/Doc1.3/SWIG.html  5.6 Code Insertion
%begin %{
#include "llviewerprecompiledheaders.h"
%}
%{


#include "stdtypes.h"
#include "LuaUI.h"
#include "llfloater.h"
#include "lllineeditor.h"
#include "llpanel.h"
#include "llbutton.h"
#include "lliconctrl.h"
#include "LuaBase_f.h"
#include "LuaAvatar_f.h"
#include "LuaAgent_f.h"
#include "LuaParticles.h"
#include "LuaWindlight.h"
#include "llprimitive.h"
#include "llviewerobject.h"
#include "llvovolume.h"
#include "llagent.h"
#include "llselectmgr.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"
#include "llcharacter.h"
#include "llagent.h"
%}

%include <std_string.i>
%include "../newview/LLUUID.i"

%import "../llcommon/stdtypes.h"
%import "../llprimitive/llprimitive.h"
%import "../newview/llagent.h"
%import "../newview/llviewerregion.h"
%import "../newview/llviewerobject.h"
%import "../newview/llvovolume.h"
%import "../newview/llselectmgr.h"
%import "../llui/llfloater.h"
%import "../llui/llpanel.h"
%import "../llui/llbutton.h"
%import "../llui/lliconctrl.h"

%import "../newview/llviewerpartsource.h"


%rename(SendChat)		LuaSendChat(const char *msg, bool IsDebug=false);

%ignore				LuaPrint(const char *msg, const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0,const char *arg5=0);
%native(print) int LuaBase_print(lua_State *L); 
%{
// Lua 5.2.x's print(...) method, from the sourcecode.
// Slightly modified to write to SL instead.
static int LuaBase_print (lua_State *L) {
	std::string out("");
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getfield(L, LUA_ENVIRONINDEX, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		size_t l;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);  /* get result */
		if (s == NULL)
			return luaL_error(L, "tostring must return a string to print");
		if (i>1) 
			out.append("     ");
		out.append(s);
		lua_pop(L, 1);  /* pop result */
	}
	LuaPrint(out.c_str());
	return 0;
}
%}

%rename(RegionFromName)		LuaRegionFromName(const char *msg);
%rename(GetCurrentRegion)	LuaGetCurrentRegion();
%rename(GetCWD)			LuaGetCWD();
%rename(error)			LuaError(const char* msg);

%rename(say)			LuaSay(const char* msg, int channel=0);
%rename(shout)			LuaShout(const char* msg, int channel=0);
%rename(whisper)		LuaWhisper(const char* msg, int channel=0);

%rename(tp)			LuaTp(const char* SimName, int x=128, int y=128, int z=128);
%rename(exists)			Lua_exists(const char* Filename);
%rename(touch)			LuaTouch(const LLUUID& id);

%rename(wear)			LuaWear(const LLUUID& assetid);
%rename(RemoveAllWearables)	LuaRemoveAllWearables();
%rename(setTEImage)		LuaSetTEImage(int index,const LLUUID& id);

%include "LuaBase_f.h"

%rename(key2name)		LuaFindAvatarName(const LLUUID& id);
%rename(name2key)		LuaFindAvatarKey(const char* fullname);
%rename(UpdateAppearance)	LuaUpdateAppearance();

%include "LuaAvatar_f.h"

%include "LuaAgent_f.h"

%include "../llcharacter/llcharacter.h"
%include "../newview/llagent.h"


extern bool gAllowWorldMap;
extern LLAgent *gAgent;

%include "LLVector.i"

%rename (Vector2)	LLVector2;
%rename (Vector3)	LLVector3;
%rename (Vector4)	LLVector4;
//%rename (Color4)	LLColor4;

%include "LuaParticles.h"
%include "LuaWindlight.h"

%include "swig/LuaObject.i"

%include "../newview/LuaUI.h"

%rename (Floater) 	LuaFloater;
%rename (Button) 	LuaButton;
%rename (LineEditor) 	LuaLineEditor;
%rename (IconCtrl)	LuaIconCtrl;

%rename (Agent) 		LuaAgent;
%rename (alert) 		LuaAlert;
%rename (dumpVisualParams) 	LuaDumpVisualParams;
%rename (dumpVisualParamsToLuaCode) LuaDumpVisualParamsToLuaCode;
%rename (Floater) 		LuaFloater;
%rename (getAvatar) 		LuaGetAvatar;
%rename (LineEditor) 		LuaLineEditor;
%rename (loadWearable) 		LuaLoadWearable;
%rename (ObjectManager) 	LuaObjectManager;
%rename (Panel) 		LuaPanel;
%rename (saveWearable) 		LuaSaveWearable;
%rename (SelectionManager) 	LuaSelectionManager;
%rename (sendRawChat) 		LuaSendRawChat;
%rename (TextBox) 		LuaTextBox;
%rename (UICtrl) 		LuaUICtrl;
%rename (UIManager)		LuaUIManager;

