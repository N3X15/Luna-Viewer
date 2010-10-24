#include "llviewerprecompiledheaders.h"

#include "LuaUI.h"

#include "llfloater.h"
#include "llui.h"

// Constructor
Floater::Floater(std::string name)
{
	{
		// Lock Lua thread into a critical section.
		FLLua::CriticalSection cs();

		// Create new floater instance.
		mFloater = new LLFloater(name);
	}
}