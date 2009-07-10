/* Code goes here! */

#include "llviewerprecompiledheaders.h"
#include "SLua.h"
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "lluictrlfactory.h"

lua_State* SLua::_LuaState = 0;

extern "C" {
	extern int luaopen_SLua(lua_State* L);
}

lua_State* SLua::init()
{
	LL_INFOS("LUA") << "Creating initial LUA state." << LL_ENDL;
	_LuaState = lua_open();
	if(_LuaState)
	{
		LL_INFOS("LUA") << "Successfully created a LUA state. Attempting lib load and function registration." << LL_ENDL;
		luaL_openlibs(_LuaState);
		if(!luaopen_SLua(_LuaState))
			LL_ERRS("LUA") << "Failed to register the SLua library into the LUA state." << LL_ENDL;
		if(!luaL_loadfile(_LuaState, gSavedSettings.getString("SLuaStartFile").c_str()))
		{
			lua_pcall(_LuaState, 0, LUA_MULTRET, 0);
		}
		else
		{
			LL_ERRS("LUA") << "Failed to open the initial LUA file: " << gSavedSettings.getString("SLuaStartFile") << LL_ENDL;
		}
	}
	else
	{
		LL_ERRS("LUA") << "Failed to initilize a new LUA state." << LL_ENDL;
	}
	return _LuaState;
}

void SLua::close()
{
	lua_close(_LuaState);
}

void SLua::runFile(std::string file)
{
	luaL_dofile(_LuaState, file.c_str());
}

lua_State* SLua::restart()
{
	if(_LuaState)
		close();
	return init();
}

bool SLua::callbackFunction(void* object, std::string func_name)
{
	lua_State* L = getLuaState();
	lua_getglobal(L, func_name.c_str());
	if(!lua_isfunction(L, lua_gettop(L)))
		return false;
	lua_pushlightuserdata(L, object);
	lua_pcall(L, 1, 1, 0);
	return true;
}

void LuaPrint(std::string message)
{
	LL_WARNS("LUA") << "LUA: " << message << LL_ENDL;
}

SLuaFloater::SLuaFloater()
{
LL_WARNS("LUA") << "Default SLuaFloater constructor, would produce a floater that could not be accessed again." << LL_ENDL;
}

SLuaFloater::SLuaFloater(std::string xml)
{
	LLUICtrlFactory::getInstance()->buildFloater(static_cast<LLFloater*>(this), xml);
}

SLuaFloater::SLuaFloater(std::string name, std::string title)
{
	//LL_INFOS("LUA") << "We created a floater!" << LL_ENDL;
	static_cast<LLFloater*>(this)->setName(name);
	this->initFloater(title, TRUE, 0, 0, FALSE, TRUE, TRUE);
	this->setTitle(title);
	this->setVisibleAndFrontmost();
	this->center();
}

SLuaFloater::SLuaFloater(std::string name, std::string title, int left, int top, int height, int width)
{
	//LL_INFOS("LUA") << "We created a floater!" << LL_ENDL;
	static_cast<LLFloater*>(this)->setName(name);
	this->initFloater(title, TRUE, 0, 0, FALSE, TRUE, TRUE);
	static_cast<LLFloater*>(this)->setOrigin(left, top);
	static_cast<LLFloater*>(this)->reshape(width, height);
	this->setTitle(title);
	this->setVisibleAndFrontmost();
}

void SLuaFloater::LuaSetTitle(std::string new_title)
{
	this->setTitle(new_title);
}

void SLuaFloater::LuaAddChild(SLuaUICtrl* new_child)
{
	static_cast<LLFloater*>(this)->addChild(new_child);
}

void SLuaFloater::LuaAddButton(SLuaButton* new_button)
{
	static_cast<LLFloater*>(this)->addChild(static_cast<LLButton*>(new_button));
}

void SLuaFloater::onClose()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClose";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onClose function found within lua code. Called it." << LL_ENDL;
}

