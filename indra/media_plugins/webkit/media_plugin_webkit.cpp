/** 
 * @file media_plugin_webkit.cpp
 * @brief Webkit plugin for LLMedia API plugin system
 *
 * $LicenseInfo:firstyear=2008&license=viewergpl$
 * 
 * Copyright (c) 2008-2009, Linden Research, Inc.
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

#include "llqtwebkit.h"

#include "linden_common.h"
#include "indra_constants.h" // for indra keyboard codes

#include "llgl.h"

#include "llplugininstance.h"
#include "llpluginmessage.h"
#include "llpluginmessageclasses.h"
#include "media_plugin_base.h"

#if LL_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#include <stdlib.h>
#endif

#if LL_WINDOWS
	// NOTE - This captures the module handle of the dll. This is used below
	// to get the path to this dll for webkit initialization.
	// I don't know how/if this can be done with apr...
	namespace {	HMODULE gModuleHandle;};
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
	{
		gModuleHandle = (HMODULE) hinstDLL;
		return TRUE;
	}
#endif

////////////////////////////////////////////////////////////////////////////////
//
class MediaPluginWebKit : 
		public MediaPluginBase,
		public LLEmbeddedBrowserWindowObserver
{
public:
	MediaPluginWebKit(LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data);
	~MediaPluginWebKit();

	/*virtual*/ void receiveMessage(const char *message_string);

