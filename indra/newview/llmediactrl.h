/**
 * @file llmediactrl.h
 * @brief Web browser UI control
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

#ifndef LL_LLMediaCtrl_H
#define LL_LLMediaCtrl_H

#include "llviewermedia.h"

#include "lluictrl.h"
#include "llframetimer.h"
#include "lldynamictexture.h"

class LLViewBorder;
class LLWebBrowserTexture;
class LLUICtrlFactory;

////////////////////////////////////////////////////////////////////////////////
//
class LLMediaCtrl :
	public LLUICtrl,
	public LLViewerMediaObserver,
	public LLViewerMediaEventEmitter
{
	public:
		LLMediaCtrl( const std::string& name, const LLRect& rect );
		virtual ~LLMediaCtrl();

		void setBorderVisible( BOOL border_visible );

		// For the tutorial window, we don't want to take focus on clicks,
		// as the examples include how to move around with the arrow
		// keys.  Thus we keep focus on the app by setting this false.
		// Defaults to true.
		void setTakeFocusOnClick( bool take_focus );

		virtual LLXMLNodePtr getXML(bool save_children = true) const;
		static LLView* fromXML(LLXMLNodePtr node, LLView *parent, LLUICtrlFactory *factory);

		// handle mouse related methods
		virtual BOOL handleHover( S32 x, S32 y, MASK mask );
		virtual BOOL handleMouseUp( S32 x, S32 y, MASK mask );
		virtual BOOL handleMouseDown( S32 x, S32 y, MASK mask );
		virtual BOOL handleDoubleClick( S32 x, S32 y, MASK mask );
		virtual BOOL handleScrollWheel( S32 x, S32 y, S32 clicks );

		// navigation
		void navigateTo( std::string url_in, std::string mime_type = "");
		void navigateBack();
		void navigateHome();
		void navigateForward();	
		void navigateToLocalPage( const std::string& subdir, const std::string& filename_in );
		bool canNavigateBack();
		bool canNavigateForward();
		void setOpenInExternalBrowser( bool valIn );
		void setOpenInInternalBrowser( bool valIn );
		std::string getCurrentNavUrl();

		// By default, we do not handle "secondlife:///app/" SLURLs, because
		// those can cause teleports, open windows, etc.  We cannot be sure
		// that each "click" is actually due to a user action, versus 
		// Javascript or some other mechanism.  However, we need the search
		// floater and login page to handle these URLs.  Those are safe
		// because we control the page content.  See DEV-9530.  JC.
		void setTrusted( bool valIn );

		void setHomePageUrl( const std::string urlIn );
		std::string getHomePageUrl();

		// set/clear URL to visit when a 404 page is reached
		void set404RedirectUrl( std::string redirect_url );
		void clr404RedirectUrl();

		// accessor/mutator for flag that indicates if frequent updates to texture happen
		bool getFrequentUpdates() { return mFrequentUpdates; };
		void setFrequentUpdates( bool frequentUpdatesIn ) {  mFrequentUpdates = frequentUpdatesIn; };

		void setIgnoreUIScale(bool ignore) { mIgnoreUIScale = ignore; }
		bool getIgnoreUIScale() { return mIgnoreUIScale; }

		void setAlwaysRefresh(bool refresh) { mAlwaysRefresh = refresh; }
		bool getAlwaysRefresh() { return mAlwaysRefresh; }
		
		void setForceUpdate(bool force_update) { mForceUpdate = force_update; }
		bool getForceUpdate() { return mForceUpdate; }

		LLPluginClassMedia* getMediaPlugin();

		bool setCaretColor( unsigned int red, unsigned int green, unsigned int blue );


		// over-rides
		virtual BOOL handleKeyHere( KEY key, MASK mask);
		virtual void handleVisibilityChange ( BOOL new_visibility );
		virtual BOOL handleUnicodeCharHere(llwchar uni_char);
		virtual void reshape( S32 width, S32 height, BOOL called_from_parent = TRUE);
		virtual void draw();
		virtual void onVisibilityChange ( BOOL curVisibilityIn );

		// focus overrides
		void onFocusLost();
		void onFocusReceived();
		
		// Incoming media event dispatcher
		virtual void handleMediaEvent(LLPluginClassMedia* self, EMediaEvent event);

		// handlers for individual events (could be done inside the switch in handleMediaEvent, they're just individual functions for clarity)
		void onClickLinkHref( LLPluginClassMedia* self );
		void onClickLinkNoFollow( LLPluginClassMedia* self );
		
	protected:
		void convertInputCoords(S32& x, S32& y);

	private:
		static bool onClickLinkExternalTarget( const LLSD&, const LLSD& );

		const S32 mTextureDepthBytes;
		LLWebBrowserTexture* mWebBrowserImage;
		LLViewBorder* mBorder;
		bool mFrequentUpdates;
		bool mForceUpdate;
		bool mOpenLinksInExternalBrowser;
		bool mOpenLinksInInternalBrowser;
		bool mTrusted;
		std::string mHomePageUrl;
		std::string mExternalUrl;
		std::string mCurrentNavUrl;
		bool mIgnoreUIScale;
		bool mAlwaysRefresh;
		viewer_media_t mMediaSource;
		bool mTakeFocusOnClick;
		ECursorType mLastSetCursor;
		bool mStretchToFill;
		bool mMaintainAspectRatio;
		bool mHideLoading;
};

////////////////////////////////////////////////////////////////////////////////
//
class LLWebBrowserTexture : public LLDynamicTexture
{
LOG_CLASS(LLWebBrowserTexture);
	public:
		LLWebBrowserTexture( S32 width, S32 height, LLMediaCtrl* browserCtrl, viewer_media_t media_source );
		virtual ~LLWebBrowserTexture();

		virtual BOOL needsRender();
		virtual void preRender( BOOL clear_depth = TRUE ) {};
		virtual void postRender( BOOL success ) {};
		virtual BOOL render();
		
		bool adjustSize();
		S32 getMediaWidth();
		S32 getMediaHeight();
		bool getNeedsUpdate();
		void setNeedsUpdate();
		bool getTextureCoordsOpenGL();

		void resize( S32 new_width, S32 new_height );
		bool updateBrowserTexture();

	protected:
		S32 mMediaWidth;
		S32 mMediaHeight;
		bool mNeedsUpdate;
		bool mNeedsResize;
		bool mTextureCoordsOpenGL;
		LLFrameTimer mElapsedTime;
		LLMediaCtrl* mWebBrowserCtrl;
		viewer_media_t mMediaSource;
};

#endif // LL_LLMediaCtrl_H
