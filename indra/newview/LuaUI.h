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
#include "llfloater.h"
#include "lluictrl.h"
#include "llagent.h"
#include "llvovolume.h"
#include "llbutton.h"
#include "lllineeditor.h"
#include "lltextbox.h"
#include "llpanel.h"
#include "llselectmgr.h"

class LuaUICtrl : public LLUICtrl
{
public:
//	LuaUICtrl();
	LuaUICtrl(std::string name);
	LuaUICtrl(std::string name, int left, int top, int height, int width);
	void LuaSetRect(int left, int top, int height, int width);
};

class LuaAgent : public LLAgent
{
public:
};

class LuaObjectManager : public LLVolumeMgr
{
public:
};

class LuaSelectionManager : public LLSelectMgr
{
public:
};

class LuaObject : public LLVOVolume
{
public:
};

class LuaPanel : public LLPanel, public LuaUICtrl
{
public:
//	LuaPanel();
	LuaPanel(std::string name);
	LuaPanel(std::string name, int left, int top, int height, int width);
	LuaPanel(std::string name, int left, int top, int height, int width, bool border);
};

class LuaButton : public LLButton, public LuaUICtrl
{
public:
//	LuaButton();
	LuaButton(std::string name, int left, int top, int height, int width, std::string label);
	void LuaSetImages(std::string unselected_image, std::string selected_image);
	void LuaSetLabel(std::string label);
	void LuaSetLabels(std::string unselected_label, std::string selected_label);
	virtual void onHeldDown(void *usedata);
};

class LuaFloater : public LLFloater, public LuaPanel
{
public:
//	LuaFloater();
	LuaFloater(std::string xml);
	LuaFloater(std::string name, std::string title);
	LuaFloater(std::string name, std::string title, int left, int top, int height, int width);
	void LuaSetTitle(std::string new_title);
	void LuaAddChild(LuaUICtrl* new_child);
	void LuaAddButton(LuaButton* new_button);
	virtual void onFocusLost();
	virtual void onFocusReceived();
	virtual void onClose();
	virtual void onClickClose();
	virtual void onClickMinimize();
};

class LuaLineEditor : public LLLineEditor, public LuaUICtrl
{
public:
//	LuaLineEditor();
	LuaLineEditor(std::string name, int left, int top, int height, int width, std::string default_text = "");
	void LuaSetText(std::string new_text);
	std::string LuaGetText();
	virtual void onCommit();
};

class LuaTextBox : public LLTextBox, public LuaUICtrl
{
public:
//	LuaTextBox();
	LuaTextBox(std::string name, int left, int top, int height, int width, std::string default_text);
	void LuaSetMaxWidth(int max_width);
	std::string LuaGetText();
	void LuaSetText(std::string new_text);
	virtual void onCommit();
};

class LuaUIManager
{
public:
	LuaFloater* getFloater(std::string name);
};
#endif //Lua_H
