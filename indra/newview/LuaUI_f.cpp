#include "llviewerprecompiledheaders.h"
#include "LuaUI_f.h"

#include "llagent.h"
#include "llbutton.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llconsole.h"
#include "llfloaterchatterbox.h"
#include "llfloatermute.h"
#include "llkeyboard.h"
//#include "lllineeditor.h"
#include "llmutelist.h"
//#include "llresizehandle.h"
#include "llchatbar.h"
#include "llstatusbar.h"
#include "llviewertexteditor.h"
#include "llviewergesture.h"			// for triggering gestures
#include "llviewermessage.h"
#include "llviewerwindow.h"
#include "llviewercontrol.h"
#include "lluictrlfactory.h"

LuaUI::LuaUI(void)
{

}

LuaUI::~LuaUI(void)
{
}


LuaFloater::LuaFloater(const char* name)
:	LLFloater(name)
{
}

LuaFloater::~LuaFloater(void)
{
	close();
}

LuaPanel::LuaPanel(const char* name)
: LLPanel(name)
{
}

LuaPanel::~LuaPanel()
{
}

LuaTextEditor::LuaTextEditor(const char* name)
: LLTextEditor(name,LLRect::null,255,std::string ())
{
}

LuaTextEditor::~LuaTextEditor()
{
}
void TestLuaFloater()
{
	LuaFloater *lf=new LuaFloater("LuaTest");
	LuaTextEditor *te=new LuaTextEditor("txtLuaTest");
	te->appendText("wot",false,true);
	//te->userSetShape(new LLRect(3,3));
	lf->initFloater("LuaTestFloater",TRUE,400,300,TRUE,TRUE,TRUE);
	lf->addChild(te);
	lf->setVisible(TRUE);
}