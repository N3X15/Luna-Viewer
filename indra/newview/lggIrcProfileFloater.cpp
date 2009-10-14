/* Copyright (c) 2009
*
* Greg Hendrickson (LordGregGreg Back). All rights reserved.
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
*   3. Neither the name Modular Systems Ltd nor the names of its contributors
*      may be used to endorse or promote products derived from this
*      software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS "AS IS"
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

#include "lggIrcProfileFloater.h"

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
#include "lggBeamMaps.h"


#include "llsdserialize.h"
#include "llpanelemerald.h"
#include "lggBeamsColors.h"
#include "llsliderctrl.h"
#include "llfocusmgr.h"
#include "llsd.h"
#include "llimview.h"
#include "lllineeditor.h"


class lggIrcProfileFloater;

class lggIrcProfileFloater : public LLFloater, public LLFloaterSingleton<lggIrcProfileFloater>
{
public:
	lggIrcProfileFloater(const LLSD& seed);
	virtual ~lggIrcProfileFloater();	
	BOOL postBuild(void);

	void setData(LLSD idata);

	static void onClickIM(void* data);
	static void onClickClose(void* data);

protected:
	LLSD myLLSDdata;
};

lggIrcProfileFloater::~lggIrcProfileFloater()
{

}
void lggIrcProfileFloater::setData(LLSD idata)
{
	myLLSDdata=idata;

	
	getChild<LLLineEditor>("EmIRCProfile_WhoNick")->setValue(myLLSDdata["NICK"].asString());
	getChild<LLLineEditor>("EmIRCProfile_WhoUser")->setValue(myLLSDdata["USER"].asString());
	getChild<LLLineEditor>("EmIRCProfile_WhoHost")->setValue(myLLSDdata["HOST"].asString());
	//childSetValue("EmIRCtxt_name",myLLSDdata["REALNAME"].asString());
	//childSetText("EmIRCtxt_channels",myLLSDdata["CHANNELS"].asString());
	getChild<LLLineEditor>("EmIRCProfile_WhoName")->setValue(myLLSDdata["REALNAME"].asString());

	getChild<LLLineEditor>("EmIRCProfile_WhoChanel")->setValue(myLLSDdata["CHANNELS"].asString());
	getChild<LLLineEditor>("EmIRCProfile_WhoServer")->setValue(myLLSDdata["SERVERS"].asString());
	getChild<LLLineEditor>("EmIRCProfile_WhoIdle")->setValue(myLLSDdata["IDLE"].asString());

	setTitle("IRC Profile - "+myLLSDdata["NICK"].asString());
}

lggIrcProfileFloater::lggIrcProfileFloater(const LLSD& seed)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_IRCprofile.xml");
	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}

}

BOOL lggIrcProfileFloater::postBuild(void)
{
	//setCanMinimize(false);
	childSetAction("EmeraldIRC_Profile_IM",onClickIM,this);
	childSetAction("EmeraldIRC_Profile_Close",onClickClose,this);
	
	

// 	childSetValue("EmIRCtxt_nick",myLLSDdata["NICK"]);
// 	childSetValue("EmIRCtxt_user",myLLSDdata["USER"]);
// 	childSetValue("EmIRCtxt_host",myLLSDdata["HOST"]);
// 	childSetValue("EmIRCtxt_name",myLLSDdata["REALNAME"]);
// 	childSetValue("EmIRCtxt_channels",myLLSDdata["CHANNELS"]);
// 	childSetValue("EmIRCtxt_server",myLLSDdata["SERVERS"]);
// 	childSetValue("EmIRCtxt_idle",myLLSDdata["IDLE"]);
	// 
	// 	args["NICK"] = nick;
	// 	args["USER"] = user;
	// 	args["HOST"] = host;
	// 	args["REALNAME"] = realName;
	// 	args["CHANNELS"] = channels;
	// 	args["SERVERS"] = servers;
	// 	args["IDLE"] = idle;
	// 	args["RCHANNEL"]= REALChannel;

	return true;
}
void lggIrcProfileFloater::onClickIM(void* data)
{
	lggIrcProfileFloater* self = (lggIrcProfileFloater*)data;

	LLUUID uid;
	uid.generate(self->myLLSDdata["NICK"].asString()+"lgg"+self->myLLSDdata["RCHANNEL"].asString());
	

	LLUUID computed_session_id=LLIMMgr::computeSessionID(IM_PRIVATE_IRC,uid);
			
	if(!gIMMgr->hasSession(computed_session_id))
	{
		make_ui_sound("UISndNewIncomingIMSession");
		gIMMgr->addSession(
		llformat("%s",self->myLLSDdata["NICK"].asString().c_str()),IM_PRIVATE_IRC,uid);
	}else
	{

		
	}
	
	self->close();
}

void lggIrcProfileFloater::onClickClose(void* data)
{
	lggIrcProfileFloater* self = (lggIrcProfileFloater*)data;
	self->close();
}
void LggIrcProfile::show(LLSD data)
{
	lggIrcProfileFloater* beam_floater = lggIrcProfileFloater::showInstance();
	beam_floater->setData(data);
}

