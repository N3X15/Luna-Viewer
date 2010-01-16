#include "llviewerprecompiledheaders.h"

#include "LuaUI.h"

/* CEGUI UI Bindings*/
#include "CEGUI.h"
#include "CEGUILua.h"
#include "CEGUIPropertyHelper.h"	

// FlexLife UI management.
#include "FlexUI.h"

/** 
	 LuaUI 
	   ^
	   |
           |
+-----[MAIN|THREAD]-----+
|          v            |
|     FlexUIManager     |
|     |           |     |
|     |  CEGUI    |     |
|     \___________/     |
|                       |
+-----------------------+

FlexUI.{h,cpp} = FlexUIManager
LuaUI.{h,cpp}  = UI bindings.
*/



