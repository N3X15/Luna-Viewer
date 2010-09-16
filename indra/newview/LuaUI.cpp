#ifdef _WITH_CEGUI
#include "llviewerprecompiledheaders.h"

#include "LuaUI.h"

// Luna UI management.

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


#endif
