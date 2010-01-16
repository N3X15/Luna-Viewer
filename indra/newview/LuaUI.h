/*  A free addition to the Second Life(tm) viewer that implements
	the LUA 5.1.x scripting language by Zwagoth Klaar(Jeffrey MacLellan)

	Implementation notes:
	*	Under NO CIRCUMSTANCE should ANY function related to money be
		accessed or made accessible to LUA scripts.
	*	Raw packet creation is not an option(breaks the above clause)
	*	Multiple callbacks will be done in LUA, if at all.
	*	Most UI should be migrated(in time) over to LUA for the purpose of expandability
*/

#ifndef LUAUI_H
#define LUAUI_H

#include "FLLua.h"
#include "CEGUI/CEGUILua.h"



class UI
{
public:
//	static bool RequestFloater(std::string name);
//	static bool RequestTextbox(std::string name);
};

#endif //Lua_H
