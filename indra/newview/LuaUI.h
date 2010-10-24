/**
* Luna UI Integration (Stopgap until native Lua UI is complete)
*	N3X15 <nexisentertainment@gmail.com>
*/
#ifndef LUAUI_H
#define LUAUI_H

#include "llui.h"
#include "llfloater.h"
#include "v2math.h"
#include "LunaLua.h"

class UIControl
{
private:
	LLUICtrl *mControl;

public:
	UIControl();

};

class Floater
{
private:
	LLFloater *mFloater;
public:
	// This must be called from Lua.  No ifs, ands, or buts.
	Floater(std::string name);

	void setSize(float h, float w);
	float getHeight();
	float getWidth();

	void setPosition(double x, double y);
	LLVector2 getPos();

	void addChild(std::string name,UIControl *x);
	UIControl *getChild(std::string name);
	void rmChild(std::string name);

	void addHook(lua_State*L);

private:
	std::map<std::string,UIControl> mChildren;
};

#endif //Lua_H
