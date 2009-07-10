/*  A free addition to the Second Life(tm) viewer that implements
	the LUA 5.1.x scripting language by Zwagoth Klaar(Jeffrey MacLellan)

	Implementation notes:
	*	Under NO CIRCUMSTANCE should ANY function related to money be
		accessed or made accessible to LUA scripts.
	*	Raw packet creation is not an option(breaks the above clause)
	*	Multiple callbacks will be done in LUA, if at all.
	*	Most UI should be migrated(in time) over to LUA for the purpose of expandability
*/

#ifndef SLUA_H
#define SLUA_H

#ifdef __cplusplus
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lstate.h"
#include "lualib.h"
}
#endif
#include "llfloater.h"
#include "lluictrl.h"
#include "llagent.h"
#include "llvovolume.h"
#include "llbutton.h"
#include "lllineeditor.h"
#include "lltextbox.h"
#include "llpanel.h"
#include "llselectmgr.h"

#ifndef SWIG
class SLua
{
public:
	static lua_State* getLuaState() { return _LuaState; }
	static lua_State* init();
	static void wrapClasses();
	static void close();
	static lua_State* restart();
	static void handle_error(lua_State* L);
	static void runFile(std::string file);
	static bool callbackFunction(void* object, std::string func_name);
private:
	static lua_State* _LuaState;
};
#endif
//class SLuaNetwork : LLMessageSystem
//{
//};

void LuaPrint(std::string message);

class SLuaUICtrl : public LLUICtrl
{
public:
	SLuaUICtrl();
	SLuaUICtrl(std::string name);
	SLuaUICtrl(std::string name, int left, int top, int height, int width);
	void LuaSetRect(int left, int top, int height, int width);
};

class SLuaAgent : public LLAgent
{
public:
};

class SLuaObjectManager : public LLVolumeMgr
{
public:
};

class SLuaSelectionManager : public LLSelectMgr
{
public:
};

class SLuaObject : public LLVOVolume
{
public:
};

class SLuaPanel : public LLPanel, public SLuaUICtrl
{
public:
	SLuaPanel();
	SLuaPanel(std::string name);
	SLuaPanel(std::string name, int left, int top, int height, int width);
	SLuaPanel(std::string name, int left, int top, int height, int width, bool border);
};

class SLuaButton : public LLButton, public SLuaUICtrl
{
public:
	SLuaButton();
	SLuaButton(std::string name, int left, int top, int height, int width, std::string label);
	SLuaButton(std::string name, int left, int top, int height, int width, std::string label, std::string lua_func_name);
	void LuaSetImages(std::string unselected_image, std::string selected_image);
	void LuaSetLabel(std::string label);
	void LuaSetLabels(std::string unselected_label, std::string selected_label);
	virtual void onHeldDown(void *usedata);
};

class SLuaFloater : public LLFloater, public SLuaPanel
{
public:
	SLuaFloater();
	SLuaFloater(std::string xml);
	SLuaFloater(std::string name, std::string title);
	SLuaFloater(std::string name, std::string title, int left, int top, int height, int width);
	void LuaSetTitle(std::string new_title);
	void LuaAddChild(SLuaUICtrl* new_child);
	void LuaAddButton(SLuaButton* new_button);
	virtual void onFocusLost();
	virtual void onFocusReceived();
	virtual void onClose();
	virtual void onClickClose();
	virtual void onClickMinimize();
};

class SLuaLineEditor : public LLLineEditor, public SLuaUICtrl
{
public:
	SLuaLineEditor();
	SLuaLineEditor(std::string name, int left, int top, int height, int width, std::string default_text = "");
	void LuaSetText(std::string new_text);
	std::string LuaGetText();
	virtual void onCommit();
};

class SLuaTextBox : public LLTextBox, public SLuaUICtrl
{
public:
	SLuaTextBox();
	SLuaTextBox(std::string name, int left, int top, int height, int width, std::string default_text);
	void LuaSetMaxWidth(int max_width);
	std::string LuaGetText();
	void LuaSetText(std::string new_text);
	virtual void onCommit();
};

class SLuaUIManager
{
public:
	SLuaFloater* getFloater(std::string name);
};
#endif SLUA_H