private:

	std::string mProfileDir;

	enum
	{
		INIT_STATE_UNINITIALIZED,		// Browser instance hasn't been set up yet
		INIT_STATE_NAVIGATING,			// Browser instance has been set up and initial navigate to about:blank has been issued
		INIT_STATE_NAVIGATE_COMPLETE,	// initial navigate to about:blank has completed
		INIT_STATE_WAIT_REDRAW,			// First real navigate begin has been received, waiting for page changed event to start handling redraws
		INIT_STATE_RUNNING				// All initialization gymnastics are complete.
	};
	int mBrowserWindowId;
	int mInitState;
	std::string mInitialNavigateURL;
	bool mNeedsUpdate;

	bool	mCanCut;
	bool	mCanCopy;
	bool	mCanPaste;
	int mLastMouseX;
	int mLastMouseY;
	bool mFirstFocus;
	
	void setInitState(int state)
	{
//		std::cerr << "changing init state to " << state << std::endl;
		mInitState = state;
	}
	
	////////////////////////////////////////////////////////////////////////////////
	//
	void update(int milliseconds)
	{
		LLQtWebKit::getInstance()->pump( milliseconds );
		
		checkEditState();
		
		if(mInitState == INIT_STATE_NAVIGATE_COMPLETE)
		{
			if(!mInitialNavigateURL.empty())
			{
				// We already have the initial navigate URL -- kick off the navigate.
				LLQtWebKit::getInstance()->navigateTo( mBrowserWindowId, mInitialNavigateURL );
				mInitialNavigateURL.clear();
			}
		}
		
		if ( (mInitState == INIT_STATE_RUNNING) && mNeedsUpdate )
		{
			const unsigned char* browser_pixels = LLQtWebKit::getInstance()->grabBrowserWindow( mBrowserWindowId );

			unsigned int buffer_size = LLQtWebKit::getInstance()->getBrowserRowSpan( mBrowserWindowId ) * LLQtWebKit::getInstance()->getBrowserHeight( mBrowserWindowId );
			
//			std::cerr << "webkit plugin: updating" << std::endl;
			
			// TODO: should get rid of this memcpy if possible
			if ( mPixels && browser_pixels )
			{
//				std::cerr << "    memcopy of " << buffer_size << " bytes" << std::endl;
				memcpy( mPixels, browser_pixels, buffer_size );
			}

			if ( mWidth > 0 && mHeight > 0 )
			{
//				std::cerr << "Setting dirty, " << mWidth << " x " << mHeight << std::endl;
				setDirty( 0, 0, mWidth, mHeight );
			}

			mNeedsUpdate = false;
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	//
	bool initBrowser()
	{
		// already initialized
		if ( mInitState > INIT_STATE_UNINITIALIZED )
			return true;

		// not enough information to initialize the browser yet.
		if ( mWidth < 0 || mHeight < 0 || mDepth < 0 || 
				mTextureWidth < 0 || mTextureHeight < 0 )
		{
			return false;
		};

		// set up directories
		char cwd[ FILENAME_MAX ];	// I *think* this is defined on all platforms we use
		if (NULL == getcwd( cwd, FILENAME_MAX - 1 ))
		{
			llwarns << "Couldn't get cwd - probably too long - failing to init." << llendl;
			return false;
		}
		std::string application_dir = std::string( cwd );

#if LL_WINDOWS
		// NOTE - On windows, at least, the component path is the
		// location of this dll's image file. 
		std::string component_dir;
		char dll_path[_MAX_PATH];
		DWORD len = GetModuleFileNameA(gModuleHandle, (LPCH)&dll_path, _MAX_PATH);
		while(len && dll_path[ len ] != ('\\') )
		{
			len--;
		}
		if(len >= 0)
		{
			dll_path[len] = 0;
			component_dir = dll_path;
		}
		else
		{
			// NOTE - This case should be a rare exception. 
			// GetModuleFileNameA should always give you a full path.
			component_dir = application_dir;
		}
#else
		std::string component_dir = application_dir;
#endif

		// window handle - needed on Windows and must be app window.
#if LL_WINDOWS
		char window_title[ MAX_PATH ];
		GetConsoleTitleA( window_title, MAX_PATH );
		void* native_window_handle = (void*)FindWindowA( NULL, window_title );
#else
		void* native_window_handle = 0;
#endif

		// main browser initialization
		bool result = LLQtWebKit::getInstance()->init( application_dir, component_dir, mProfileDir, native_window_handle );
		if ( result )
		{
			// create single browser window
			mBrowserWindowId = LLQtWebKit::getInstance()->createBrowserWindow( mWidth, mHeight );

#if LL_WINDOWS
			// Enable plugins
			LLQtWebKit::getInstance()->enablePlugins(true);
#elif LL_DARWIN
			// Enable plugins
			LLQtWebKit::getInstance()->enablePlugins(true);
#elif LL_LINUX
			// Enable plugins
			LLQtWebKit::getInstance()->enablePlugins(true);
#endif
			// Enable cookies
			LLQtWebKit::getInstance()->enableCookies( true );

			// tell LLQtWebKit about the size of the browser window
			LLQtWebKit::getInstance()->setSize( mBrowserWindowId, mWidth, mHeight );

			// observer events that LLQtWebKit emits
			LLQtWebKit::getInstance()->addObserver( mBrowserWindowId, this );

			// append details to agent string
			LLQtWebKit::getInstance()->setBrowserAgentId( "LLPluginMedia Web Browser" );

			// don't flip bitmap
			LLQtWebKit::getInstance()->flipWindow( mBrowserWindowId, true );
			
			// set background color to be black - mostly for initial login page
			LLQtWebKit::getInstance()->setBackgroundColor( mBrowserWindowId, 0x00, 0x00, 0x00 );

			// Set state _before_ starting the navigate, since onNavigateBegin might get called before this call returns.
			setInitState(INIT_STATE_NAVIGATING);

			// Don't do this here -- it causes the dreaded "white flash" when loading a browser instance.
			// FIXME: Re-added this because navigating to a "page" initializes things correctly - especially
			// for the HTTP AUTH dialog issues (DEV-41731). Will fix at a later date.
			LLQtWebKit::getInstance()->navigateTo( mBrowserWindowId, "about:blank" );

			return true;
		};

		return false;
	};

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onCursorChanged(const EventType& event)
	{
		LLQtWebKit::ECursor llqt_cursor = (LLQtWebKit::ECursor)event.getIntValue();
		std::string name;

		switch(llqt_cursor)
		{
			case LLQtWebKit::C_ARROW:
				name = "arrow";
			break;
			case LLQtWebKit::C_IBEAM:
				name = "ibeam";
			break;
			case LLQtWebKit::C_SPLITV:
				name = "splitv";
			break;
			case LLQtWebKit::C_SPLITH:
				name = "splith";
			break;
			case LLQtWebKit::C_POINTINGHAND:
				name = "hand";
			break;
			
			default:
				llwarns << "Unknown cursor ID: " << (int)llqt_cursor << llendl;
			break;
		}
		
		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "cursor_changed");
		message.setValue("name", name);
		sendMessage(message);
	}

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onPageChanged( const EventType& event )
	{
		if(mInitState == INIT_STATE_WAIT_REDRAW)
		{
			setInitState(INIT_STATE_RUNNING);
		}
		
		// flag that an update is required
		mNeedsUpdate = true;
	};

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onNavigateBegin(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "navigate_begin");
			message.setValue("uri", event.getEventUri());
			sendMessage(message);
		
			setStatus(STATUS_LOADING);
		}

		if(mInitState == INIT_STATE_NAVIGATE_COMPLETE)
		{
			setInitState(INIT_STATE_WAIT_REDRAW);
		}
		
	}

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onNavigateComplete(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "navigate_complete");
			message.setValue("uri", event.getEventUri());
			message.setValueS32("result_code", event.getIntValue());
			message.setValue("result_string", event.getStringValue());
			message.setValueBoolean("history_back_available", LLQtWebKit::getInstance()->userActionIsEnabled( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_BACK));
			message.setValueBoolean("history_forward_available", LLQtWebKit::getInstance()->userActionIsEnabled( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_FORWARD));
			sendMessage(message);
			
			setStatus(STATUS_LOADED);
		}
		else if(mInitState == INIT_STATE_NAVIGATING)
		{
			setInitState(INIT_STATE_NAVIGATE_COMPLETE);
		}

	}

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onUpdateProgress(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "progress");
			message.setValueS32("percent", event.getIntValue());
			sendMessage(message);
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onStatusTextChange(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "status_text");
			message.setValue("status", event.getStringValue());
			sendMessage(message);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onTitleChange(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "name_text");
			message.setValue("name", event.getStringValue());
			sendMessage(message);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onLocationChange(const EventType& event)
	{
		if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "location_changed");
			message.setValue("uri", event.getEventUri());
			sendMessage(message);
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onClickLinkHref(const EventType& event)
	{
		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "click_href");
		message.setValue("uri", event.getStringValue());
		message.setValue("target", event.getStringValue2());
		sendMessage(message);
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// virtual
	void onClickLinkNoFollow(const EventType& event)
	{
		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER, "click_nofollow");
		message.setValue("uri", event.getStringValue());
		sendMessage(message);
	}
	
	LLQtWebKit::EKeyboardModifier decodeModifiers(std::string &modifiers)
	{
		int result = 0;
		
		if(modifiers.find("shift") != std::string::npos)
			result |= LLQtWebKit::KM_MODIFIER_SHIFT;

		if(modifiers.find("alt") != std::string::npos)
			result |= LLQtWebKit::KM_MODIFIER_ALT;
		
		if(modifiers.find("control") != std::string::npos)
			result |= LLQtWebKit::KM_MODIFIER_CONTROL;
		
		if(modifiers.find("meta") != std::string::npos)
			result |= LLQtWebKit::KM_MODIFIER_META;
		
		return (LLQtWebKit::EKeyboardModifier)result;
	}
	

	////////////////////////////////////////////////////////////////////////////////
	//
	void keyEvent(LLQtWebKit::EKeyEvent key_event, int key, LLQtWebKit::EKeyboardModifier modifiers)
	{
		int llqt_key;
		
		// The incoming values for 'key' will be the ones from indra_constants.h
		// the outgoing values are the ones from llqtwebkit.h
		
		switch((KEY)key)
		{
			// This is the list that the llqtwebkit implementation actually maps into Qt keys.
//			case KEY_XXX:			llqt_key = LL_DOM_VK_CANCEL;			break;
//			case KEY_XXX:			llqt_key = LL_DOM_VK_HELP;			break;
			case KEY_BACKSPACE:		llqt_key = LL_DOM_VK_BACK_SPACE;		break;
			case KEY_TAB:			llqt_key = LL_DOM_VK_TAB;			break;
//			case KEY_XXX:			llqt_key = LL_DOM_VK_CLEAR;			break;
			case KEY_RETURN:		llqt_key = LL_DOM_VK_RETURN;			break;
			case KEY_PAD_RETURN:	llqt_key = LL_DOM_VK_ENTER;			break;
			case KEY_SHIFT:			llqt_key = LL_DOM_VK_SHIFT;			break;
			case KEY_CONTROL:		llqt_key = LL_DOM_VK_CONTROL;		break;
			case KEY_ALT:			llqt_key = LL_DOM_VK_ALT;			break;
//			case KEY_XXX:			llqt_key = LL_DOM_VK_PAUSE;			break;
			case KEY_CAPSLOCK:		llqt_key = LL_DOM_VK_CAPS_LOCK;		break;
			case KEY_ESCAPE:		llqt_key = LL_DOM_VK_ESCAPE;			break;
			case KEY_PAGE_UP:		llqt_key = LL_DOM_VK_PAGE_UP;		break;
			case KEY_PAGE_DOWN:		llqt_key = LL_DOM_VK_PAGE_DOWN;		break;
			case KEY_END:			llqt_key = LL_DOM_VK_END;			break;
			case KEY_HOME:			llqt_key = LL_DOM_VK_HOME;			break;
			case KEY_LEFT:			llqt_key = LL_DOM_VK_LEFT;			break;
			case KEY_UP:			llqt_key = LL_DOM_VK_UP;				break;
			case KEY_RIGHT:			llqt_key = LL_DOM_VK_RIGHT;			break;
			case KEY_DOWN:			llqt_key = LL_DOM_VK_DOWN;			break;
//			case KEY_XXX:			llqt_key = LL_DOM_VK_PRINTSCREEN;	break;
			case KEY_INSERT:		llqt_key = LL_DOM_VK_INSERT;			break;
			case KEY_DELETE:		llqt_key = LL_DOM_VK_DELETE;			break;
//			case KEY_XXX:			llqt_key = LL_DOM_VK_CONTEXT_MENU;	break;
			
			default:
				if(key < KEY_SPECIAL)
				{
					// Pass the incoming key through -- it should be regular ASCII, which should be correct for webkit.
					llqt_key = key;
				}
				else
				{
					// Don't pass through untranslated special keys -- they'll be all wrong.
					llqt_key = 0;
				}
			break;
		}
		
//		std::cerr << "keypress, original code = 0x" << std::hex << key << ", converted code = 0x" << std::hex << llqt_key << std::dec << std::endl;
		
		if(llqt_key != 0)
		{
			LLQtWebKit::getInstance()->keyEvent( mBrowserWindowId, key_event, llqt_key, modifiers);
		}

		checkEditState();
	};

	////////////////////////////////////////////////////////////////////////////////
	//
	void unicodeInput( const std::string &utf8str, LLQtWebKit::EKeyboardModifier modifiers)
	{
		LLWString wstr = utf8str_to_wstring(utf8str);
		
		unsigned int i;
		for(i=0; i < wstr.size(); i++)
		{
//			std::cerr << "unicode input, code = 0x" << std::hex << (unsigned long)(wstr[i]) << std::dec << std::endl;
			
			if(wstr[i] == 32)
			{
				// For some reason, the webkit plugin really wants the space bar to come in through the key-event path, not the unicode path.
				LLQtWebKit::getInstance()->keyEvent( mBrowserWindowId, LLQtWebKit::KE_KEY_DOWN, 32, modifiers);
				LLQtWebKit::getInstance()->keyEvent( mBrowserWindowId, LLQtWebKit::KE_KEY_UP, 32, modifiers);
			}
			else
			{
				LLQtWebKit::getInstance()->unicodeInput(mBrowserWindowId, wstr[i], modifiers);
			}
		}

		checkEditState();
	};
	
	void checkEditState(void)
	{
		bool can_cut = LLQtWebKit::getInstance()->userActionIsEnabled( mBrowserWindowId, LLQtWebKit::UA_EDIT_CUT);
		bool can_copy = LLQtWebKit::getInstance()->userActionIsEnabled( mBrowserWindowId, LLQtWebKit::UA_EDIT_COPY);
		bool can_paste = LLQtWebKit::getInstance()->userActionIsEnabled( mBrowserWindowId, LLQtWebKit::UA_EDIT_PASTE);
					
		if((can_cut != mCanCut) || (can_copy != mCanCopy) || (can_paste != mCanPaste))
		{
			LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "edit_state");
			
			if(can_cut != mCanCut)
			{
				mCanCut = can_cut;
				message.setValueBoolean("cut", can_cut);
			}

			if(can_copy != mCanCopy)
			{
				mCanCopy = can_copy;
				message.setValueBoolean("copy", can_copy);
			}

			if(can_paste != mCanPaste)
			{
				mCanPaste = can_paste;
				message.setValueBoolean("paste", can_paste);
			}
			
			sendMessage(message);
			
		}
	}

};

