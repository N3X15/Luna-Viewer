// <edit>

#include "llviewerprecompiledheaders.h"
#include "floaterexploreanimations.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llscrolllistctrl.h"
#include "llfloateranimpreview.h"
#include "llvoavatar.h"
#include "llagent.h"
#include "llviewercamera.h"
#include "llviewerwindow.h"
#include "lltoolmgr.h"
// <stuff for jelly roll>
#include "llfloatertools.h"
#include "llselectmgr.h"
// </stuff for jelly roll>

std::map< LLUUID, std::list< LLAnimHistoryItem* > > LLFloaterExploreAnimations::animHistory;
LLFloaterExploreAnimations* LLFloaterExploreAnimations::sInstance;


LLAnimHistoryItem::LLAnimHistoryItem(LLUUID assetid)
{
	mAssetID = assetid;
}

LLFloaterExploreAnimations::LLFloaterExploreAnimations(LLUUID avatarid)
:	LLFloater()
{
	mLastMouseX = 0;
	mLastMouseY = 0;

	LLFloaterExploreAnimations::sInstance = this;
	mAvatarID = avatarid;
	mAnimPreview = new LLPreviewAnimation(256, 256);
	mAnimPreview->setZoom(2.0f);
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_explore_animations.xml");
}

void LLFloaterExploreAnimations::close(bool app_quitting)
{
	LLFloater::close(app_quitting);
}

LLFloaterExploreAnimations::~LLFloaterExploreAnimations()
{
	delete mAnimPreview;
	LLFloaterExploreAnimations::sInstance = NULL;
}

BOOL LLFloaterExploreAnimations::postBuild(void)
{
	mTitle = getTitle();
	childSetCommitCallback("anim_list", onSelectAnimation, this);

	childSetAction("Stop Selected",StopSelected,this);
	childSetAction("Revoke Selected",RevokeSelected,this);
	childSetAction("Stop+Revoke Selected",StopRevokeSelected,this);

	LLRect r = getRect();
	mPreviewRect.set(r.getWidth() - 266, r.getHeight() - 25, r.getWidth() - 10, r.getHeight() - 256);
	update();
	return TRUE;
}

void LLFloaterExploreAnimations::update()
{
	LLScrollListCtrl* list = getChild<LLScrollListCtrl>("anim_list");
	LLUUID selection = list->getSelectedValue().asUUID();
	list->clearRows(); // do this differently probably

    std::string fullname;
    if (gCacheName->getFullName(mAvatarID, fullname))
    {
        setTitle(mTitle + ": " + fullname);
    }
    else
    {
        setTitle(mTitle + ": INVALID AVATAR (" + mAvatarID.asString() + ")");
	}


	std::list<LLAnimHistoryItem*> history = animHistory[mAvatarID];
	std::list<LLAnimHistoryItem*>::iterator iter = history.begin();
	std::list<LLAnimHistoryItem*>::iterator end = history.end();
	for( ; iter != end; ++iter)
	{
		LLAnimHistoryItem* item = (*iter);

		if (gSavedSettings.getBOOL("PhoenixFilterDefaultAnims") && isLindenDefaultAnim(item->mAssetID.asString().c_str()))
		{
			continue;
		}

		LLSD element;
		element["id"] = item->mAssetID;

		LLSD& name_column = element["columns"][0];
		name_column["column"] = "name";
		name_column["value"] = item->mAssetID.asString();

		LLSD& info_column = element["columns"][1];
		info_column["column"] = "info";
		if(item->mPlaying)
			info_column["value"] = "Playing";
		else
			info_column["value"] = llformat("%.1f min ago", (LLTimer::getElapsedSeconds() - item->mTimeStopped) / 60.f);

		list->addElement(element, ADD_BOTTOM);
	}

	list->selectByID(selection);
}

