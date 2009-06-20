#pragma once
#include "llfloater.h"
#include "llpanel.h"
#include "lltexteditor.h"

class LuaUI
{
public:
	LuaUI(void);
	~LuaUI(void);
};


class LuaFloater : public LLFloater
{
public:
	LuaFloater(const char* name);
	~LuaFloater(void);
};

class LuaPanel : public LLPanel
{
public:
	LuaPanel(const char* name);
	~LuaPanel(void);
};

class LuaTextEditor : public LLTextEditor
{
public:
	LuaTextEditor(const char* name);
	~LuaTextEditor(void);
};

void LuaTestFloater();