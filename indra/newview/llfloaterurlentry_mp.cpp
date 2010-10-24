/**
 * @file llfloaterurlentry.cpp
 * @brief LLFloaterURLEntry class implementation
 *
 * $LicenseInfo:firstyear=2007&license=viewergpl$
 * 
 * Copyright (c) 2007-2009, Linden Research, Inc.
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

#include "llfloaterurlentry_mp.h"

#include "floatermediaplayer.h"

// project includes
#include "llcombobox.h"
#include "llurlhistory.h"
#include "lluictrlfactory.h"
#include "llwindow.h"
#include "llviewerwindow.h"

static LLFloaterMPURLEntry* sInstance = NULL;


//-----------------------------------------------------------------------------
// LLFloaterURLEntry()
//-----------------------------------------------------------------------------
LLFloaterMPURLEntry::LLFloaterMPURLEntry(LLHandle<LLFloater> parent)
	:
	LLFloater(),
	mFloaterMediaPlayerHandle(parent)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_url_entry.xml");

	mMediaURLEdit = getChild<LLComboBox>("media_entry");

	// Cancel button
	childSetAction("cancel_btn", onBtnCancel, this);

	// Cancel button
	childSetAction("clear_btn", onBtnClear, this);

	// clear media list button
	LLSD parcel_history = LLURLHistory::getURLHistory("Media_Player");
	bool enable_clear_button = parcel_history.size() > 0 ? true : false;
	childSetEnabled( "clear_btn", enable_clear_button );

	// OK button
	childSetAction("ok_btn", onBtnOK, this);

	setDefaultBtn("ok_btn");
	buildURLHistory();

	sInstance = this;
}

//-----------------------------------------------------------------------------
// ~LLFloaterURLEntry()
//-----------------------------------------------------------------------------
LLFloaterMPURLEntry::~LLFloaterMPURLEntry()
{
	sInstance = NULL;
}

void LLFloaterMPURLEntry::buildURLHistory()
{
	LLCtrlListInterface* url_list = childGetListInterface("media_entry");
	if (url_list)
	{
		url_list->operateOnAll(LLCtrlListInterface::OP_DELETE);
	}

	// Get all of the entries in the "parcel" collection
	LLSD parcel_history = LLURLHistory::getURLHistory("Media_Player");

	LLSD::array_iterator iter_history =
		parcel_history.beginArray();
	LLSD::array_iterator end_history =
		parcel_history.endArray();
	for(; iter_history != end_history; ++iter_history)
	{
		url_list->addSimpleElement((*iter_history).asString());
	}
}

// static
LLHandle<LLFloater> LLFloaterMPURLEntry::show(LLHandle<LLFloater> parent)
{
	if (sInstance)
	{
		sInstance->open();
	}
	else
	{
		sInstance = new LLFloaterMPURLEntry(parent);
	}
	return sInstance->getHandle();
}

bool LLFloaterMPURLEntry::addURLToCombobox(const std::string& media_url)
{
	if(! mMediaURLEdit->setSimple( media_url ) && ! media_url.empty())
	{
		mMediaURLEdit->add( media_url );
		mMediaURLEdit->setSimple( media_url );
		return true;
	}

	// URL was not added for whatever reason (either it was empty or already existed)
	return false;
}

// static
//-----------------------------------------------------------------------------
// onBtnOK()
//-----------------------------------------------------------------------------
void LLFloaterMPURLEntry::onBtnOK( void* userdata )
{
	LLFloaterMPURLEntry *self =(LLFloaterMPURLEntry *)userdata;

	std::string media_url	= self->mMediaURLEdit->getValue().asString();
	self->mMediaURLEdit->remove(media_url);
	LLURLHistory::removeURL("Media_Player", media_url);
	if(self->addURLToCombobox(media_url))
	{
		// Add this url to the parcel collection
		LLURLHistory::addURL("Media_Player", media_url);
	}

	// leading whitespace causes problems with the MIME-type detection so strip it
	LLStringUtil::trim( media_url );
	FloaterMediaPlayer* media_player = (FloaterMediaPlayer*)self->mFloaterMediaPlayerHandle.get();
	if (media_player)
	{
		// status is ignored for now -- error = "none/none"
		media_player->addMediaURL(self->mMediaURLEdit->getValue().asString());
	}
}

// static
//-----------------------------------------------------------------------------
// onBtnCancel()
//-----------------------------------------------------------------------------
void LLFloaterMPURLEntry::onBtnCancel( void* userdata )
{
	LLFloaterMPURLEntry *self =(LLFloaterMPURLEntry *)userdata;
	self->close();
}

// static
//-----------------------------------------------------------------------------
// onBtnClear()
//-----------------------------------------------------------------------------
void LLFloaterMPURLEntry::onBtnClear( void* userdata )
{
	LLNotifications::instance().add( "ConfirmClearMediaUrlList", LLSD(), LLSD(), 
									boost::bind(&LLFloaterMPURLEntry::callback_clear_url_list, (LLFloaterMPURLEntry*)userdata, _1, _2) );
}

bool LLFloaterMPURLEntry::callback_clear_url_list(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);
	if ( option == 0 ) // YES
	{
		// clear saved list
		LLCtrlListInterface* url_list = childGetListInterface("media_entry");
		if ( url_list )
		{
			url_list->operateOnAll( LLCtrlListInterface::OP_DELETE );
		}

		// clear current contents of combo box
		mMediaURLEdit->clear();

		// clear stored version of list
		LLURLHistory::clear("Media_Player");

		// cleared the list so disable Clear button
		childSetEnabled( "clear_btn", false );
	}
	return false;
}