void LLFloaterExploreAnimations::draw()
{
	LLFloater::draw();

	LLRect r = getRect();
	mPreviewRect.set(r.getWidth() - 266, r.getHeight() - 25, r.getWidth() - 10, r.getHeight() - 256);

	gGL.color3f(1.f, 1.f, 1.f);

	gGL.getTexUnit(0)->bind(mAnimPreview->getTexture());

	gGL.begin( LLRender::QUADS );
	{
		gGL.texCoord2f(0.f, 1.f);
		gGL.vertex2i(mPreviewRect.mLeft, mPreviewRect.mTop);
		gGL.texCoord2f(0.f, 0.f);
		gGL.vertex2i(mPreviewRect.mLeft, mPreviewRect.mBottom);
		gGL.texCoord2f(1.f, 0.f);
		gGL.vertex2i(mPreviewRect.mRight, mPreviewRect.mBottom);
		gGL.texCoord2f(1.f, 1.f);
		gGL.vertex2i(mPreviewRect.mRight, mPreviewRect.mTop);
	}
	gGL.end();

	gGL.getTexUnit(0)->unbind(LLTexUnit::TT_TEXTURE);

	//LLVOAvatar* avatarp = mAnimPreview->getDummyAvatar();
	//if (!avatarp->areAnimationsPaused())
	//{
	//	mAnimPreview->requestUpdate();
	//}
}





// static
void LLFloaterExploreAnimations::startAnim(LLUUID avatarid, LLUUID assetid)
{
	LLAnimHistoryItem* item = NULL;

	std::list<LLAnimHistoryItem*> history = animHistory[avatarid];
	std::list<LLAnimHistoryItem*>::iterator iter = history.begin();
	std::list<LLAnimHistoryItem*>::iterator end = history.end();
	for( ; iter != end; ++iter)
	{
		if((*iter)->mAssetID == assetid)
		{
			item = (*iter);
			break;
		}
	}
	if(!item)
	{
		item = new LLAnimHistoryItem(assetid);
		item->mAvatarID = avatarid;
		item->mTimeStarted = LLTimer::getElapsedSeconds();
	}
	item->mPlaying = true;
	history.push_back(item);
	animHistory[avatarid] = history; // is this really necessary?
	handleHistoryChange();
}

// static
void LLFloaterExploreAnimations::stopAnim(LLUUID avatarid, LLUUID assetid)
{
	LLAnimHistoryItem* item = NULL;

	std::list<LLAnimHistoryItem*> history = animHistory[avatarid];
	std::list<LLAnimHistoryItem*>::iterator iter = history.begin();
	std::list<LLAnimHistoryItem*>::iterator end = history.end();
	for( ; iter != end; ++iter)
	{
		if((*iter)->mAssetID == assetid)
		{
			item = (*iter);
			break;
		}
	}
	if(!item)
	{
		item = new LLAnimHistoryItem(assetid);
		item->mAvatarID = avatarid;
		item->mTimeStarted = LLTimer::getElapsedSeconds();
		history.push_back(item);
	}
	item->mPlaying = false;
	item->mTimeStopped = LLTimer::getElapsedSeconds();
	handleHistoryChange();
}

class LLAnimHistoryItemCompare
{
public:
	bool operator() (LLAnimHistoryItem* first, LLAnimHistoryItem* second)
	{
		if(first->mPlaying)
		{
			if(second->mPlaying)
			{
				return (first->mTimeStarted > second->mTimeStarted);
			}
			else
			{
				return true;
			}
		}
		else if(second->mPlaying)
		{
			return false;
		}
		else
		{
			return (first->mTimeStopped > second->mTimeStopped);
		}
	}
};

// static
void LLFloaterExploreAnimations::handleHistoryChange()
{
	std::map< LLUUID, std::list< LLAnimHistoryItem* > >::iterator av_iter = animHistory.begin();
	std::map< LLUUID, std::list< LLAnimHistoryItem* > >::iterator av_end = animHistory.end();
	for( ; av_iter != av_end; ++av_iter)
	{
		std::list<LLAnimHistoryItem*> history = (*av_iter).second;

		// Sort it
		LLAnimHistoryItemCompare c;
		history.sort(c);

		// Remove dupes
		history.unique();

		// Trim it
		if(history.size() > 32)
		{
			history.resize(32);
		}

		animHistory[(*av_iter).first] = history;
	}

	// Update floater
	if(LLFloaterExploreAnimations::sInstance)
		LLFloaterExploreAnimations::sInstance->update();
}





// static
void LLFloaterExploreAnimations::onSelectAnimation(LLUICtrl* ctrl, void* user_data)
{
	LLFloaterExploreAnimations* floater = (LLFloaterExploreAnimations*)user_data;
	LLPreviewAnimation* preview = (LLPreviewAnimation*)floater->mAnimPreview;
	LLScrollListCtrl* list = floater->getChild<LLScrollListCtrl>("anim_list");
	LLUUID selection = list->getSelectedValue().asUUID();
	
	preview->getDummyAvatar()->deactivateAllMotions();
	preview->getDummyAvatar()->startMotion(selection, 0.f);
	preview->setZoom(2.0f);
}

