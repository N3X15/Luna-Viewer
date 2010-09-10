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

struct LLGridData
{
	const char* mLabel;
	const char* mName;
	const char* mGridURI;
	const char* mHelperURI;
};

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

unsigned char gMACAddress[MAC_ADDRESS_BYTES];		/* Flawfinder: ignore */

LLViewerLogin::LLViewerLogin() :
	mGridChoice(DEFAULT_GRID_CHOICE),
	mCurrentURI(0),
	mNameEditted(false)
{
	parseCommandLineURIs();
}

void LLViewerLogin::setGridChoice(EGridInfo grid)
{	
	if(grid < 0 || grid >= GRID_INFO_COUNT)
	{
		llerrs << "Invalid grid index specified." << llendl;
	}

	mGridChoice = grid;
	if(GRID_INFO_LOCAL == mGridChoice)
	{
		mGridName = LOOPBACK_ADDRESS_STRING;
	}
	else if (GRID_INFO_OTHER == mGridChoice)
	{
		// *FIX:Mani - could this possibly be valid?
		mGridName = "other";
	}
	else
	{
		mGridName = gGridInfo[mGridChoice].mLabel;
		setGridURI(getStaticGridURI(grid));
		setHelperURI(getStaticGridHelperURI(grid));
		setLoginPageURI(std::string());
	}

	gSavedSettings.setS32("ServerChoice", mGridChoice);
	gSavedSettings.setString("CustomServer", "");
}

void LLViewerLogin::setGridChoice(const std::string& grid_name)
{
	// Set the grid choice based on a string.
	// The string can be:
	// - a grid label from the gGridInfo table 
	// - an ip address
    if(!grid_name.empty())
    {
        // find the grid choice from the user setting.
        int grid_index = GRID_INFO_NONE; 
        for(;grid_index < GRID_INFO_OTHER; ++grid_index)
        {
            if(0 == LLStringUtil::compareInsensitive(gGridInfo[grid_index].mLabel, grid_name))
            {
				// Founding a matching label in the list...
				setGridChoice((EGridInfo)grid_index);
				break;
            }
        }

        if(GRID_INFO_OTHER == grid_index)
        {
            // *FIX:MEP Can and should we validate that this is an IP address?
            mGridChoice = GRID_INFO_OTHER;
            mGridName = grid_name;
			gSavedSettings.setS32("ServerChoice", mGridChoice);
			gSavedSettings.setString("CustomServer", mGridName);
        }
    }
}

void LLViewerLogin::setGridURI(const std::string& uri)
{
	std::vector<std::string> uri_list;
	uri_list.push_back(uri);
	setGridURIs(uri_list);
}

void LLViewerLogin::setGridURIs(const std::vector<std::string>& urilist)
{
	mGridURIs.clear();
	mGridURIs.insert(mGridURIs.begin(), urilist.begin(), urilist.end());
	mCurrentURI = 0;
}

EGridInfo LLViewerLogin::getGridChoice() const
{
	return mGridChoice;
}

std::string LLViewerLogin::getGridLabel()
{
	if(mGridChoice == GRID_INFO_NONE)
	{
		return "None";
	}
	else if(mGridChoice < GRID_INFO_OTHER)
	{
		return gGridInfo[mGridChoice].mLabel;
	}
	else if (!mGridName.empty())
	{
		return mGridName;
	}
	else
	{
		return LLURI(getCurrentGridURI()).hostName();
	}
}

std::string LLViewerLogin::getKnownGridLabel(EGridInfo grid_index) const
{
	if(grid_index > GRID_INFO_NONE && grid_index < GRID_INFO_OTHER)
	{
		return gGridInfo[grid_index].mLabel;
	}
	return gGridInfo[GRID_INFO_NONE].mLabel;
}

const std::vector<std::string>& LLViewerLogin::getCommandLineURIs()
{
	return mCommandLineURIs;
}

const std::vector<std::string>& LLViewerLogin::getGridURIs()
{
	return mGridURIs;
}

void LLViewerLogin::parseCommandLineURIs()
{
	// return the login uri set on the command line.
	LLControlVariable* c = gSavedSettings.getControl("CmdLineLoginURI");
	if(c)
	{
		LLSD v = c->getValue();
		if (!v.isUndefined())
		{
			bool foundRealURI = false;
			if(v.isArray())
			{
				for(LLSD::array_const_iterator itr = v.beginArray();
					itr != v.endArray(); ++itr)
				{
					std::string uri = itr->asString();
					if(!uri.empty())
					{
						foundRealURI = true;
						mCommandLineURIs.push_back(uri);
					}
				}
			}
			else if (v.isString())
			{
				std::string uri = v.asString();
				if(!uri.empty())
				{
					foundRealURI = true;
					mCommandLineURIs.push_back(uri);
				}
			}

			if (foundRealURI)
			{
				mGridChoice = GRID_INFO_OTHER;
				mCurrentURI = 0;
				mGridName = getGridLabel();
			}
		}
	}

	setLoginPageURI(gSavedSettings.getString("LoginPage"));
	setHelperURI(gSavedSettings.getString("CmdLineHelperURI"));
}

const std::string LLViewerLogin::getCurrentGridURI()
{
	return (((int)(mGridURIs.size()) > mCurrentURI) ? mGridURIs[mCurrentURI] : std::string());
}

bool LLViewerLogin::tryNextURI()
{
	if (++mCurrentURI < (int)(mGridURIs.size()))
	{
		return true;
	}
	else
	{
		mCurrentURI = 0;
		return false;
	}
}

const std::string LLViewerLogin::getStaticGridHelperURI(const EGridInfo grid) const
{
	std::string helper_uri;
	// grab URI from selected grid
	if(grid > GRID_INFO_NONE && grid < GRID_INFO_OTHER)
	{
		helper_uri = gGridInfo[grid].mHelperURI;
	}

	if (helper_uri.empty())
	{
		// what do we do with unnamed/miscellaneous grids?
		// for now, operations that rely on the helper URI (currency/land purchasing) will fail
	}
	return helper_uri;
}

const std::string LLViewerLogin::getHelperURI() const
{
	return mHelperURI;
}

void LLViewerLogin::setHelperURI(const std::string& uri)
{
	mHelperURI = uri;
}

const std::string LLViewerLogin::getLoginPageURI() const
{
	return mLoginPageURI;
}

void LLViewerLogin::setLoginPageURI(const std::string& uri)
{
	mLoginPageURI = uri;
}

bool LLViewerLogin::isInProductionGrid()
{
	// *NOTE:Mani This used to compare GRID_INFO_AGNI to gGridChoice,
	// but it seems that loginURI trumps that.
	std::vector<std::string> uris = getGridURIs();
	if (uris.size() > 0)
	{
		if((getCurrentGridURI().find("agni") != std::string::npos))
		{
			return true;
		}
	}

	return false;
}

const std::string LLViewerLogin::getStaticGridURI(const EGridInfo grid) const
{
	// If its a known grid choice, get the uri from the table,
	// else try the grid name.
	if(mGridChoice > GRID_INFO_NONE && mGridChoice < GRID_INFO_OTHER)
	{
		return gGridInfo[mGridChoice].mGridURI;
	}
	else
	{
		return mGridName;
	}
}
