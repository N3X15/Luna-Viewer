/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "llviewerprecompiledheaders.h"

#include "mfdKeywordFloater.h"

#include "llagentdata.h"
#include "llcommandhandler.h"
#include "llfloater.h"
#include "llsdutil.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llagent.h"
#include "llfilepicker.h"
#include "llpanel.h"
#include "lliconctrl.h"
#include "llbutton.h"
#include "llcolorswatch.h"

#include "llsdserialize.h"
#include "llpanelemerald.h"
#include "llsliderctrl.h"
#include "llfocusmgr.h"
#include <boost/regex.hpp>


class mfdKeywordFloater;

const F32 CONTEXT_CONE_IN_ALPHA = 0.0f;
const F32 CONTEXT_CONE_OUT_ALPHA = 1.f;
const F32 CONTEXT_FADE_TIME = 0.08f;

class mfdKeywordFloater : public LLFloater, public LLFloaterSingleton<mfdKeywordFloater>
{
public:
	mfdKeywordFloater(const LLSD& seed);
	virtual ~mfdKeywordFloater();
	

	BOOL postBuild(void);
	void draw();
	void update();
	
	void setData(void* data);
	
	// UI Handlers
	static void onClickSave(void* data);
	static void onClickCancel(void* data);

protected:
	F32 mContextConeOpacity;
	LLPanelEmerald * empanel;

};

void mfdKeywordFloater::draw()
{
	
	//Try draw rectangle attach beam
	LLRect swatch_rect;
	LLButton* createButton = empanel->getChild<LLButton>("keyword_allert");	
	createButton->localRectToOtherView(createButton->getLocalRect(), &swatch_rect, this);
	LLRect local_rect = getLocalRect();
	if (gFocusMgr.childHasKeyboardFocus(this) && empanel->isInVisibleChain() && mContextConeOpacity > 0.001f)
	{
		gGL.getTexUnit(0)->unbind(LLTexUnit::TT_TEXTURE);
		LLGLEnable(GL_CULL_FACE);
		gGL.begin(LLRender::QUADS);
		{
			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_IN_ALPHA * mContextConeOpacity);
			gGL.vertex2i(swatch_rect.mLeft, swatch_rect.mTop);
			gGL.vertex2i(swatch_rect.mRight, swatch_rect.mTop);
			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_OUT_ALPHA * mContextConeOpacity);
			gGL.vertex2i(local_rect.mRight, local_rect.mTop);
			gGL.vertex2i(local_rect.mLeft, local_rect.mTop);

			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_OUT_ALPHA * mContextConeOpacity);
			gGL.vertex2i(local_rect.mLeft, local_rect.mTop);
			gGL.vertex2i(local_rect.mLeft, local_rect.mBottom);
			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_IN_ALPHA * mContextConeOpacity);
			gGL.vertex2i(swatch_rect.mLeft, swatch_rect.mBottom);
			gGL.vertex2i(swatch_rect.mLeft, swatch_rect.mTop);

			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_OUT_ALPHA * mContextConeOpacity);
			gGL.vertex2i(local_rect.mRight, local_rect.mBottom);
			gGL.vertex2i(local_rect.mRight, local_rect.mTop);
			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_IN_ALPHA * mContextConeOpacity);
			gGL.vertex2i(swatch_rect.mRight, swatch_rect.mTop);
			gGL.vertex2i(swatch_rect.mRight, swatch_rect.mBottom);

			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_OUT_ALPHA * mContextConeOpacity);
			gGL.vertex2i(local_rect.mLeft, local_rect.mBottom);
			gGL.vertex2i(local_rect.mRight, local_rect.mBottom);
			gGL.color4f(0.0f, 0.0f, 0.0f, CONTEXT_CONE_IN_ALPHA * mContextConeOpacity);
			gGL.vertex2i(swatch_rect.mRight, swatch_rect.mBottom);
			gGL.vertex2i(swatch_rect.mLeft, swatch_rect.mBottom);
		}
		gGL.end();
	}

	mContextConeOpacity = lerp(mContextConeOpacity, gSavedSettings.getF32("PickerContextOpacity"), LLCriticalDamp::getInterpolant(CONTEXT_FADE_TIME));


	//Draw Base Stuff
	LLFloater::draw();
}

mfdKeywordFloater::~mfdKeywordFloater()
{
}
mfdKeywordFloater::mfdKeywordFloater(const LLSD& seed):mContextConeOpacity(0.0f)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_alertwindow.xml");
	
	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}

}