// static

//-----------------------------------------------------------------------------
// handleMouseDown()
//-----------------------------------------------------------------------------
BOOL LLFloaterExploreAnimations::handleMouseDown(S32 x, S32 y, MASK mask)
{
	if (mPreviewRect.pointInRect(x, y))
	{
		//llinfos << "lolwut" << llendl;
		bringToFront( x, y );
		gFocusMgr.setMouseCapture(this);
		gViewerWindow->hideCursor();
		mLastMouseX = x;
		mLastMouseY = y;
		return TRUE;
	}

	return LLFloater::handleMouseDown(x, y, mask);
}

//-----------------------------------------------------------------------------
// handleMouseUp()
//-----------------------------------------------------------------------------
BOOL LLFloaterExploreAnimations::handleMouseUp(S32 x, S32 y, MASK mask)
{
	//llinfos << "lolwut pear" << llendl;
	gFocusMgr.setMouseCapture(FALSE);
	gViewerWindow->showCursor();
	return LLFloater::handleMouseUp(x, y, mask);
}

//-----------------------------------------------------------------------------
// handleHover()
//-----------------------------------------------------------------------------
BOOL LLFloaterExploreAnimations::handleHover(S32 x, S32 y, MASK mask)
{
	MASK local_mask = mask & ~MASK_ALT;

	if (mAnimPreview && hasMouseCapture())
	{
		if (local_mask == MASK_PAN)
		{
			// pan here
			mAnimPreview->pan((F32)(x - mLastMouseX) * -0.005f, (F32)(y - mLastMouseY) * -0.005f);
		}
		else if (local_mask == MASK_ORBIT)
		{
			F32 yaw_radians = (F32)(x - mLastMouseX) * -0.01f;
			F32 pitch_radians = (F32)(y - mLastMouseY) * 0.02f;
			
			mAnimPreview->rotate(yaw_radians, pitch_radians);
		}
		else 
		{
			F32 yaw_radians = (F32)(x - mLastMouseX) * -0.01f;
			F32 zoom_amt = (F32)(y - mLastMouseY) * 0.02f;
			
			mAnimPreview->rotate(yaw_radians, 0.f);
			mAnimPreview->zoom(zoom_amt);
		}

		mAnimPreview->requestUpdate();

		LLUI::setCursorPositionLocal(this, mLastMouseX, mLastMouseY);
	}

	if (!mPreviewRect.pointInRect(x, y) || !mAnimPreview)
	{
		return LLFloater::handleHover(x, y, mask);
	}
	else if (local_mask == MASK_ORBIT)
	{
		gViewerWindow->setCursor(UI_CURSOR_TOOLCAMERA);
	}
	else if (local_mask == MASK_PAN)
	{
		gViewerWindow->setCursor(UI_CURSOR_TOOLPAN);
	}
	else
	{
		gViewerWindow->setCursor(UI_CURSOR_TOOLZOOMIN);
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// handleScrollWheel()
//-----------------------------------------------------------------------------
BOOL LLFloaterExploreAnimations::handleScrollWheel(S32 x, S32 y, S32 clicks)
{
	mAnimPreview->zoom((F32)clicks * -0.2f);
	mAnimPreview->requestUpdate();
	return TRUE;
}

//-----------------------------------------------------------------------------
// onMouseCaptureLost()
//-----------------------------------------------------------------------------
void LLFloaterExploreAnimations::onMouseCaptureLost()
{
	gViewerWindow->showCursor();
}
// </edit>

void LLFloaterExploreAnimations::StopSelected(void *userdata )
{
	LLFloaterExploreAnimations *self = (LLFloaterExploreAnimations*)userdata;
	LLDynamicArray<LLUUID> ids;
	std::vector< LLScrollListItem * > items = self->getChild<LLScrollListCtrl>("anim_list")->getAllSelected();

	for( std::vector< LLScrollListItem * >::iterator itr = items.begin(); itr != items.end(); itr++ )
	{
		LLScrollListItem *item = *itr;
		const LLUUID &id = item->getValue().asUUID();
		if( ids.find(id) == LLDynamicArray<LLUUID>::FAIL )
		{
			ids.put(id);
		}
	}
	gAgent.sendAnimationRequests(ids,ANIM_REQUEST_STOP);
}

void LLFloaterExploreAnimations::RevokeSelected(void *userdata )
{
	LLFloaterExploreAnimations *self = (LLFloaterExploreAnimations*)userdata;
	LLDynamicArray<LLUUID> ids;
	std::vector< LLScrollListItem * > items = self->getChild<LLScrollListCtrl>("anim_list")->getAllSelected();

	for( std::vector< LLScrollListItem * >::iterator itr = items.begin(); itr != items.end(); itr++ )
	{
		LLScrollListItem *item = *itr;
		const LLUUID &id = item->getValue().asUUID();
		if( ids.find(id) == LLDynamicArray<LLUUID>::FAIL )
		{
			ids.put(id);
		}
	}
	if( !ids.empty() )
	{
		for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
		{
			LLUUID id = *itr;
			LLMessageSystem* msg = gMessageSystem;
			msg->newMessageFast(_PREHASH_RevokePermissions);
			msg->nextBlockFast(_PREHASH_AgentData);
			msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
			msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
			msg->nextBlockFast(_PREHASH_Data);
			msg->addUUIDFast(_PREHASH_ObjectID, id);
			msg->addU32Fast(_PREHASH_ObjectPermissions, U32_MAX);
			gAgent.sendReliableMessage();
		}
	}
}

void LLFloaterExploreAnimations::StopRevokeSelected(void *userdata )
{
	StopSelected(userdata);
	RevokeSelected(userdata);
}

bool LLFloaterExploreAnimations::isLindenDefaultAnim(std::string anim_id)
{
	if  ((anim_id == "6b61c8e8-4747-0d75-12d7-e49ff207a4ca" ) || //express_afraid 
		(anim_id == "b5b4a67d-0aee-30d2-72cd-77b333e932ef" ) || //aim_r_bazooka 
		(anim_id == "46bb4359-de38-4ed8-6a22-f1f52fe8f506" ) || //aim_l_bow 
		(anim_id == "3147d815-6338-b932-f011-16b56d9ac18b" ) || //aim_r_handgun 
		(anim_id == "ea633413-8006-180a-c3ba-96dd1d756720" ) || //aim_r_rifle 
		(anim_id == "5747a48e-073e-c331-f6f3-7c2149613d3e" ) || //express_anger 
		(anim_id == "fd037134-85d4-f241-72c6-4f42164fedee" ) || //away 
		(anim_id == "c4ca6188-9127-4f31-0158-23c4e2f93304" ) || //backflip 
		(anim_id == "18b3a4b5-b463-bd48-e4b6-71eaac76c515" ) || //express_laugh 
		(anim_id == "db84829b-462c-ee83-1e27-9bbee66bd624" ) || //blowkiss 
		(anim_id == "b906c4ba-703b-1940-32a3-0c7f7d791510" ) || //express_bored 
		(anim_id == "82e99230-c906-1403-4d9c-3889dd98daba" ) || //bow 
		(anim_id == "349a3801-54f9-bf2c-3bd0-1ac89772af01" ) || //brush 
		(anim_id == "efcf670c-2d18-8128-973a-034ebc806b67" ) || //busy 
		(anim_id == "9b0c1c4e-8ac7-7969-1494-28c874c4f668" ) || //clap 
		(anim_id == "9ba1c942-08be-e43a-fb29-16ad440efc50" ) || //courtbow 
		(anim_id == "201f3fdf-cb1f-dbec-201f-7333e328ae7c" ) || //crouch 
		(anim_id == "47f5f6fb-22e5-ae44-f871-73aaaf4a6022" ) || //crouchwalk 
		(anim_id == "92624d3e-1068-f1aa-a5ec-8244585193ed" ) || //express_cry 
		(anim_id == "038fcec9-5ebd-8a8e-0e2e-6e71a0a1ac53" ) || //turn_180 
		(anim_id == "6883a61a-b27b-5914-a61e-dda118a9ee2c" ) || //turnback_180 
		(anim_id == "b68a3d7c-de9e-fc87-eec8-543d787e5b0d" ) || //dance1 
		(anim_id == "928cae18-e31d-76fd-9cc9-2f55160ff818" ) || //dance2 
		(anim_id == "30047778-10ea-1af7-6881-4db7a3a5a114" ) || //dance3 
		(anim_id == "951469f4-c7b2-c818-9dee-ad7eea8c30b7" ) || //dance4 
		(anim_id == "4bd69a1d-1114-a0b4-625f-84e0a5237155" ) || //dance5 
		(anim_id == "cd28b69b-9c95-bb78-3f94-8d605ff1bb12" ) || //dance6 
		(anim_id == "a54d8ee2-28bb-80a9-7f0c-7afbbe24a5d6" ) || //dance7 
		(anim_id == "b0dc417c-1f11-af36-2e80-7e7489fa7cdc" ) || //dance8 
		(anim_id == "57abaae6-1d17-7b1b-5f98-6d11a6411276" ) || //dead 
		(anim_id == "0f86e355-dd31-a61c-fdb0-3a96b9aad05f" ) || //drink 
		(anim_id == "514af488-9051-044a-b3fc-d4dbf76377c6" ) || //express_embarrased 
		(anim_id == "aa2df84d-cf8f-7218-527b-424a52de766e" ) || //express_afraid_emote 
		(anim_id == "1a03b575-9634-b62a-5767-3a679e81f4de" ) || //express_anger_emote 
		(anim_id == "214aa6c1-ba6a-4578-f27c-ce7688f61d0d" ) || //express_bored_emote 
		(anim_id == "d535471b-85bf-3b4d-a542-93bea4f59d33" ) || //express_cry_emote 
		(anim_id == "d4416ff1-09d3-300f-4183-1b68a19b9fc1" ) || //express_disdain 
		(anim_id == "0b8c8211-d78c-33e8-fa28-c51a9594e424" ) || //express_embarrassed_emote 
		(anim_id == "fee3df48-fa3d-1015-1e26-a205810e3001" ) || //express_frown 
		(anim_id == "1e8d90cc-a84e-e135-884c-7c82c8b03a14" ) || //express_kiss 
		(anim_id == "62570842-0950-96f8-341c-809e65110823" ) || //express_laugh_emote 
		(anim_id == "d63bc1f9-fc81-9625-a0c6-007176d82eb7" ) || //express_open_mouth 
		(anim_id == "f76cda94-41d4-a229-2872-e0296e58afe1" ) || //express_repulsed_emote 
		(anim_id == "eb6ebfb2-a4b3-a19c-d388-4dd5c03823f7" ) || //express_sad_emote 
		(anim_id == "a351b1bc-cc94-aac2-7bea-a7e6ebad15ef" ) || //express_shrug_emote 
		(anim_id == "b7c7c833-e3d3-c4e3-9fc0-131237446312" ) || //express_smile 
		(anim_id == "728646d9-cc79-08b2-32d6-937f0a835c24" ) || //express_surprise_emote 
		(anim_id == "835965c6-7f2f-bda2-5deb-2478737f91bf" ) || //express_tongue_out 
		(anim_id == "b92ec1a5-e7ce-a76b-2b05-bcdb9311417e" ) || //express_toothsmile 
		(anim_id == "da020525-4d94-59d6-23d7-81fdebf33148" ) || //express_wink_emote 
		(anim_id == "9c05e5c7-6f07-6ca4-ed5a-b230390c3950" ) || //express_worry_emote 
		(anim_id == "666307d9-a860-572d-6fd4-c3ab8865c094" ) || //falldown 
		(anim_id == "f5fc7433-043d-e819-8298-f519a119b688" ) || //female_walk 
		(anim_id == "c1bc7f36-3ba0-d844-f93c-93be945d644f" ) || //angry_fingerwag 
		(anim_id == "7db00ccd-f380-f3ee-439d-61968ec69c8a" ) || //fist_pump 
		(anim_id == "aec4610c-757f-bc4e-c092-c6e9caf18daf" ) || //fly 
		(anim_id == "2b5a38b2-5e00-3a97-a495-4c826bc443e6" ) || //flyslow 
		(anim_id == "9b29cd61-c45b-5689-ded2-91756b8d76a9" ) || //hello 
		(anim_id == "ef62d355-c815-4816-2474-b1acc21094a6" ) || //hold_r_bazooka 
		(anim_id == "8b102617-bcba-037b-86c1-b76219f90c88" ) || //hold_l_bow 
		(anim_id == "efdc1727-8b8a-c800-4077-975fc27ee2f2" ) || //hold_r_handgun 
		(anim_id == "3d94bad0-c55b-7dcc-8763-033c59405d33" ) || //hold_r_rifle 
		(anim_id == "7570c7b5-1f22-56dd-56ef-a9168241bbb6" ) || //hold_throw_r 
		(anim_id == "4ae8016b-31b9-03bb-c401-b1ea941db41d" ) || //hover 
		(anim_id == "20f063ea-8306-2562-0b07-5c853b37b31e" ) || //hover_down 
		(anim_id == "62c5de58-cb33-5743-3d07-9e4cd4352864" ) || //hover_up 
		(anim_id == "5ea3991f-c293-392e-6860-91dfa01278a3" ) || //impatient 
		(anim_id == "2305bd75-1ca9-b03b-1faa-b176b8a8c49e" ) || //jump 
		(anim_id == "709ea28e-1573-c023-8bf8-520c8bc637fa" ) || //jumpforjoy 
		(anim_id == "19999406-3a3a-d58c-a2ac-d72e555dcf51" ) || //kissmybutt 
		(anim_id == "7a17b059-12b2-41b1-570a-186368b6aa6f" ) || //land 
		(anim_id == "ca5b3f14-3194-7a2b-c894-aa699b718d1f" ) || //laugh_short 
		(anim_id == "f4f00d6e-b9fe-9292-f4cb-0ae06ea58d57" ) || //soft_land 
		(anim_id == "08464f78-3a8e-2944-cba5-0c94aff3af29" ) || //motorcycle_sit 
		(anim_id == "315c3a41-a5f3-0ba4-27da-f893f769e69b" ) || //musclebeach 
		(anim_id == "5a977ed9-7f72-44e9-4c4c-6e913df8ae74" ) || //no_head 
		(anim_id == "d83fa0e5-97ed-7eb2-e798-7bd006215cb4" ) || //no_unhappy 
		(anim_id == "f061723d-0a18-754f-66ee-29a44795a32f" ) || //nyanya 
		(anim_id == "eefc79be-daae-a239-8c04-890f5d23654a" ) || //punch_onetwo 
		(anim_id == "b312b10e-65ab-a0a4-8b3c-1326ea8e3ed9" ) || //peace 
		(anim_id == "17c024cc-eef2-f6a0-3527-9869876d7752" ) || //point_me 
		(anim_id == "ec952cca-61ef-aa3b-2789-4d1344f016de" ) || //point_you 
		(anim_id == "7a4e87fe-de39-6fcb-6223-024b00893244" ) || //prejump 
		(anim_id == "f3300ad9-3462-1d07-2044-0fef80062da0" ) || //punch_l 
		(anim_id == "c8e42d32-7310-6906-c903-cab5d4a34656" ) || //punch_r 
		(anim_id == "36f81a92-f076-5893-dc4b-7c3795e487cf" ) || //express_repulsed 
		(anim_id == "49aea43b-5ac3-8a44-b595-96100af0beda" ) || //kick_roundhouse_r 
		(anim_id == "35db4f7e-28c2-6679-cea9-3ee108f7fc7f" ) || //rps_countdown 
		(anim_id == "0836b67f-7f7b-f37b-c00a-460dc1521f5a" ) || //rps_paper 
		(anim_id == "42dd95d5-0bc6-6392-f650-777304946c0f" ) || //rps_rock 
		(anim_id == "16803a9f-5140-e042-4d7b-d28ba247c325" ) || //rps_scissors 
		(anim_id == "05ddbff8-aaa9-92a1-2b74-8fe77a29b445" ) || //run 
		(anim_id == "0eb702e2-cc5a-9a88-56a5-661a55c0676a" ) || //express_sad 
		(anim_id == "cd7668a6-7011-d7e2-ead8-fc69eff1a104" ) || //salute 
		(anim_id == "e04d450d-fdb5-0432-fd68-818aaf5935f8" ) || //shoot_l_bow 
		(anim_id == "6bd01860-4ebd-127a-bb3d-d1427e8e0c42" ) || //shout 
		(anim_id == "70ea714f-3a97-d742-1b01-590a8fcd1db5" ) || //express_shrug 
		(anim_id == "1a5fe8ac-a804-8a5d-7cbd-56bd83184568" ) || //sit 
		(anim_id == "b1709c8d-ecd3-54a1-4f28-d55ac0840782" ) || //sit_female 
		(anim_id == "1c7600d6-661f-b87b-efe2-d7421eb93c86" ) || //sit_ground 
		(anim_id == "1a2bd58e-87ff-0df8-0b4c-53e047b0bb6e" ) || //sit_ground_constrained 
		(anim_id == "245f3c54-f1c0-bf2e-811f-46d8eeb386e7" ) || //sit_generic 
		(anim_id == "a8dee56f-2eae-9e7a-05a2-6fb92b97e21e" ) || //sit_to_stand 
		(anim_id == "f2bed5f9-9d44-39af-b0cd-257b2a17fe40" ) || //sleep 
		(anim_id == "d2f2ee58-8ad1-06c9-d8d3-3827ba31567a" ) || //smoke_idle 
		(anim_id == "6802d553-49da-0778-9f85-1599a2266526" ) || //smoke_inhale 
		(anim_id == "0a9fb970-8b44-9114-d3a9-bf69cfe804d6" ) || //smoke_throw_down 
		(anim_id == "eae8905b-271a-99e2-4c0e-31106afd100c" ) || //snapshot 
		(anim_id == "2408fe9e-df1d-1d7d-f4ff-1384fa7b350f" ) || //stand 
		(anim_id == "3da1d753-028a-5446-24f3-9c9b856d9422" ) || //standup 
		(anim_id == "15468e00-3400-bb66-cecc-646d7c14458e" ) || //stand_1 
		(anim_id == "370f3a20-6ca6-9971-848c-9a01bc42ae3c" ) || //stand_2 
		(anim_id == "42b46214-4b44-79ae-deb8-0df61424ff4b" ) || //stand_3 
		(anim_id == "f22fed8b-a5ed-2c93-64d5-bdd8b93c889f" ) || //stand_4 
		(anim_id == "80700431-74ec-a008-14f8-77575e73693f" ) || //stretch 
		(anim_id == "1cb562b0-ba21-2202-efb3-30f82cdf9595" ) || //stride 
		(anim_id == "41426836-7437-7e89-025d-0aa4d10f1d69" ) || //surf 
		(anim_id == "313b9881-4302-73c0-c7d0-0e7a36b6c224" ) || //express_surprise 
		(anim_id == "85428680-6bf9-3e64-b489-6f81087c24bd" ) || //sword_strike_r 
		(anim_id == "5c682a95-6da4-a463-0bf6-0f5b7be129d1" ) || //talk 
		(anim_id == "11000694-3f41-adc2-606b-eee1d66f3724" ) || //angry_tantrum 
		(anim_id == "aa134404-7dac-7aca-2cba-435f9db875ca" ) || //throw_r 
		(anim_id == "83ff59fe-2346-f236-9009-4e3608af64c1" ) || //tryon_shirt 
		(anim_id == "56e0ba0d-4a9f-7f27-6117-32f2ebbf6135" ) || //turnleft 
		(anim_id == "2d6daa51-3192-6794-8e2e-a15f8338ec30" ) || //turnright 
		(anim_id == "c541c47f-e0c0-058b-ad1a-d6ae3a4584d9" ) || //type 
		(anim_id == "6ed24bd8-91aa-4b12-ccc7-c97c857ab4e0" ) || //walk 
		(anim_id == "7693f268-06c7-ea71-fa21-2b30d6533f8f" ) || //whisper 
		(anim_id == "b1ed7982-c68e-a982-7561-52a88a5298c0" ) || //whistle 
		(anim_id == "869ecdad-a44b-671e-3266-56aef2e3ac2e" ) || //express_wink 
		(anim_id == "c0c4030f-c02b-49de-24ba-2331f43fe41c" ) || //wink_hollywood 
		(anim_id == "9f496bd2-589a-709f-16cc-69bf7df1d36c" ) || //express_worry 
		(anim_id == "15dd911d-be82-2856-26db-27659b142875" ) || //yes_head 
		(anim_id == "b8c8b2a3-9008-1771-3bfc-90924955ab2d" ) || //yes_happy 
		(anim_id == "42ecd00b-9947-a97c-400a-bbc9174c7aeb" )) //yoga_float 
	{
		return true;
	}
	else
	{
		return false;
	}
}
