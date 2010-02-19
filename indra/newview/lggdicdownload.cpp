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

#include "lggdicdownload.h"

#include "llagentdata.h"
#include "llcommandhandler.h"
#include "llfloater.h"
#include "lluictrlfactory.h"
#include "llagent.h"
#include "llpanel.h"
#include "llbutton.h"
#include "llcolorswatch.h"
#include "llcombobox.h"
#include "llview.h"
#include "llpanelemerald.h"

class lggDicDownloadFloater;

class lggDicDownloadFloater : public LLFloater, public LLFloaterSingleton<lggDicDownloadFloater>
{
public:
	lggDicDownloadFloater(const LLSD& seed);
	virtual ~lggDicDownloadFloater();
	BOOL postBuild(void);
	void setData(std::vector<std::string> shortNames, std::vector<std::string> longNames, void * data);
	void downloadDic(std::string name);
	static void onClickDownload(void* data);
	std::vector<std::string> sNames;
	std::vector<std::string> lNames;
	LLPanelEmerald * empanel;
};
lggDicDownloadFloater::~lggDicDownloadFloater()
{
}
lggDicDownloadFloater::lggDicDownloadFloater(const LLSD& seed)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_dictionaries.xml");

	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}

}

BOOL lggDicDownloadFloater::postBuild(void)
{
	childSetAction("Emerald_dic_download",onClickDownload,this);
	return true;
}
void lggDicDownloadFloater::setData(std::vector<std::string> shortNames, std::vector<std::string> longNames, void * data)
{
	sNames=shortNames;
	lNames=longNames;
	empanel = (LLPanelEmerald*)data;

	LLComboBox* comboBox = getChild<LLComboBox>("Emerald_combo_dics");
	if(comboBox != NULL) 
	{
		comboBox->removeall();
		comboBox->add("");
		for(int i = 0; i < (int)lNames.size(); i++)
		{
			comboBox->add(lNames[i]);
		}
		comboBox->setCurrentByIndex(0);
	}
}
void lggDicDownloadFloater::onClickDownload(void* data)
{
	lggDicDownloadFloater* self = (lggDicDownloadFloater*)data;
	if(self)
	{
		//std::string selection = self->childGetValue("Emerald_combo_dics").asString();
		LLComboBox* comboBox = self->getChild<LLComboBox>("Emerald_combo_dics");
		if(comboBox != NULL) 
		{
			int index = comboBox->getCurrentIndex();
			if(index!=0)
			{
				index--;
				std::string newDict(self->sNames[index]);
				self->downloadDic(newDict+".aff");
				self->downloadDic(newDict+".dic");
			}
		}
	}
	if(self->empanel)self->empanel->refresh();
	self->close();

}
void lggDicDownloadFloater::downloadDic(std::string name)
{
	std::string dicpath(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "dictionaries", 
			name.c_str()));
	//LLButton* butt = getChild<LLButton>("Emerald_dic_download");
	//if(butt)butt->setLabel(LLStringExplicit("Downloading... Please Wait"));

	LLHTTPClient::downloadFile(std::string("http://www.modularsystems.sl/app/dics/"+name),
			dicpath);
}
void LggDicDownload::show(BOOL showin, std::vector<std::string> shortNames, std::vector<std::string> longNames, void * data)
{
	if(showin)
	{
		lggDicDownloadFloater* dic_floater = lggDicDownloadFloater::showInstance();
		dic_floater->setData(shortNames,longNames,data);
	}
}