#ifndef FLOATER_MEDIA_PLAYER_H
#define FLOATER_MEDIA_PLAYER_H

#include "llviewerprecompiledheaders.h"
#include "llfloater.h"
#include "llscrolllistctrl.h"
#include "llviewermedia.h"

class LLTextureCtrl;

class FloaterMediaPlayer : public LLFloater
{
private:
	FloaterMediaPlayer();

public:
	~FloaterMediaPlayer();

	virtual BOOL	handleKeyHere(KEY key, MASK mask);

	/*virtual*/ void onClose(bool app_quitting);
	/*virtual*/ void onOpen();
	/*virtual*/ BOOL postBuild();
	/*virtual*/ void draw();

	static void toggle(void*); //Toggles interface visibility
	static void showInstance();
	static void addMediaURL(const std::string& media_url);

	static FloaterMediaPlayer* getInstance(){ return sInstance; }

private:
	static FloaterMediaPlayer* sInstance;
	LLScrollListCtrl*	mMPPlayList;
	static void onMPSeek(LLUICtrl* ctrl,void *userdata);
	static void onClickMPPrev( void* userdata );
	static void onClickMPPlay( void* userdata );
	static void onClickMPPause( void* userdata );
	static void onClickMPNext( void* userdata );
	static void onClickMPAddFile( void* userdata );
	static void onClickMPAddURL( void* userdata );
	static void onClickMPRem( void* userdata );
	static void onDoubleClick(void *userdata);
	static F64 media_length;
	static viewer_media_t sMPMediaImpl;
	static std::string mp_url;
	LLTextureCtrl*	mMPMediaImage;
	LLHandle<LLFloater>	mMPURLEntryFloater;
};
#endif // FLOATER_MEDIA_PLAYER_H
