/** 
 * @file llviewernetwork.cpp
 * @author James Cook, Richard Nelson
 * @brief Networking constants and globals for viewer.
 *
 * $LicenseInfo:firstyear=2006&license=viewergpl$
 * 
 * Copyright (c) 2006-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llviewernetwork.h"
#include "llviewercontrol.h"
#include "llstartup.h"

 #include "hippogridmanager.h"

struct LLGridData
{
	const char* mLabel;
	const char* mName;
	const char* mGridURI;
	const char* mHelperURI;
};
/*
static LLGridData gGridInfo[GRID_INFO_COUNT] = 
{
	{ "None", "", "", ""},
	{ "Aditi", 
	  "util.aditi.lindenlab.com", 
	  "https://login.aditi.lindenlab.com/cgi-bin/login.cgi",
	  "http://aditi-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Agni", 
	  "util.agni.lindenlab.com", 
	  "https://login.agni.lindenlab.com/cgi-bin/login.cgi",
	  "https://secondlife.com/helpers/" },
	{ "Aruna",
	  "util.aruna.lindenlab.com",
	  "https://login.aruna.lindenlab.com/cgi-bin/login.cgi",
	  "http://aruna-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Bharati",
	  "util.bharati.lindenlab.com",
	  "https://login.bharati.lindenlab.com/cgi-bin/login.cgi",
	  "http://bharati-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Chandra",
	  "util.chandra.lindenlab.com",
	  "https://login.chandra.lindenlab.com/cgi-bin/login.cgi",
	  "http://chandra-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Damballah",
	  "util.damballah.lindenlab.com",
	  "https://login.damballah.lindenlab.com/cgi-bin/login.cgi",
	  "http://damballah-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Danu",
	  "util.danu.lindenlab.com",
	  "https://login.danu.lindenlab.com/cgi-bin/login.cgi",
	  "http://danu-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Durga",
	  "util.durga.lindenlab.com",
	  "https://login.durga.lindenlab.com/cgi-bin/login.cgi",
	  "http://durga-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Ganga",
	  "util.ganga.lindenlab.com",
	  "https://login.ganga.lindenlab.com/cgi-bin/login.cgi",
	  "http://ganga-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Mitra",
	  "util.mitra.lindenlab.com",
	  "https://login.mitra.lindenlab.com/cgi-bin/login.cgi",
	  "http://mitra-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Mohini",
	  "util.mohini.lindenlab.com",
	  "https://login.mohini.lindenlab.com/cgi-bin/login.cgi",
	  "http://mohini-secondlife.webdev.lindenlab.com/helpers/" },
  	{ "Nandi",
	  "util.nandi.lindenlab.com",
	  "https://login.nandi.lindenlab.com/cgi-bin/login.cgi",
	  "http://nandi-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Parvati",
	  "util.parvati.lindenlab.com",
	  "https://login.parvati.lindenlab.com/cgi-bin/login.cgi",
	  "http://parvati-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Radha",
	  "util.radha.lindenlab.com",
	  "https://login.radha.lindenlab.com/cgi-bin/login.cgi",
	  "http://radha-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Ravi",
	  "util.ravi.lindenlab.com",
	  "https://login.ravi.lindenlab.com/cgi-bin/login.cgi",
	  "http://ravi-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Siva", 
	  "util.siva.lindenlab.com",
	  "https://login.siva.lindenlab.com/cgi-bin/login.cgi",
	  "http://siva-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Shakti",
	  "util.shakti.lindenlab.com",
	  "https://login.shakti.lindenlab.com/cgi-bin/login.cgi",
	  "http://shakti-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Skanda",
	  "util.skanda.lindenlab.com",
	  "https://login.skanda.lindenlab.com/cgi-bin/login.cgi",
	  "http://skanda-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Soma",
	  "util.soma.lindenlab.com",
	  "https://login.soma.lindenlab.com/cgi-bin/login.cgi",
	  "http://soma-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Uma",
	  "util.uma.lindenlab.com",
	  "https://login.uma.lindenlab.com/cgi-bin/login.cgi",
	  "http://uma-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Vaak",
	  "util.vaak.lindenlab.com",
	  "https://login.vaak.lindenlab.com/cgi-bin/login.cgi",
	  "http://vaak-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Yami",
	  "util.yami.lindenlab.com",
	  "https://login.yami.lindenlab.com/cgi-bin/login.cgi",
	  "http://yami-secondlife.webdev.lindenlab.com/helpers/" },
	{ "Local", 
	  "localhost", 
	  "https://login.dmz.lindenlab.com/cgi-bin/login.cgi",
	  "" },
	{ "Other", 
	  "", 
	  "https://login.dmz.lindenlab.com/cgi-bin/login.cgi",
	  "" }
};
*/
unsigned char gMACAddress[MAC_ADDRESS_BYTES];		/* Flawfinder: ignore */


void LLViewerLogin::getLoginURIs(std::vector<std::string>& uris) const
{
	// return the login uri set on the command line.
	LLControlVariable* c = gSavedSettings.getControl("CmdLineLoginURI");
	if(c)
		{
		LLSD v = c->getValue();
			if(v.isArray())
			{
				for(LLSD::array_const_iterator itr = v.beginArray();
					itr != v.endArray(); ++itr)
				{
					std::string uri = itr->asString();
					if(!uri.empty())
					{
					uris.push_back(uri);
					}
				}
			}
		else
			{
				std::string uri = v.asString();
				if(!uri.empty())
				{
				uris.push_back(uri);
			}
		}
}

	// If there was no command line uri...
	if(uris.empty())
{
		uris.push_back(gHippoGridManager->getConnectedGrid()->getLoginUri());
		/*
		// If its a known grid choice, get the uri from the table,
		// else try the grid name.
		if(mGridChoice > GRID_INFO_NONE && mGridChoice < GRID_INFO_OTHER)
	{
			uris.push_back(gGridInfo[mGridChoice].mLoginURI);
	}
	else
	{
			uris.push_back(mGridName);
		} */
	}
}

const std::string &LLViewerLogin::getGridLabel() const
	{
	return gHippoGridManager->getConnectedGrid()->getGridNick();
	}

const std::string &LLViewerLogin::getLoginPage() const
	{
	return gHippoGridManager->getConnectedGrid()->getLoginPage();
}

const std::string &LLViewerLogin::getHelperURI() const
{
	return gHippoGridManager->getConnectedGrid()->getHelperUri();
}

bool LLViewerLogin::isOpenSimulator()
{
	return gHippoGridManager->getConnectedGrid()->isOpenSimulator();
}

bool LLViewerLogin::isSecondLife()
{
	return gHippoGridManager->getConnectedGrid()->isSecondLife();
}

bool LLViewerLogin::isInProductionGrid()
{
			return true;
}
