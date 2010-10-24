#include "llviewerprecompiledheaders.h"

#include "floatermediaplayer.h"

#include "lltexturectrl.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llfloaterurlentry_mp.h"
#include "llfilepicker.h"



F64 FloaterMediaPlayer::media_length;
viewer_media_t FloaterMediaPlayer::sMPMediaImpl;
FloaterMediaPlayer* FloaterMediaPlayer::sInstance = NULL;
std::string FloaterMediaPlayer::mp_url;

FloaterMediaPlayer::FloaterMediaPlayer() :  LLFloater(std::string("PhoenixMediaPlayer"))
{
}

FloaterMediaPlayer::~FloaterMediaPlayer()
{
}

BOOL FloaterMediaPlayer::handleKeyHere(KEY key, MASK mask)
{
	if (( KEY_DELETE == key ) && (MASK_NONE == mask))
	{
 		sInstance->mMPPlayList->deleteSelectedItems();
		return TRUE;
	}
	else if(( 'A' == key ) && (MASK_CONTROL == mask))
	{
 		sInstance->mMPPlayList->selectAll();
		return TRUE;
	}
	return LLPanel::handleKeyHere(key, mask);
}

void FloaterMediaPlayer::showInstance()
{
	if (sInstance)
	{
		if(!sInstance->getVisible())
		{
			sInstance->open();
		}
	}
	else
	{
		sInstance = new FloaterMediaPlayer();
		LLUICtrlFactory::getInstance()->buildFloater(sInstance, "floater_media_player.xml");
	}
}
								
void FloaterMediaPlayer::draw()
{
	LLFloater::draw();
}

void FloaterMediaPlayer::onOpen()
{
	sInstance->setVisible(TRUE);
}

void FloaterMediaPlayer::onClose(bool app_quitting)
{
	sInstance->setVisible(FALSE);
	if( app_quitting )
	{
		if(sMPMediaImpl)
		{
			sMPMediaImpl->stop();
			sMPMediaImpl->destroyMediaSource();
			sMPMediaImpl = NULL;
		}
		destroy();
	}
}

BOOL FloaterMediaPlayer::postBuild()
{
	mMPMediaImage = getChild<LLTextureCtrl>("mp_media_image");
	if (mMPMediaImage)
	{
		mMPMediaImage->setImageAssetID(LLUUID("8b5fec65-8d8d-9dc5-cda8-8fdf2716e361"));
	}
	childSetCommitCallback("mp_seek",onMPSeek,this);
	childSetAction("mp_prev", onClickMPPrev,this);
	childSetAction("mp_play", onClickMPPlay,this);
	childSetAction("mp_pause", onClickMPPause,this);
	childSetAction("mp_next", onClickMPNext,this);
	childSetAction("mp_add_file", onClickMPAddFile,this);
	childSetAction("mp_add_URL", onClickMPAddURL,this);
	childSetAction("mp_rem", onClickMPRem,this);
	mMPPlayList = getChild<LLScrollListCtrl>("mp_playlist");
	mMPPlayList->setDoubleClickCallback(onDoubleClick);
	return TRUE;
}

void FloaterMediaPlayer::onMPSeek(LLUICtrl* ctrl,void *userdata)
{
	if(sMPMediaImpl)
	{
		F32 seek_pos = sInstance->childGetValue("mp_seek").asReal();
		seek_pos = (seek_pos/100) * media_length;
		sMPMediaImpl->seek(seek_pos);
	}
}

void FloaterMediaPlayer::onClickMPPrev( void* userdata )
{
	sInstance->mMPPlayList->selectPrevItem(FALSE);
	if(sMPMediaImpl)
	{
		mp_url = sInstance->mMPPlayList->getSelectedValue().asString();
		sMPMediaImpl->navigateTo(mp_url, "video/*");
		sMPMediaImpl->play();
	}
}

void FloaterMediaPlayer::onClickMPPlay( void* userdata )
{
	if(!sMPMediaImpl)
	{
		mp_url = sInstance->mMPPlayList->getSelectedValue().asString();
		// There is no media impl, make a new one
		sMPMediaImpl = LLViewerMedia::newMediaImpl(mp_url, LLUUID("8b5fec65-8d8d-9dc5-cda8-8fdf2716e361"),
			599, 359, 0,
			0, "video/*");
		sMPMediaImpl->play();
	}
	else
	{
		//There is one, re-use it and resume play
		sMPMediaImpl->start();
	}
	sInstance->childSetVisible("mp_play",false);
	sInstance->childSetVisible("mp_pause",true);
}

void FloaterMediaPlayer::onClickMPPause( void* userdata )
{
	sMPMediaImpl->pause();
	sInstance->childSetVisible("mp_pause",false);
	sInstance->childSetVisible("mp_play",true);
}

void FloaterMediaPlayer::onClickMPNext( void* userdata )
{
	sInstance->mMPPlayList->selectNextItem(FALSE);
	if(sMPMediaImpl)
	{
		mp_url = sInstance->mMPPlayList->getSelectedValue().asString();
		sMPMediaImpl->navigateTo(mp_url, "video/*");
		sMPMediaImpl->play();
	}
}

void FloaterMediaPlayer::onClickMPAddFile( void* userdata )
{
	LLFilePicker& picker = LLFilePicker::instance();
	if ( !picker.getMultipleOpenFiles(LLFilePicker::FFLOAD_MEDIA) ) 
	{
		return;
	}
	std::string filename = picker.getFirstFile();	
	while( !filename.empty() )
	{
		filename = "file://"+filename;
		sInstance->mMPPlayList->addSimpleElement(filename);
		filename = picker.getNextFile();
	}
}

void FloaterMediaPlayer::onClickMPAddURL( void* userdata )
{
	FloaterMediaPlayer *self = (FloaterMediaPlayer *)userdata;
	self->mMPURLEntryFloater = LLFloaterMPURLEntry::show( self->getHandle() );
	LLFloater* parent_floater = gFloaterView->getParentFloater(self);
	if (parent_floater)
	{
		parent_floater->addDependentFloater(self->mMPURLEntryFloater.get());
	}
}

void FloaterMediaPlayer::addMediaURL(const std::string& media_url)
{
	sInstance->mMPPlayList->addSimpleElement(media_url);
}

void FloaterMediaPlayer::onClickMPRem( void* userdata )
{
	sInstance->mMPPlayList->deleteSelectedItems();
}

void FloaterMediaPlayer::onDoubleClick( void* userdata )
{
	if(!sMPMediaImpl)
	{
		onClickMPPlay(NULL);
	}
	else
	{
		mp_url = sInstance->mMPPlayList->getSelectedValue().asString();
		sMPMediaImpl->navigateTo(mp_url, "video/*");
		sMPMediaImpl->play();
	}
}