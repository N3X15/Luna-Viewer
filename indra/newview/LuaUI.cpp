/* Code goes here! */

#include "llviewerprecompiledheaders.h"
#include "LuaUI.h"
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "lluictrlfactory.h"
#include "llbutton.h"
#include "llcheckboxctrl.h"
#include "llinventoryview.h"
#include "lllineeditor.h"
#include "lltexturectrl.h"
#include "llscrolllistctrl.h"
#include "llimview.h"
#include "lltextbox.h"
#include "lldispatcher.h"
#include "llviewertexteditor.h"

/*LuaFloater::LuaFloater()
{
LL_WARNS("LUA") << "Default LuaFloater constructor, would produce a floater that could not be accessed again." << LL_ENDL;
}*/

LuaFloater::LuaFloater(std::string xml):
	LuaPanel("")
{
	LLUICtrlFactory::getInstance()->buildFloater(static_cast<LLFloater*>(this), xml);
}

LuaFloater::LuaFloater(std::string name, std::string title):
	LuaPanel(name)
{
	//LL_INFOS("LUA") << "We created a floater!" << LL_ENDL;
	static_cast<LLFloater*>(this)->setName(name);
	this->initFloater(title, TRUE, 0, 0, FALSE, TRUE, TRUE);
	this->setTitle(title);
	this->setVisibleAndFrontmost();
	this->center();
}

LuaFloater::LuaFloater(std::string name, std::string title, int left, int top, int right, int bottom):
	LuaPanel(name,left,top,right,bottom)
{
	//LL_INFOS("LUA") << "We created a floater!" << LL_ENDL;
	static_cast<LLFloater*>(this)->setName(name);
	this->initFloater(title, TRUE, 0, 0, FALSE, TRUE, TRUE);
	static_cast<LLFloater*>(this)->setRect(LLRect(left,top,right,bottom));
	this->setTitle(title);
	this->setVisibleAndFrontmost();
}

void LuaFloater::LuaSetTitle(std::string new_title)
{
	this->setTitle(new_title);
}

void LuaFloater::LuaAddChild(LuaUICtrl* new_child)
{
	static_cast<LLFloater*>(this)->addChild(new_child);
}

void LuaFloater::LuaAddButton(LuaButton* new_button)
{
	static_cast<LLFloater*>(this)->addChild(static_cast<LLButton*>(new_button));
}

void LuaFloater::onClose()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClose";
	LUA_CALL0(temp_name);
}

void LuaFloater::onClickClose()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClickClose";
	LUA_CALL0(temp_name);
}


void LuaFloater::onFocusReceived()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onFocusReceived";
	LUA_CALL0(temp_name);
}

void LuaFloater::onFocusLost()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onFocusLost";
	LUA_CALL0(temp_name);
}

void LuaFloater::onClickMinimize()
{
	std::string temp_name = static_cast<LLFloater*>(this)->getName() + "_onClickMinimize";
	LUA_CALL0(temp_name);
}
/*
LuaPanel::LuaPanel()
{}*/

LuaPanel::LuaPanel(std::string name):
	LuaUICtrl(	name, 0,0,1,1)
{}

LuaPanel::LuaPanel(std::string name, int left, int top, int right, int bottom):
	LuaUICtrl(	name, left, top, right, bottom)
{}

LuaPanel::LuaPanel(std::string name, int left, int top, int right, int bottom, bool border):
	LuaUICtrl(	name, left, top, right, bottom)
{}

/*
LuaUICtrl::LuaUICtrl()
{}*/

LuaUICtrl::LuaUICtrl(std::string name)
{}

LuaUICtrl::LuaUICtrl(std::string name, int left, int top, int right, int bottom)
{}

//This name is misleading, I know.
void LuaUICtrl::LuaSetRect(int left, int top, int right, int bottom)
{
	this->setRect(LLRect(left, top, right, bottom));
}

 LuaButton::LuaButton(std::string name, int left, int top, int right, int bottom, std::string label)
:	LLButton(	name, LLRect(left,top,right,bottom), "", "", LLStringUtil::null, NULL, NULL),
	LuaUICtrl(	name, left, top, right, bottom)
{
	static_cast<LLButton*>(this)->setLabel(label);
}

void LuaButton::LuaSetImages(std::string unselected_image, std::string selected_image)
{}

void LuaButton::LuaSetLabel(std::string label)
{}

void LuaButton::LuaSetLabels(std::string unselected_label, std::string selected_label)
{}

void LuaButton::onHeldDown(void *userdata)
{
	std::string temp_name = static_cast<LLButton*>(this)->getName() + "_onHeldDown";
	LUA_CALL0(temp_name);
}

//LuaLineEditor::LuaLineEditor()
//{}
LuaLineEditor::LuaLineEditor(std::string name, int left, int top, int right, int bottom, std::string default_text)
	:LLLineEditor(name,LLRect(left,top,right,bottom),default_text),
	LuaUICtrl(	name, left, top, right, bottom)
{
}

void LuaLineEditor::LuaSetText(std::string new_text)
{
	this->setText(new_text);
}

std::string LuaLineEditor::LuaGetText()
{
	return this->getText();
}

void LuaLineEditor::onCommit()
{
	std::string temp_name = static_cast<LLLineEditor*>(this)->getName() + "_onCommit";
	LUA_CALL(temp_name) << LuaGetText() << LUA_END;
}

LuaTextBox::LuaTextBox(std::string name, int left, int top, int right, int bottom, std::string default_text)
	:LLTextBox(name,LLRect(left,top,right,bottom),default_text),
	LuaUICtrl(	name, left, top, right, bottom)
{}

void LuaTextBox::LuaSetMaxWidth(int max_width)
{
	this->setWrappedText(this->getText(), max_width);
}

void LuaTextBox::LuaSetText(std::string new_text)
{
	this->setText(new_text);
}

std::string LuaTextBox::LuaGetText()
{
	return this->getText();
}

void LuaTextBox::onCommit()
{
	std::string temp_name = static_cast<LLTextBox*>(this)->getName() + "_onCommit";
	LUA_CALL(temp_name) << getText() << LUA_END;
}

LuaFloater* LuaUIManager::getFloater(std::string name)
{
	return static_cast<LuaFloater*>(gViewerWindow->getRootView()->getChild<LLFloater>(name));
}