void SLuaFloater::onClickClose()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClickClose";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onClickClose function found within lua code. Called it." << LL_ENDL;
}


void SLuaFloater::onFocusReceived()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onFocusReceived";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onFocusReceived function found within lua code. Called it." << LL_ENDL;
}

void SLuaFloater::onFocusLost()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onFocusLost";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onFocusLost function found within lua code. Called it." << LL_ENDL;
}

void SLuaFloater::onClickMinimize()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClickMinimize";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onClickMinimize function found within lua code. Called it." << LL_ENDL;
}

SLuaPanel::SLuaPanel()
{}

SLuaPanel::SLuaPanel(std::string name)
{}

SLuaPanel::SLuaPanel(std::string name, int left, int top, int height, int width)
{}

SLuaPanel::SLuaPanel(std::string name, int left, int top, int height, int width, bool border)
{}

SLuaUICtrl::SLuaUICtrl()
{}

SLuaUICtrl::SLuaUICtrl(std::string name)
{}

SLuaUICtrl::SLuaUICtrl(std::string name, int left, int top, int height, int width)
{}

//This name is misleading, I know.
void SLuaUICtrl::LuaSetRect(int left, int top, int height, int width)
{
	this->setOrigin(left, top);
	this->reshape(width, height);
	//this->setRect(LLRect(left, top, height, width));
}

SLuaButton::SLuaButton()
{}

SLuaButton::SLuaButton(std::string name, int left, int top, int height, int width, std::string label)
{}

SLuaButton::SLuaButton(std::string name, int left, int top, int height, int width, std::string label, std::string lua_func_name)
{}

void SLuaButton::LuaSetImages(std::string unselected_image, std::string selected_image)
{}

void SLuaButton::LuaSetLabel(std::string label)
{}

void SLuaButton::LuaSetLabels(std::string unselected_label, std::string selected_label)
{}

void SLuaButton::onHeldDown(void *userdata)
{
	lua_State* L = SLua::getLuaState();
	std::string temp_name = static_cast<LLButton*>(this)->getName() + "_onHeldDown";
	lua_getglobal(L, temp_name.c_str());
	if(lua_isfunction(L, 0))
	{
		lua_pushlightuserdata(L, this);
		lua_pushlightuserdata(L, userdata);
		lua_pcall(L, 2, 1, 0);
	}
}

SLuaLineEditor::SLuaLineEditor()
{}

SLuaLineEditor::SLuaLineEditor(std::string name, int left, int top, int height, int width, std::string default_text)
{}

void SLuaLineEditor::LuaSetText(std::string new_text)
{
	this->setText(new_text);
}

std::string SLuaLineEditor::LuaGetText()
{
	return this->getText();
}

void SLuaLineEditor::onCommit()
{
	std::string temp_name = static_cast<LLLineEditor*>(this)->getName() + "_onCommit";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onCommit function found within lua code. Called it." << LL_ENDL;
}

SLuaTextBox::SLuaTextBox()
{}

SLuaTextBox::SLuaTextBox(std::string name, int left, int top, int height, int width, std::string default_text)
{}

void SLuaTextBox::LuaSetMaxWidth(int max_width)
{
	this->setWrappedText(this->getText(), max_width);
}

void SLuaTextBox::LuaSetText(std::string new_text)
{
	this->setText(new_text);
}

std::string SLuaTextBox::LuaGetText()
{
	return this->getText();
}

void SLuaTextBox::onCommit()
{
	std::string temp_name = static_cast<LLTextBox*>(this)->getName() + "_onCommit";
	if(SLua::callbackFunction(this, temp_name))
		LL_WARNS("LUA") << "LUA: onCommit function found within lua code. Called it." << LL_ENDL;
}

SLuaFloater* SLuaUIManager::getFloater(std::string name)
{
	return static_cast<SLuaFloater*>(gViewerWindow->getRootView()->getChild<LLFloater>(name));
}