MediaPluginWebKit::MediaPluginWebKit(LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data) :
	MediaPluginBase(host_send_func, host_user_data)
{
//	std::cerr << "MediaPluginWebKit constructor" << std::endl;

	mBrowserWindowId = 0;
	mInitState = INIT_STATE_UNINITIALIZED;
	mNeedsUpdate = true;
	mCanCut = false;
	mCanCopy = false;
	mCanPaste = false;
	mLastMouseX = 0;
	mLastMouseY = 0;
	mFirstFocus = true;
}

MediaPluginWebKit::~MediaPluginWebKit()
{
	// unhook observer
	LLQtWebKit::getInstance()->remObserver( mBrowserWindowId, this );

	// clean up
	LLQtWebKit::getInstance()->reset();

//	std::cerr << "MediaPluginWebKit destructor" << std::endl;
}

void MediaPluginWebKit::receiveMessage(const char *message_string)
{
//	std::cerr << "MediaPluginWebKit::receiveMessage: received message: \"" << message_string << "\"" << std::endl;
	LLPluginMessage message_in;
	
	if(message_in.parse(message_string) >= 0)
	{
		std::string message_class = message_in.getClass();
		std::string message_name = message_in.getName();
		if(message_class == LLPLUGIN_MESSAGE_CLASS_BASE)
		{
			if(message_name == "init")
			{
				std::string user_data_path = message_in.getValue("user_data_path"); // n.b. always has trailing platform-specific dir-delimiter
				mProfileDir = user_data_path + "browser_profile";

				LLPluginMessage message("base", "init_response");
				LLSD versions = LLSD::emptyMap();
				versions[LLPLUGIN_MESSAGE_CLASS_BASE] = LLPLUGIN_MESSAGE_CLASS_BASE_VERSION;
				versions[LLPLUGIN_MESSAGE_CLASS_MEDIA] = LLPLUGIN_MESSAGE_CLASS_MEDIA_VERSION;
				versions[LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER] = LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER_VERSION;
				message.setValueLLSD("versions", versions);

				std::string plugin_version = "Webkit media plugin, Webkit version ";
				plugin_version += LLQtWebKit::getInstance()->getVersion();
				message.setValue("plugin_version", plugin_version);
				sendMessage(message);
				
				// Plugin gets to decide the texture parameters to use.
				mDepth = 4;

				message.setMessage(LLPLUGIN_MESSAGE_CLASS_MEDIA, "texture_params");
				message.setValueS32("default_width", 1024);
				message.setValueS32("default_height", 1024);
				message.setValueS32("depth", mDepth);
				message.setValueU32("internalformat", GL_RGBA);
				message.setValueU32("format", GL_RGBA);
				message.setValueU32("type", GL_UNSIGNED_BYTE);
				message.setValueBoolean("coords_opengl", true);
				sendMessage(message);
			}
			else if(message_name == "idle")
			{
				// no response is necessary here.
				F64 time = message_in.getValueReal("time");
				
				// Convert time to milliseconds for update()
				update((int)(time * 1000.0f));
			}
			else if(message_name == "cleanup")
			{
				// DTOR most likely won't be called but the recent change to the way this process
				// is (not) killed means we see this message and can do what we need to here.
				// Note: this cleanup is ultimately what writes cookies to the disk
				LLQtWebKit::getInstance()->remObserver( mBrowserWindowId, this );
				LLQtWebKit::getInstance()->reset();
			}
			else if(message_name == "shm_added")
			{
				SharedSegmentInfo info;
				info.mAddress = message_in.getValuePointer("address");
				info.mSize = (size_t)message_in.getValueS32("size");
				std::string name = message_in.getValue("name");
				
				
//				std::cerr << "MediaPluginWebKit::receiveMessage: shared memory added, name: " << name 
//					<< ", size: " << info.mSize 
//					<< ", address: " << info.mAddress 
//					<< std::endl;

				mSharedSegments.insert(SharedSegmentMap::value_type(name, info));
			
			}
			else if(message_name == "shm_remove")
			{
				std::string name = message_in.getValue("name");
				
//				std::cerr << "MediaPluginWebKit::receiveMessage: shared memory remove, name = " << name << std::endl;

				SharedSegmentMap::iterator iter = mSharedSegments.find(name);
				if(iter != mSharedSegments.end())
				{
					if(mPixels == iter->second.mAddress)
					{
						// This is the currently active pixel buffer.  Make sure we stop drawing to it.
						mPixels = NULL;
						mTextureSegmentName.clear();
					}
					mSharedSegments.erase(iter);
				}
				else
				{
//					std::cerr << "MediaPluginWebKit::receiveMessage: unknown shared memory region!" << std::endl;
				}

				// Send the response so it can be cleaned up.
				LLPluginMessage message("base", "shm_remove_response");
				message.setValue("name", name);
				sendMessage(message);
			}
			else
			{
//				std::cerr << "MediaPluginWebKit::receiveMessage: unknown base message: " << message_name << std::endl;
			}
		}
		else if(message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA)
		{
			if(message_name == "size_change")
			{
				std::string name = message_in.getValue("name");
				S32 width = message_in.getValueS32("width");
				S32 height = message_in.getValueS32("height");
				S32 texture_width = message_in.getValueS32("texture_width");
				S32 texture_height = message_in.getValueS32("texture_height");
				
				if(!name.empty())
				{
					// Find the shared memory region with this name
					SharedSegmentMap::iterator iter = mSharedSegments.find(name);
					if(iter != mSharedSegments.end())
					{
						mPixels = (unsigned char*)iter->second.mAddress;
						mWidth = width;
						mHeight = height;

						// initialize (only gets called once)
						initBrowser();

						// size changed so tell the browser
						LLQtWebKit::getInstance()->setSize( mBrowserWindowId, mWidth, mHeight );
						
//						std::cerr << "webkit plugin: set size to " << mWidth << " x " << mHeight 
//								<< ", rowspan is " << LLQtWebKit::getInstance()->getBrowserRowSpan(mBrowserWindowId) << std::endl;
								
						S32 real_width = LLQtWebKit::getInstance()->getBrowserRowSpan(mBrowserWindowId) / LLQtWebKit::getInstance()->getBrowserDepth(mBrowserWindowId); 
						
						// The actual width the browser will be drawing to is probably smaller... let the host know by modifying texture_width in the response.
						if(real_width <= texture_width)
						{
							texture_width = real_width;
						}
						else
						{
							// This won't work -- it'll be bigger than the allocated memory.  This is a fatal error.
//							std::cerr << "Fatal error: browser rowbytes greater than texture width" << std::endl;
							mDeleteMe = true;
							return;
						}
						
						mTextureWidth = texture_width;
						mTextureHeight = texture_height;
						
					};
				};

				LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "size_change_response");
				message.setValue("name", name);
				message.setValueS32("width", width);
				message.setValueS32("height", height);
				message.setValueS32("texture_width", texture_width);
				message.setValueS32("texture_height", texture_height);
				sendMessage(message);

			}
			else if(message_name == "load_uri")
			{
				std::string uri = message_in.getValue("uri");

//				std::cout << "loading URI: " << uri << std::endl;
				
				if(!uri.empty())
				{
					if(mInitState >= INIT_STATE_NAVIGATE_COMPLETE)
					{
						LLQtWebKit::getInstance()->navigateTo( mBrowserWindowId, uri );
					}
					else
					{
						mInitialNavigateURL = uri;
					}
				}
			}
			else if(message_name == "mouse_event")
			{
				std::string event = message_in.getValue("event");
				S32 button = message_in.getValueS32("button");
				mLastMouseX = message_in.getValueS32("x");
				mLastMouseY = message_in.getValueS32("y");
				std::string modifiers = message_in.getValue("modifiers");
				
				// Treat unknown mouse events as mouse-moves.
				LLQtWebKit::EMouseEvent mouse_event = LLQtWebKit::ME_MOUSE_MOVE;
				if(event == "down")
				{
					mouse_event = LLQtWebKit::ME_MOUSE_DOWN;
				}
				else if(event == "up")
				{
					mouse_event = LLQtWebKit::ME_MOUSE_UP;
				}
				else if(event == "double_click")
				{
					mouse_event = LLQtWebKit::ME_MOUSE_DOUBLE_CLICK;
				}
				
				LLQtWebKit::getInstance()->mouseEvent( mBrowserWindowId, mouse_event, button, mLastMouseX, mLastMouseY, decodeModifiers(modifiers));
				checkEditState();
			}
			else if(message_name == "scroll_event")
			{
				S32 x = message_in.getValueS32("x");
				S32 y = message_in.getValueS32("y");
				std::string modifiers = message_in.getValue("modifiers");
				
				// Incoming scroll events are adjusted so that 1 detent is approximately 1 unit.
				// Qt expects 1 detent to be 120 units.
				// It also seems that our y scroll direction is inverted vs. what Qt expects.
				
				x *= 120;
				y *= -120;
				
				LLQtWebKit::getInstance()->scrollWheelEvent(mBrowserWindowId, mLastMouseX, mLastMouseY, x, y, decodeModifiers(modifiers));
			}
			else if(message_name == "key_event")
			{
				std::string event = message_in.getValue("event");
				S32 key = message_in.getValueS32("key");
				std::string modifiers = message_in.getValue("modifiers");
				
				// Treat unknown events as key-up for safety.
				LLQtWebKit::EKeyEvent key_event = LLQtWebKit::KE_KEY_UP;
				if(event == "down")
				{
					key_event = LLQtWebKit::KE_KEY_DOWN;
				}
				else if(event == "repeat")
				{
					key_event = LLQtWebKit::KE_KEY_REPEAT;
				}
				
				keyEvent(key_event, key, decodeModifiers(modifiers));
			}
			else if(message_name == "text_event")
			{
				std::string text = message_in.getValue("text");
				std::string modifiers = message_in.getValue("modifiers");
				
				unicodeInput(text, decodeModifiers(modifiers));
			}
			if(message_name == "edit_cut")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_EDIT_CUT );
				checkEditState();
			}
			if(message_name == "edit_copy")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_EDIT_COPY );
				checkEditState();
			}
			if(message_name == "edit_paste")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_EDIT_PASTE );
				checkEditState();
			}
			else
			{
//				std::cerr << "MediaPluginWebKit::receiveMessage: unknown media message: " << message_string << std::endl;
			};
		}
		else if(message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER)
		{
			if(message_name == "focus")
			{
				bool val = message_in.getValueBoolean("focused");
				LLQtWebKit::getInstance()->focusBrowser( mBrowserWindowId, val );
				
				if(mFirstFocus && val)
				{
					// On the first focus, post a tab key event.  This fixes a problem with initial focus.
					std::string empty;
					keyEvent(LLQtWebKit::KE_KEY_DOWN, KEY_TAB, decodeModifiers(empty));
					keyEvent(LLQtWebKit::KE_KEY_UP, KEY_TAB, decodeModifiers(empty));
					mFirstFocus = false;
				}
			}
			else if(message_name == "clear_cache")
			{
				LLQtWebKit::getInstance()->clearCache();
			}
			else if(message_name == "clear_cookies")
			{
				LLQtWebKit::getInstance()->clearAllCookies();
			}
			else if(message_name == "enable_cookies")
			{
				bool val = message_in.getValueBoolean("enable");
				LLQtWebKit::getInstance()->enableCookies( val );
			}
			else if(message_name == "proxy_setup")
			{
				bool val = message_in.getValueBoolean("enable");
				std::string host = message_in.getValue("host");
				int port = message_in.getValueS32("port");
				LLQtWebKit::getInstance()->enableProxy( val, host, port );
			}
			else if(message_name == "browse_stop")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_STOP );
			}
			else if(message_name == "browse_reload")
			{
				// foo = message_in.getValueBoolean("ignore_cache");
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_RELOAD );
			}
			else if(message_name == "browse_forward")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_FORWARD );
			}
			else if(message_name == "browse_back")
			{
				LLQtWebKit::getInstance()->userAction( mBrowserWindowId, LLQtWebKit::UA_NAVIGATE_BACK );
			}
			else if(message_name == "set_status_redirect")
			{
				int code = message_in.getValueS32("code");
				std::string url = message_in.getValue("url");
				if ( 404 == code )	// browser lib only supports 404 right now
				{
					LLQtWebKit::getInstance()->set404RedirectUrl( mBrowserWindowId, url );
				};
			}
			else if(message_name == "set_user_agent")
			{
				std::string user_agent = message_in.getValue("user_agent");
				LLQtWebKit::getInstance()->setBrowserAgentId( user_agent );
			}
			else if(message_name == "init_history")
			{
				// Initialize browser history
				LLSD history = message_in.getValueLLSD("history");
				// First, clear the URL history
				LLQtWebKit::getInstance()->clearHistory(mBrowserWindowId);
				// Then, add the history items in order
				LLSD::array_iterator iter_history = history.beginArray();
				LLSD::array_iterator end_history = history.endArray();
				for(; iter_history != end_history; ++iter_history)
				{
					std::string url = (*iter_history).asString();
					if(! url.empty()) {
						LLQtWebKit::getInstance()->prependHistoryUrl(mBrowserWindowId, url);
					}
				}
			}
			else
			{
//				std::cerr << "MediaPluginWebKit::receiveMessage: unknown media_browser message: " << message_string << std::endl;
			};
		}
		else
		{
//			std::cerr << "MediaPluginWebKit::receiveMessage: unknown message class: " << message_class << std::endl;
		};
	}
}

int init_media_plugin(LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data, LLPluginInstance::sendMessageFunction *plugin_send_func, void **plugin_user_data)
{
	MediaPluginWebKit *self = new MediaPluginWebKit(host_send_func, host_user_data);
	*plugin_send_func = MediaPluginWebKit::staticReceiveMessage;
	*plugin_user_data = (void*)self;

	return 0;
}

