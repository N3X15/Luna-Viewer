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

#include "llui.h"
#include "llfloater.h"
#include "v2math.h"
#include "LunaLua.h"

class Floater: public LLFloater
{
private:
	LLFloater mFloater;
public:
	Floater(std::string name)
	{
		FLLua::CriticalSection cs();
		this->LLFloater(name);
		//self::LLFloater(name);
	}
};
Floater * createFloater(std::string name, LLVector2 scale, LLVector2 pos=LLVector2(0.f,0.f))
{
	Floater *f = new Floater(name);
}

#endif //Lua_H