BOOL mfdKeywordFloater::postBuild(void)
{
	setCanMinimize(false);
	childSetAction("EmeraldKeywords_save",onClickSave,this);
	childSetAction("EmeraldKeywords_cancel",onClickCancel,this);

	childSetValue("EmeraldKeywords_Alert",gSavedPerAccountSettings.getBOOL("EmeraldKeywordOn"));
	childSetValue("EmeraldKeywords_Entries",gSavedPerAccountSettings.getString("EmeraldKeywords"));
	childSetValue("EmeraldKeywords_IM",gSavedPerAccountSettings.getBOOL("EmeraldKeywordInIM"));
	childSetValue("EmeraldKeywords_GroupChat",gSavedPerAccountSettings.getBOOL("EmeraldKeywordInGroup"));
	childSetValue("EmeraldKeywords_LocalChat",gSavedPerAccountSettings.getBOOL("EmeraldKeywordInChat"));
	childSetValue("EmeraldKeywords_IRC",gSavedPerAccountSettings.getBOOL("EmeraldKeywordInIRC"));
	childSetValue("EmeraldKeywords_Highlight",gSavedPerAccountSettings.getBOOL("EmeraldKeywordChangeColor"));
	//childSetValue("EmeraldKeywords_Color",gSavedPerAccountSettings.getLLSD("EmeraldKeywordColor"));
	childSetValue("EmeraldKeywords_PlaySound",gSavedPerAccountSettings.getBOOL("EmeraldKeywordPlaySound"));
	childSetValue("EmeraldKeywords_SoundUUID",gSavedPerAccountSettings.getString("EmeraldKeywordSound"));

	LLColorSwatchCtrl* colorctrl = getChild<LLColorSwatchCtrl>("EmeraldKeywords_Color");
	colorctrl->set(LLColor4(gSavedPerAccountSettings.getColor4("EmeraldKeywordColor")),TRUE);

	return true;
}
void mfdKeywordFloater::setData(void* data)
{
	empanel = (LLPanelEmerald*)data;
	if(empanel)
	{
		gFloaterView->getParentFloater(empanel)->addDependentFloater(this);
	}
}
void mfdKeywordFloater::update()
{
	
}
void mfdKeywordFloater::onClickSave(void* data)
{
	
	mfdKeywordFloater* self = (mfdKeywordFloater*)data;

	gSavedPerAccountSettings.setBOOL("EmeraldKeywordOn", self->childGetValue("EmeraldKeywords_Alert").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordInIM", self->childGetValue("EmeraldKeywords_IM").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordInGroup", self->childGetValue("EmeraldKeywords_GroupChat").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordInChat", self->childGetValue("EmeraldKeywords_LocalChat").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordInIRC", self->childGetValue("EmeraldKeywords_IRC").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordChangeColor", self->childGetValue("EmeraldKeywords_Highlight").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldKeywordPlaySound", self->childGetValue("EmeraldKeywords_PlaySound").asBoolean());

	gSavedPerAccountSettings.setString("EmeraldKeywords", self->childGetValue("EmeraldKeywords_Entries").asString());
	gSavedPerAccountSettings.setString("EmeraldKeywordSound", self->childGetValue("EmeraldKeywords_SoundUUID").asString());

	
	gSavedPerAccountSettings.setColor4("EmeraldKeywordColor", 
		 ((LLColorSwatchCtrl*)(self->getChild<LLColorSwatchCtrl>("EmeraldKeywords_Color")))->get()
		 );



	self->close();

}

void mfdKeywordFloater::onClickCancel(void* data)
{
	mfdKeywordFloater* self = (mfdKeywordFloater*)data;	
	self->close();	
}
void MfdKeywordFloaterStart::show(BOOL showin, void * data)
{
	if(showin)
	{
		
		mfdKeywordFloater* floater = mfdKeywordFloater::showInstance();
		floater->setData(data);

	}
}
bool containsKeyWord(std::string source)
{
	std::string s = gSavedPerAccountSettings.getString("EmeraldKeywords");
	LLStringUtil::toLower(s);
	LLStringUtil::toLower(source);
	boost::regex re(",");
	boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
	boost::sregex_token_iterator j;

	while(i != j)
	{
		if(source.find( *i++) != std::string::npos)
		{
			if(gSavedPerAccountSettings.getBOOL("EmeraldKeywordPlaySound"))
				LLUI::sAudioCallback(LLUUID(gSavedPerAccountSettings.getString("EmeraldKeywordSound")));

			return true;
		}
	}
	return false;

}

BOOL MfdKeywordFloaterStart::hasKeyword(std::string msg,int source)
{
	if(!gSavedPerAccountSettings.getBOOL("EmeraldKeywordOn"))return FALSE;

// 	if((source ==3) && (gSavedPerAccountSettings.getBOOL("EmeraldKeywordInGroup")))
// 		return containsKeyWord(msg);
// 	if((source ==4) && (gSavedPerAccountSettings.getBOOL("EmeraldKeywordInIRC")))
// 		return containsKeyWord(msg);
	if((source == 1) && (gSavedPerAccountSettings.getBOOL("EmeraldKeywordInChat")))
		return containsKeyWord(msg);
	if((source == 2) && (gSavedPerAccountSettings.getBOOL("EmeraldKeywordInIM")))
		return containsKeyWord(msg);
	return FALSE;
	
}


