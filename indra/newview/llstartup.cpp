/** 
 * @file llstartup.cpp
 * @brief startup routines.
 *
 * $LicenseInfo:firstyear=2004&license=viewergpl$
 * 
 * Copyright (c) 2004-2009, Linden Research, Inc.
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

#include "llstartup.h"

#if LL_WINDOWS
#	include <process.h>		// _spawnl()
#else
#	include <sys/stat.h>		// mkdir()
#endif

#include "audioengine.h"

#ifdef LL_FMOD
# include "audioengine_fmod.h"
#endif

#ifdef LL_OPENAL
#include "audioengine_openal.h"
#endif

#include "llares.h"
#include "llcachename.h"
#include "llviewercontrol.h"
#include "lldir.h"
#include "llerrorcontrol.h"
#include "llfiltersd2xmlrpc.h"
#include "llfocusmgr.h"
#include "llhttpsender.h"
#include "imageids.h"
#include "lllandmark.h"
#include "llloginflags.h"
#include "llmd5.h"
#include "llmemorystream.h"
#include "llmessageconfig.h"
#include "llmoveview.h"
#include "llregionhandle.h"
#include "llsd.h"
#include "llsdserialize.h"
#include "llsdutil.h"
#include "llsecondlifeurls.h"
#include "llstring.h"
#include "lluserrelations.h"
#include "llversionviewer.h"
#include "llvfs.h"
#include "llxorcipher.h"	// saved password, MAC address
#include "message.h"
#include "v3math.h"

#include "llagent.h"
#include "llagentpilot.h"
#include "llfloateravatarpicker.h"
#include "llcallbacklist.h"
#include "llcallingcard.h"
#include "llcolorscheme.h"
#include "llconsole.h"
#include "llcontainerview.h"
#include "llfloaterstats.h"
#include "lldebugview.h"
#include "lldrawable.h"
#include "lleventnotifier.h"
#include "llface.h"
#include "llfeaturemanager.h"
#include "llfirstuse.h"
#include "llfloateractivespeakers.h"
#include "llfloaterbeacons.h"
#include "llfloatercamera.h"
#include "llfloaterchat.h"
#include "llfloatergesture.h"
#include "llfloaterhud.h"
#include "llfloaterland.h"
#include "llfloatertopobjects.h"
#include "llfloatertos.h"
#include "llfloaterworldmap.h"
#include "llframestats.h"
#include "llframestatview.h"
#include "llgesturemgr.h"
#include "llgroupmgr.h"
#include "llhudeffecttrail.h"
#include "llhudmanager.h"
#include "llhttpclient.h"
#include "llimagebmp.h"
#include "llinventorymodel.h"
#include "llinventoryview.h"
#include "llkeyboard.h"
#include "llloginhandler.h"			// gLoginHandler, SLURL support
#include "llpanellogin.h"
#include "llmutelist.h"
#include "llnotify.h"
#include "llpanelavatar.h"
#include "llpaneldirbrowser.h"
#include "llpaneldirland.h"
#include "llpanelevent.h"
#include "llpanelclassified.h"
#include "llpanelpick.h"
#include "llpanelplace.h"
#include "llpanelgrouplandmoney.h"
#include "llpanelgroupnotices.h"
#include "llpreview.h"
#include "llpreviewscript.h"
#include "llproductinforequest.h"
#include "llsecondlifeurls.h"
#include "llselectmgr.h"
#include "llsky.h"
#include "llsrv.h"
#include "llstatview.h"
#include "lltrans.h"
#include "llstatusbar.h"		// sendMoneyBalanceRequest(), owns L$ balance
#include "llsurface.h"
#include "lltexturecache.h"
#include "lltexturefetch.h"
#include "lltoolmgr.h"
#include "llui.h"
#include "llurldispatcher.h"
#include "llurlsimstring.h"
#include "llurlhistory.h"
#include "llurlwhitelist.h"
#include "lluserauth.h"
#include "llvieweraudio.h"
#include "llviewerassetstorage.h"
#include "llviewercamera.h"
#include "llviewerdisplay.h"
#include "llviewergenericmessage.h"
#include "llviewergesture.h"
#include "llviewerimagelist.h"
#include "llviewermedia.h"
#include "llviewermenu.h"
#include "llviewermessage.h"
#include "llviewernetwork.h"
#include "llviewerobjectlist.h"
#include "llviewerparcelmedia.h"
#include "llviewerparcelmgr.h"
#include "llviewerregion.h"
#include "llviewerstats.h"
#include "llviewerthrottle.h"
#include "llviewerwindow.h"
#include "llvoavatar.h"
#include "llvoclouds.h"
#include "llweb.h"
#include "llworld.h"
#include "llworldmap.h"
#include "llxfermanager.h"
#include "pipeline.h"
#include "llappviewer.h"
#include "llfasttimerview.h"
#include "llfloatermap.h"
#include "llweb.h"
#include "llvoiceclient.h"
#include "llnamelistctrl.h"
#include "llnamebox.h"
#include "llnameeditor.h"
#include "llpostprocess.h"
#include "llwlparammanager.h"
#include "llwaterparammanager.h"
#include "llagentlanguage.h"

#include "llfloateravatarlist.h"
#include "jcfloater_animation_list.h"
#include "jcfloater_areasearch.h"
#include "exporttracker.h"
#include "llfloaterteleporthistory.h"
#include "jc_lslviewerbridge.h"
#include "wlfPanel_AdvSettings.h"
#include "llfloaterao.h"
#if LL_LIBXUL_ENABLED
#include "llmozlib.h"
#endif // LL_LIBXUL_ENABLED

#if LL_WINDOWS
#include "llwindebug.h"
#include "lldxhardware.h"
#endif

#if COMPILE_OTR          // [$PLOTR$]
#include "otr_wrapper.h"
#include "lggIrcGroupHandler.h"
#endif // COMPILE_OTR    // [/$PLOTR$]

//
// exported globals
//
bool gAgentMovementCompleted = false;
std::string gInitialOutfit;
std::string gInitialOutfitGender;

std::string SCREEN_HOME_FILENAME = "screen_home.bmp";
std::string SCREEN_LAST_FILENAME = "screen_last.bmp";

//
// Imported globals
//
extern S32 gStartImageWidth;
extern S32 gStartImageHeight;

//
// local globals
//

LLPointer<LLImageGL> gStartImageGL;

static LLHost gAgentSimHost;
static BOOL gSkipOptionalUpdate = FALSE;

static bool gGotUseCircuitCodeAck = false;
static std::string sInitialOutfit;
static std::string sInitialOutfitGender;	// "male" or "female"

static bool gUseCircuitCallbackCalled = false;

EStartupState LLStartUp::gStartupState = STATE_FIRST;


//
// local function declaration
//

void login_show();
void login_callback(S32 option, void* userdata);
void show_first_run_dialog();
bool first_run_dialog_callback(const LLSD& notification, const LLSD& response);
void set_startup_status(const F32 frac, const std::string& string, const std::string& msg);
bool login_alert_status(const LLSD& notification, const LLSD& response);
void update_app(BOOL mandatory, const std::string& message);
bool update_dialog_callback(const LLSD& notification, const LLSD& response);
void login_packet_failed(void**, S32 result);
void use_circuit_callback(void**, S32 result);
void register_viewer_callbacks(LLMessageSystem* msg);
void init_stat_view();
void asset_callback_nothing(LLVFS*, const LLUUID&, LLAssetType::EType, void*, S32);
bool callback_choose_gender(const LLSD& notification, const LLSD& response);
void init_start_screen(S32 location_id);
void release_start_screen();
void reset_login();
void apply_udp_blacklist(const std::string& csv);

void callback_cache_name(const LLUUID& id, const std::string& firstname, const std::string& lastname, BOOL is_group, void* data)
{
	LLNameListCtrl::refreshAll(id, firstname, lastname, is_group);
	LLNameBox::refreshAll(id, firstname, lastname, is_group);
	LLNameEditor::refreshAll(id, firstname, lastname, is_group);
	
	// TODO: Actually be intelligent about the refresh.
	// For now, just brute force refresh the dialogs.
	dialog_refresh_all();
}

//
// exported functionality
//

//
// local classes
//

namespace
{
	class LLNullHTTPSender : public LLHTTPSender
	{
		virtual void send(const LLHost& host, 
						  const std::string& message, const LLSD& body, 
						  LLHTTPClient::ResponderPtr response) const
		{
			LL_WARNS("AppInit") << " attemped to send " << message << " to " << host
					<< " with null sender" << LL_ENDL;
		}
	};
}

class LLGestureInventoryFetchObserver : public LLInventoryFetchObserver
{
public:
	LLGestureInventoryFetchObserver() {}
	virtual void done()
	{
		// we've downloaded all the items, so repaint the dialog
		LLFloaterGesture::refreshAll();

		gInventory.removeObserver(this);
		delete this;
	}
};

void update_texture_fetch()
{
	LLAppViewer::getTextureCache()->update(1); // unpauses the texture cache thread
	LLAppViewer::getImageDecodeThread()->update(1); // unpauses the image thread
	LLAppViewer::getTextureFetch()->update(1); // unpauses the texture fetch thread
	gImageList.updateImages(0.10f);
}
void pass_process_sound_trigger(LLMessageSystem* msg,void**)
{
	process_sound_trigger(msg,0);
	LLFloaterAvatarList::processSoundTrigger(msg,0);
	JCLSLBridge::processSoundTrigger(msg,0);
}
// Returns false to skip other idle processing. Should only return
// true when all initialization done.
bool idle_startup()
{
	LLMemType mt1(LLMemType::MTYPE_STARTUP);

	const F32 TIMEOUT_SECONDS = 5.f;
	const S32 MAX_TIMEOUT_COUNT = 3;
	static LLTimer timeout;
	static S32 timeout_count = 0;

	static LLTimer login_time;

	// until this is encapsulated, this little hack for the
	// auth/transform loop will do.
	static F32 progress = 0.10f;

	static std::string auth_method;
	static std::string auth_desc;
	static std::string auth_message;
	static std::string firstname;
	static std::string lastname;
	static LLUUID web_login_key;
	static std::string password;
	static std::vector<const char*> requested_options;

	static U64 first_sim_handle = 0;
	static LLHost first_sim;
	static std::string first_sim_seed_cap;

	static LLVector3 initial_sun_direction(1.f, 0.f, 0.f);
	static LLVector3 agent_start_position_region(10.f, 10.f, 10.f);		// default for when no space server
	static LLVector3 agent_start_look_at(1.0f, 0.f, 0.f);
	static std::string agent_start_location = "safe";

	// last location by default
	static S32  agent_location_id = START_LOCATION_ID_LAST;
	static S32  location_which = START_LOCATION_ID_LAST;

	static bool show_connect_box = true;

	static bool stipend_since_login = false;

	static bool samename = false;

	// HACK: These are things from the main loop that usually aren't done
	// until initialization is complete, but need to be done here for things
	// to work.
	gIdleCallbacks.callFunctions();
	gViewerWindow->handlePerFrameHover();
	LLMortician::updateClass();

	if (gNoRender)
	{
		// HACK, skip optional updates if you're running drones
		gSkipOptionalUpdate = TRUE;
	}
	else
	{
		// Update images?
		gImageList.updateImages(0.01f);
	}

	if ( STATE_FIRST == LLStartUp::getStartupState() )
	{
		gViewerWindow->showCursor();
		gViewerWindow->getWindow()->setCursor(UI_CURSOR_WAIT);

		/////////////////////////////////////////////////
		//
		// Initialize stuff that doesn't need data from simulators
		//

		new JCLSLBridge();
		new AOInvTimer();

// [RLVa:KB] - Version: 1.23.4 | Checked: 2009-07-10 (RLVa-1.0.0g) | Modified: RLVa-0.2.1d
		if ( (gSavedSettings.controlExists(RLV_SETTING_MAIN)) && (gSavedSettings.getBOOL(RLV_SETTING_MAIN)) )
			rlv_handler_t::setEnabled(TRUE);
// [/RLVa:KB]

		if (LLFeatureManager::getInstance()->isSafe())
		{
			LLNotifications::instance().add("DisplaySetToSafe");
		}
		else if ((gSavedSettings.getS32("LastFeatureVersion") < LLFeatureManager::getInstance()->getVersion()) &&
				 (gSavedSettings.getS32("LastFeatureVersion") != 0))
		{
			LLNotifications::instance().add("DisplaySetToRecommended");
		}
		else if (!gViewerWindow->getInitAlert().empty())
		{
			LLNotifications::instance().add(gViewerWindow->getInitAlert());
		}
			
		gSavedSettings.setS32("LastFeatureVersion", LLFeatureManager::getInstance()->getVersion());

		std::string xml_file = LLUI::locateSkin("xui_version.xml");
		LLXMLNodePtr root;
		bool xml_ok = false;
		if (LLXMLNode::parseFile(xml_file, root, NULL))
		{
			if( (root->hasName("xui_version") ) )
			{
				std::string value = root->getValue();
				F32 version = 0.0f;
				LLStringUtil::convertToF32(value, version);
				if (version >= 1.0f)
				{
					xml_ok = true;
				}
			}
		}
		if (!xml_ok)
		{
			// If XML is bad, there's a good possibility that notifications.xml is ALSO bad.
			// If that's so, then we'll get a fatal error on attempting to load it, 
			// which will display a nontranslatable error message that says so.
			// Otherwise, we'll display a reasonable error message that IS translatable.
			LLAppViewer::instance()->earlyExit("BadInstallation");
		}
		//
		// Statistics stuff
		//

		// Load autopilot and stats stuff
		gAgentPilot.load(gSavedSettings.getString("StatsPilotFile"));
		gFrameStats.setFilename(gSavedSettings.getString("StatsFile"));
		gFrameStats.setSummaryFilename(gSavedSettings.getString("StatsSummaryFile"));

		//gErrorStream.setTime(gSavedSettings.getBOOL("LogTimestamps"));

		// Load the throttle settings
		gViewerThrottle.load();

		if (ll_init_ares() == NULL || !gAres->isInitialized())
		{
			std::string diagnostic = "Could not start address resolution system";
			LL_WARNS("AppInit") << diagnostic << LL_ENDL;
			LLAppViewer::instance()->earlyExit("LoginFailedNoNetwork", LLSD().insert("DIAGNOSTIC", diagnostic));
		}
		
		//
		// Initialize messaging system
		//
		LL_DEBUGS("AppInit") << "Initializing messaging system..." << LL_ENDL;

		std::string message_template_path = gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS,"message_template.msg");

		LLFILE* found_template = NULL;
		found_template = LLFile::fopen(message_template_path, "r");		/* Flawfinder: ignore */
		
		#if LL_WINDOWS
			// On the windows dev builds, unpackaged, the message_template.msg 
			// file will be located in 
			// indra/build-vc**/newview/<config>/app_settings.
			if (!found_template)
			{
				message_template_path = gDirUtilp->getExpandedFilename(LL_PATH_EXECUTABLE, "app_settings", "message_template.msg");
				found_template = LLFile::fopen(message_template_path.c_str(), "r");		/* Flawfinder: ignore */
			}	
		#endif

		if (found_template)
		{
			fclose(found_template);

			U32 port = gSavedSettings.getU32("UserConnectionPort");

			if ((NET_USE_OS_ASSIGNED_PORT == port) &&   // if nothing specified on command line (-port)
			    (gSavedSettings.getBOOL("ConnectionPortEnabled")))
			  {
			    port = gSavedSettings.getU32("ConnectionPort");
			  }

			LLHTTPSender::setDefaultSender(new LLNullHTTPSender());

			// TODO parameterize 
			const F32 circuit_heartbeat_interval = 5;
			const F32 circuit_timeout = 100;

			const LLUseCircuitCodeResponder* responder = NULL;
			bool failure_is_fatal = true;
			
			if(!start_messaging_system(
				   message_template_path,
				   port,
				   LL_VERSION_MAJOR,
				   LL_VERSION_MINOR,
				   LL_VERSION_PATCH,
				   FALSE,
				   std::string(),
				   responder,
				   failure_is_fatal,
				   circuit_heartbeat_interval,
				   circuit_timeout))
			{
				std::string diagnostic = llformat(" Error: %d", gMessageSystem->getErrorCode());
				LL_WARNS("AppInit") << diagnostic << LL_ENDL;
				LLAppViewer::instance()->earlyExit("LoginFailedNoNetwork", LLSD().insert("DIAGNOSTIC", diagnostic));
			}

			#if LL_WINDOWS
				// On the windows dev builds, unpackaged, the message.xml file will 
				// be located in indra/build-vc**/newview/<config>/app_settings.
				std::string message_path = gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS,"message.xml");
							
				if (!LLFile::isfile(message_path.c_str())) 
				{
					LLMessageConfig::initClass("viewer", gDirUtilp->getExpandedFilename(LL_PATH_EXECUTABLE, "app_settings", ""));
				}
				else
				{
					LLMessageConfig::initClass("viewer", gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, ""));
				}
			#else			
				LLMessageConfig::initClass("viewer", gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, ""));
			#endif

		}
		else
		{
			LLAppViewer::instance()->earlyExit("MessageTemplateNotFound", LLSD().insert("PATH", message_template_path));
		}

		if(gMessageSystem && gMessageSystem->isOK())
		{
			// Initialize all of the callbacks in case of bad message
			// system data
			LLMessageSystem* msg = gMessageSystem;
			msg->setExceptionFunc(MX_UNREGISTERED_MESSAGE,
								  invalid_message_callback,
								  NULL);
			msg->setExceptionFunc(MX_PACKET_TOO_SHORT,
								  invalid_message_callback,
								  NULL);

			// running off end of a packet is now valid in the case
			// when a reader has a newer message template than
			// the sender
			/*msg->setExceptionFunc(MX_RAN_OFF_END_OF_PACKET,
								  invalid_message_callback,
								  NULL);*/
			msg->setExceptionFunc(MX_WROTE_PAST_BUFFER_SIZE,
								  invalid_message_callback,
								  NULL);

			if (gSavedSettings.getBOOL("LogMessages"))
			{
				LL_DEBUGS("AppInit") << "Message logging activated!" << LL_ENDL;
				msg->startLogging();
			}

			// start the xfer system. by default, choke the downloads
			// a lot...
			const S32 VIEWER_MAX_XFER = 3;
			start_xfer_manager(gVFS);
			gXferManager->setMaxIncomingXfers(VIEWER_MAX_XFER);
			F32 xfer_throttle_bps = gSavedSettings.getF32("XferThrottle");
			if (xfer_throttle_bps > 1.f)
			{
				gXferManager->setUseAckThrottling(TRUE);
				gXferManager->setAckThrottleBPS(xfer_throttle_bps);
			}
			gAssetStorage = new LLViewerAssetStorage(msg, gXferManager, gVFS);


			F32 dropPercent = gSavedSettings.getF32("PacketDropPercentage");
			msg->mPacketRing.setDropPercentage(dropPercent);

            F32 inBandwidth = gSavedSettings.getF32("InBandwidth"); 
            F32 outBandwidth = gSavedSettings.getF32("OutBandwidth"); 
			if (inBandwidth != 0.f)
			{
				LL_DEBUGS("AppInit") << "Setting packetring incoming bandwidth to " << inBandwidth << LL_ENDL;
				msg->mPacketRing.setUseInThrottle(TRUE);
				msg->mPacketRing.setInBandwidth(inBandwidth);
			}
			if (outBandwidth != 0.f)
			{
				LL_DEBUGS("AppInit") << "Setting packetring outgoing bandwidth to " << outBandwidth << LL_ENDL;
				msg->mPacketRing.setUseOutThrottle(TRUE);
				msg->mPacketRing.setOutBandwidth(outBandwidth);
			}
		}

		LL_INFOS("AppInit") << "Message System Initialized." << LL_ENDL;
		
		//-------------------------------------------------
		// Init audio, which may be needed for prefs dialog
		// or audio cues in connection UI.
		//-------------------------------------------------

		if (FALSE == gSavedSettings.getBOOL("NoAudio"))
		{
			gAudiop = NULL;

#ifdef LL_FMOD			
			if (!gAudiop
#if !LL_WINDOWS
			    && NULL == getenv("LL_BAD_FMOD_DRIVER")
#endif // !LL_WINDOWS
			    )
			{
				gAudiop = (LLAudioEngine *) new LLAudioEngine_FMOD();

			if (gAudiop)
			{
#if LL_WINDOWS
				// FMOD on Windows needs the window handle to stop playing audio
				// when window is minimized. JC
				void* window_handle = (HWND)gViewerWindow->getPlatformWindow();
#else
				void* window_handle = NULL;
#endif
				bool init = gAudiop->init(kAUDIO_NUM_SOURCES, window_handle);
				if(init)
				{
					gAudiop->setMuted(TRUE);
				}
				else
				{
					LL_WARNS("AppInit") << "Unable to initialize audio engine" << LL_ENDL;
					delete gAudiop;
					gAudiop = NULL;
				}
			}
			}
#endif

#ifdef LL_OPENAL
			if (!gAudiop
#if !LL_WINDOWS
			    && NULL == getenv("LL_BAD_OPENAL_DRIVER")
#endif // !LL_WINDOWS
			    )
			{
				gAudiop = (LLAudioEngine *) new LLAudioEngine_OpenAL();

			if (gAudiop)
			{
#if LL_WINDOWS
				// FMOD on Windows needs the window handle to stop playing audio
				// when window is minimized. JC
				void* window_handle = (HWND)gViewerWindow->getPlatformWindow();
#else
				void* window_handle = NULL;
#endif
				bool init = gAudiop->init(kAUDIO_NUM_SOURCES, window_handle);
				if(init)
				{
					gAudiop->setMuted(TRUE);
				}
				else
				{
					LL_WARNS("AppInit") << "Unable to initialize audio engine" << LL_ENDL;
					delete gAudiop;
					gAudiop = NULL;
				}
			}
			}
#endif
		}
		
		LL_INFOS("AppInit") << "Audio Engine Initialized." << LL_ENDL;
		
		if (LLTimer::knownBadTimer())
		{
			LL_WARNS("AppInit") << "Unreliable timers detected (may be bad PCI chipset)!!" << LL_ENDL;
		}

		//
		// Log on to system
		//
		if (!LLStartUp::sSLURLCommand.empty())
		{
			// this might be a secondlife:///app/login URL
			gLoginHandler.parseDirectLogin(LLStartUp::sSLURLCommand);
		}
		if (!gLoginHandler.getFirstName().empty()
			|| !gLoginHandler.getLastName().empty()
			|| !gLoginHandler.getWebLoginKey().isNull() )
		{
			// We have at least some login information on a SLURL
			firstname = gLoginHandler.getFirstName();
			lastname = gLoginHandler.getLastName();
			web_login_key = gLoginHandler.getWebLoginKey();

			// Show the login screen if we don't have everything
			show_connect_box = 
				firstname.empty() || lastname.empty() || web_login_key.isNull();
		}
        else if(gSavedSettings.getLLSD("UserLoginInfo").size() == 3)
        {
            LLSD cmd_line_login = gSavedSettings.getLLSD("UserLoginInfo");
			firstname = cmd_line_login[0].asString();
			lastname = cmd_line_login[1].asString();

			LLMD5 pass((unsigned char*)cmd_line_login[2].asString().c_str());
			char md5pass[33];               /* Flawfinder: ignore */
			pass.hex_digest(md5pass);
			password = md5pass;
			
#ifdef USE_VIEWER_AUTH
			show_connect_box = true;
#else
			show_connect_box = false;
#endif
			gSavedSettings.setBOOL("AutoLogin", TRUE);
        }
		else if (gSavedSettings.getBOOL("AutoLogin"))
		{
			firstname = gSavedSettings.getString("FirstName");
			lastname = gSavedSettings.getString("LastName");
			password = LLStartUp::loadPasswordFromDisk();
			gSavedSettings.setBOOL("RememberPassword", TRUE);
			
#ifdef USE_VIEWER_AUTH
			show_connect_box = true;
#else
			show_connect_box = false;
#endif
		}
		else
		{
			// if not automatically logging in, display login dialog
			// a valid grid is selected
			firstname = gSavedSettings.getString("FirstName");
			lastname = gSavedSettings.getString("LastName");
			password = LLStartUp::loadPasswordFromDisk();
			show_connect_box = true;
		}


		// Go to the next startup state
		LLStartUp::setStartupState( STATE_BROWSER_INIT );
		return FALSE;
	}

	
	if (STATE_BROWSER_INIT == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "STATE_BROWSER_INIT" << LL_ENDL;
		std::string msg = LLTrans::getString("LoginInitializingBrowser");
		set_startup_status(0.03f, msg.c_str(), gAgent.mMOTD.c_str());
		display_startup();
		LLViewerMedia::initBrowser();

		LLStartUp::setStartupState( STATE_LOGIN_SHOW );
		return FALSE;
	}


	if (STATE_LOGIN_SHOW == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "Initializing Window" << LL_ENDL;
		
		gViewerWindow->getWindow()->setCursor(UI_CURSOR_ARROW);

		timeout_count = 0;

		if (show_connect_box)
		{
			// Load all the name information out of the login view
			// NOTE: Hits "Attempted getFields with no login view shown" warning, since we don't
			// show the login view until login_show() is called below.  
			// LLPanelLogin::getFields(firstname, lastname, password);

			if (gNoRender)
			{
				LL_ERRS("AppInit") << "Need to autologin or use command line with norender!" << LL_ENDL;
			}
			// Make sure the process dialog doesn't hide things
			gViewerWindow->setShowProgress(FALSE);
			
			// Load login history
			std::string login_hist_filepath = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "saved_logins.xml");
			LLSavedLogins login_history = LLSavedLogins::loadFile(login_hist_filepath);

			// Show the login dialog
			login_show();
			// connect dialog is already shown, so fill in the names
			LLPanelLogin::setFields( firstname, lastname, password, login_history );

			LLPanelLogin::giveFocus();

			gSavedSettings.setBOOL("FirstRunThisInstall", FALSE);

			LLStartUp::setStartupState( STATE_LOGIN_WAIT );		// Wait for user input
		}
		else
		{
			// skip directly to message template verification
			LLStartUp::setStartupState( STATE_LOGIN_CLEANUP );
		}

		// *NOTE: This is where LLViewerParcelMgr::getInstance() used to get allocated before becoming LLViewerParcelMgr::getInstance().

		// *NOTE: This is where gHUDManager used to bet allocated before becoming LLHUDManager::getInstance().

		// *NOTE: This is where gMuteList used to get allocated before becoming LLMuteList::getInstance().

		// Initialize UI
		if (!gNoRender)
		{
			// Initialize all our tools.  Must be done after saved settings loaded.
			// NOTE: This also is where gToolMgr used to be instantiated before being turned into a singleton.
			LLToolMgr::getInstance()->initTools();

			// Quickly get something onscreen to look at.
			gViewerWindow->initWorldUI();
		}
		
		gViewerWindow->setNormalControlsVisible( FALSE );	
		gLoginMenuBarView->setVisible( TRUE );
		gLoginMenuBarView->setEnabled( TRUE );

		// Push our window frontmost
		gViewerWindow->getWindow()->show();
		display_startup();

		// DEV-16927.  The following code removes errant keystrokes that happen while the window is being 
		// first made visible.
#ifdef _WIN32
		MSG msg;
		while( PeekMessage( &msg, /*All hWnds owned by this thread */ NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) );
#endif
		timeout.reset();
		return FALSE;
	}

	if (STATE_LOGIN_WAIT == LLStartUp::getStartupState())
	{
		// Don't do anything.  Wait for the login view to call the login_callback,
		// which will push us to the next state.

		// Sleep so we don't spin the CPU
		ms_sleep(1);
		return FALSE;
	}
	
	if (STATE_LOGIN_CLEANUP == LLStartUp::getStartupState())
	{
		//reset the values that could have come in from a slurl
		if (!gLoginHandler.getWebLoginKey().isNull())
		{
			firstname = gLoginHandler.getFirstName();
			lastname = gLoginHandler.getLastName();
			web_login_key = gLoginHandler.getWebLoginKey();
		}
				
		if (show_connect_box)
		{
			// TODO if not use viewer auth
			// Load all the name information out of the login view
			LLPanelLogin::getFields(&firstname, &lastname, &password);
			// end TODO			
	 
			// HACK: Try to make not jump on login
			gKeyboard->resetKeys();
		}

		if (!firstname.empty() && !lastname.empty())
		{
			gSavedSettings.setString("FirstName", firstname);
			gSavedSettings.setString("LastName", lastname);
			if (!gSavedSettings.controlExists("RememberLogin")) gSavedSettings.declareBOOL("RememberLogin", false, "Remember login", false);
			gSavedSettings.setBOOL("RememberLogin", LLPanelLogin::getRememberLogin());

			LL_INFOS("AppInit") << "Attempting login as: " << firstname << " " << lastname << LL_ENDL;
			gDebugInfo["LoginName"] = firstname + " " + lastname;	
		}

		// create necessary directories
		// *FIX: these mkdir's should error check
		gDirUtilp->setLindenUserDir(firstname, lastname);
    	LLFile::mkdir(gDirUtilp->getLindenUserDir());

        // Set PerAccountSettingsFile to the default value.
		gSavedSettings.setString("PerAccountSettingsFile",
			gDirUtilp->getExpandedFilename(LL_PATH_PER_SL_ACCOUNT, 
				LLAppViewer::instance()->getSettingsFilename("Default", "PerAccount")
				)
			);

		// Overwrite default user settings with user settings								 
		LLAppViewer::instance()->loadSettingsFromDirectory("Account");

		// Need to set the LastLogoff time here if we don't have one.  LastLogoff is used for "Recent Items" calculation
		// and startup time is close enough if we don't have a real value.
		if (gSavedPerAccountSettings.getU32("LastLogoff") == 0)
		{
			gSavedPerAccountSettings.setU32("LastLogoff", time_corrected());
		}

		//Always use default in portable mode
		if (gSavedSettings.getBOOL("PortableMode"))
		{
			gDirUtilp->setChatLogsDir(gDirUtilp->getOSUserAppDir());
			gSavedPerAccountSettings.setString("InstantMessageLogPath",gDirUtilp->getChatLogsDir());
		}
		//Default the path if one isn't set.
		else if (gSavedPerAccountSettings.getString("InstantMessageLogPath").empty())
		{
			gDirUtilp->setChatLogsDir(gDirUtilp->getOSUserAppDir());
			gSavedPerAccountSettings.setString("InstantMessageLogPath",gDirUtilp->getChatLogsDir());
		}
		else
		{
			gDirUtilp->setChatLogsDir(gSavedPerAccountSettings.getString("InstantMessageLogPath"));		
		}
		
		gDirUtilp->setPerAccountChatLogsDir(firstname, lastname);

		LLFile::mkdir(gDirUtilp->getChatLogsDir());
		LLFile::mkdir(gDirUtilp->getPerAccountChatLogsDir());

		//good as place as any to create user windlight directories
		std::string user_windlight_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "windlight", ""));
		LLFile::mkdir(user_windlight_path_name.c_str());		

		std::string user_windlight_skies_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "windlight/skies", ""));
		LLFile::mkdir(user_windlight_skies_path_name.c_str());

		std::string user_windlight_water_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "windlight/water", ""));
		LLFile::mkdir(user_windlight_water_path_name.c_str());

		std::string user_windlight_days_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "windlight/days", ""));
		LLFile::mkdir(user_windlight_days_path_name.c_str());

		//guna make a beams directior here too /lgg		
		std::string lgg_beams_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beams", ""));
		LLFile::mkdir(lgg_beams_path_name.c_str());	

		std::string lgg_beamscolors_path_name(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beamsColors", ""));
		LLFile::mkdir(lgg_beamscolors_path_name.c_str());	

		//guna make a ircgroups directior here too /lgg		
		std::string lgg_ircgroups_path_name(gDirUtilp->getExpandedFilename( LL_PATH_PER_SL_ACCOUNT, "IRCGroups", ""));
		LLFile::mkdir(lgg_ircgroups_path_name.c_str());	


		if (show_connect_box)
		{
			// *TODO Remove
/*			if ( LLPanelLogin::isGridComboDirty() )
			{
				// User picked a grid from the popup, so clear the 
				// stored uris and they will be reacquired from the grid choice.
				sAuthUris.clear();
			}
*/			
			std::string location;
			LLPanelLogin::getLocation( location );
			LLURLSimString::setString( location );

			// END TODO
			LLPanelLogin::close();
		}

		
		//For HTML parsing in text boxes.
		LLTextEditor::setLinkColor( gSavedSettings.getColor4("HTMLLinkColor") );

		// Load URL History File
		LLURLHistory::loadFile("url_history.xml");

		//-------------------------------------------------
		// Handle startup progress screen
		//-------------------------------------------------

		// on startup the user can request to go to their home,
		// their last location, or some URL "-url //sim/x/y[/z]"
		// All accounts have both a home and a last location, and we don't support
		// more locations than that.  Choose the appropriate one.  JC
// [RLVa:KB] - Checked: 2009-07-08 (RLVa-1.0.0e) | Modified: RLVa-0.2.1d
		#ifndef RLV_EXTENSION_STARTLOCATION
		if (rlv_handler_t::isEnabled())
		#else
		if ( (rlv_handler_t::isEnabled()) && (RlvSettings::getLoginLastLocation()) )
		#endif // RLV_EXTENSION_STARTLOCATION
		{
			// Force login at the last location
			agent_location_id = START_LOCATION_ID_LAST;
			location_which = START_LOCATION_ID_LAST;
			gSavedSettings.setBOOL("LoginLastLocation", FALSE);
			
			// Clear some things that would cause us to divert to a user-specified location
			LLURLSimString::setString(LLURLSimString::sLocationStringLast);
			LLStartUp::sSLURLCommand.clear();
		} else
// [/RLVa:KB]
		if (LLURLSimString::parse())
		{
			// a startup URL was specified
			agent_location_id = START_LOCATION_ID_URL;

			// doesn't really matter what location_which is, since
			// agent_start_look_at will be overwritten when the
			// UserLoginLocationReply arrives
			location_which = START_LOCATION_ID_LAST;
		}
		else if (gSavedSettings.getBOOL("LoginLastLocation"))
		{
			agent_location_id = START_LOCATION_ID_LAST;	// last location
			location_which = START_LOCATION_ID_LAST;
		}
		else
		{
			agent_location_id = START_LOCATION_ID_HOME;	// home
			location_which = START_LOCATION_ID_HOME;
		}

		gViewerWindow->getWindow()->setCursor(UI_CURSOR_WAIT);

		if (!gNoRender)
		{
			init_start_screen(agent_location_id);
		}

		// Display the startup progress bar.
		gViewerWindow->setShowProgress(!gSavedSettings.getBOOL("EmeraldDisableLoginScreens"));
		gViewerWindow->setProgressCancelButtonVisible(TRUE, std::string("Quit")); // *TODO: Translate

		// Poke the VFS, which could potentially block for a while if
		// Windows XP is acting up
		set_startup_status(0.07f, LLTrans::getString("LoginVerifyingCache"), LLStringUtil::null);
		display_startup();

		gVFS->pokeFiles();

		// color init must be after saved settings loaded
		init_colors();

		// skipping over STATE_UPDATE_CHECK because that just waits for input
		LLStartUp::setStartupState( STATE_LOGIN_AUTH_INIT );

		return FALSE;
	}

	if (STATE_UPDATE_CHECK == LLStartUp::getStartupState())
	{
		// wait for user to give input via dialog box
		return FALSE;
	}

	if(STATE_LOGIN_AUTH_INIT == LLStartUp::getStartupState())
	{
//#define LL_MINIMIAL_REQUESTED_OPTIONS
		gDebugInfo["GridName"] = LLViewerLogin::getInstance()->getGridLabel();

		// *Note: this is where gUserAuth used to be created.
		requested_options.clear();
		requested_options.push_back("inventory-root");
		requested_options.push_back("inventory-skeleton");
		//requested_options.push_back("inventory-meat");
		//requested_options.push_back("inventory-skel-targets");
#if (!defined LL_MINIMIAL_REQUESTED_OPTIONS)
		if(FALSE == gSavedSettings.getBOOL("NoInventoryLibrary"))
		{
			requested_options.push_back("inventory-lib-root");
			requested_options.push_back("inventory-lib-owner");
			requested_options.push_back("inventory-skel-lib");
		//	requested_options.push_back("inventory-meat-lib");
		}

		requested_options.push_back("initial-outfit");
		requested_options.push_back("gestures");
		requested_options.push_back("event_categories");
		requested_options.push_back("event_notifications");
		requested_options.push_back("classified_categories");
		requested_options.push_back("adult_compliant");
		//requested_options.push_back("inventory-targets");
		requested_options.push_back("buddy-list");
		requested_options.push_back("ui-config");
#endif
		requested_options.push_back("tutorial_setting");
		requested_options.push_back("login-flags");
		requested_options.push_back("global-textures");
		if(gSavedSettings.getBOOL("ConnectAsGod"))
		{
			gSavedSettings.setBOOL("UseDebugMenus", TRUE);
			requested_options.push_back("god-connect");
		}
		// *TODO Remove
/*		std::vector<std::string> uris = LLViewerLogin::getInstance()->getLoginURIs();
		std::vector<std::string>::const_iterator iter, end;
		for (iter = uris.begin(), end = uris.end(); iter != end; ++iter)
		{
			std::vector<std::string> rewritten;
			rewritten = LLSRV::rewriteURI(*iter);
			sAuthUris.insert(sAuthUris.end(),
							 rewritten.begin(), rewritten.end());
		}
		sAuthUriNum = 0;
*/		auth_method = "login_to_simulator";
		
		LLStringUtil::format_map_t args;
		args["[APP_NAME]"] = LLAppViewer::instance()->getSecondLifeTitle();
		auth_desc = LLTrans::getString("LoginInProgress", args);
		LLStartUp::setStartupState( STATE_LOGIN_AUTHENTICATE );
	}

	if (STATE_LOGIN_AUTHENTICATE == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "STATE_LOGIN_AUTHENTICATE" << LL_ENDL;
		set_startup_status(progress, auth_desc, auth_message);
		progress += 0.02f;
		display_startup();
		
		std::stringstream start;
		if (LLURLSimString::parse())
		{
			// a startup URL was specified
			std::stringstream unescaped_start;
			unescaped_start << "uri:" 
							<< LLURLSimString::sInstance.mSimName << "&" 
							<< LLURLSimString::sInstance.mX << "&" 
							<< LLURLSimString::sInstance.mY << "&" 
							<< LLURLSimString::sInstance.mZ;
			start << xml_escape_string(unescaped_start.str());
			
		}
		else if (gSavedSettings.getBOOL("LoginLastLocation"))
		{
			start << "last";
		}
		else
		{
			start << "home";
		}

		char hashed_mac_string[MD5HEX_STR_SIZE];		/* Flawfinder: ignore */
		LLMD5 hashed_mac;
		hashed_mac.update( gMACAddress, MAC_ADDRESS_BYTES );
		hashed_mac.finalize();
		hashed_mac.hex_digest(hashed_mac_string);
		
		LLViewerLogin* vl = LLViewerLogin::getInstance();
		std::string grid_uri = vl->getCurrentGridURI();
		
		llinfos << "Authenticating with " << grid_uri << llendl;

		// TODO if statement here to use web_login_key
		// sAuthUriNum = llclamp(sAuthUriNum, 0, (S32)sAuthUris.size()-1);
		LLUserAuth::getInstance()->authenticate(
			grid_uri,
			auth_method,
			firstname,
			lastname,			
			password, // web_login_key,
			start.str(),
			gSkipOptionalUpdate,
			gAcceptTOS,
			gAcceptCriticalMessage,
			gLastExecEvent,
			requested_options,
			hashed_mac_string,
			LLAppViewer::instance()->getSerialNumber());

		// reset globals
		gAcceptTOS = FALSE;
		gAcceptCriticalMessage = FALSE;
		LLStartUp::setStartupState( STATE_LOGIN_NO_DATA_YET );
		return FALSE;
	}

	if(STATE_LOGIN_NO_DATA_YET == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "STATE_LOGIN_NO_DATA_YET" << LL_ENDL;
		// If we get here we have gotten past the potential stall
		// in curl, so take "may appear frozen" out of progress bar. JC
		auth_desc = "Logging in...";
		set_startup_status(progress, auth_desc, auth_message);
		// Process messages to keep from dropping circuit.
		LLMessageSystem* msg = gMessageSystem;
		while (msg->checkAllMessages(gFrameCount, gServicePump))
		{
		}
		msg->processAcks();
		LLUserAuth::UserAuthcode error = LLUserAuth::getInstance()->authResponse();
		if(LLUserAuth::E_NO_RESPONSE_YET == error)
		{
			LL_DEBUGS("AppInit") << "waiting..." << LL_ENDL;
			return FALSE;
		}
		LLStartUp::setStartupState( STATE_LOGIN_DOWNLOADING );
		progress += 0.01f;
		set_startup_status(progress, auth_desc, auth_message);
		return FALSE;
	}

	if(STATE_LOGIN_DOWNLOADING == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "STATE_LOGIN_DOWNLOADING" << LL_ENDL;
		// Process messages to keep from dropping circuit.
		LLMessageSystem* msg = gMessageSystem;
		while (msg->checkAllMessages(gFrameCount, gServicePump))
		{
		}
		msg->processAcks();
		LLUserAuth::UserAuthcode error = LLUserAuth::getInstance()->authResponse();
		if(LLUserAuth::E_DOWNLOADING == error)
		{
			LL_DEBUGS("AppInit") << "downloading..." << LL_ENDL;
			return FALSE;
		}
		LLStartUp::setStartupState( STATE_LOGIN_PROCESS_RESPONSE );
		progress += 0.01f;
		set_startup_status(progress, LLTrans::getString("LoginProcessingResponse"), auth_message);
		return FALSE;
	}

	if(STATE_LOGIN_PROCESS_RESPONSE == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "STATE_LOGIN_PROCESS_RESPONSE" << LL_ENDL;
		std::ostringstream emsg;
		bool quit = false;
		bool update = false;
		std::string login_response;
		std::string reason_response;
		std::string message_response;
		bool successful_login = false;
		LLUserAuth::UserAuthcode error = LLUserAuth::getInstance()->authResponse();
		// reset globals
		gAcceptTOS = FALSE;
		gAcceptCriticalMessage = FALSE;
		switch(error)
		{
		case LLUserAuth::E_OK:
			login_response = LLUserAuth::getInstance()->getResponse("login");
			if(login_response == "true")
			{
				// Yay, login!
				successful_login = true;
			}
			else if(login_response == "indeterminate")
			{
				LL_INFOS("AppInit") << "Indeterminate login..." << LL_ENDL;
				LLViewerLogin::getInstance()->setGridURIs(LLSRV::rewriteURI(LLUserAuth::getInstance()->getResponse("next_url")));
				
				auth_method = LLUserAuth::getInstance()->getResponse("next_method");
				auth_message = LLUserAuth::getInstance()->getResponse("message");
				if(auth_method.substr(0, 5) == "login")
				{
					auth_desc.assign(LLTrans::getString("LoginAuthenticating"));
				}
				else
				{
					auth_desc.assign(LLTrans::getString("LoginMaintenance"));
				}
				// ignoring the duration & options array for now.
				// Go back to authenticate.
				LLStartUp::setStartupState( STATE_LOGIN_AUTHENTICATE );
				return FALSE;
			}
			else
			{
				emsg << "Login failed.\n";
				reason_response = LLUserAuth::getInstance()->getResponse("reason");
				message_response = LLUserAuth::getInstance()->getResponse("message");

				if (!message_response.empty())
				{
					// XUI: fix translation for strings returned during login
					// We need a generic table for translations
					std::string big_reason = LLAgent::sTeleportErrorMessages[ message_response ];
					if ( big_reason.size() == 0 )
					{
						emsg << message_response;
					}
					else
					{
						emsg << big_reason;
					}
				}

				if(reason_response == "tos")
				{
					if (show_connect_box)
					{
						LL_DEBUGS("AppInit") << "Need tos agreement" << LL_ENDL;
						LLStartUp::setStartupState( STATE_UPDATE_CHECK );
						LLFloaterTOS* tos_dialog = LLFloaterTOS::show(LLFloaterTOS::TOS_TOS,
																	message_response);
						tos_dialog->startModal();
						// LLFloaterTOS deletes itself.
						return false;
					}
					else
					{
						quit = true;
					}
				}
				if(reason_response == "critical")
				{
					if (show_connect_box)
					{
						LL_DEBUGS("AppInit") << "Need critical message" << LL_ENDL;
						LLStartUp::setStartupState( STATE_UPDATE_CHECK );
						LLFloaterTOS* tos_dialog = LLFloaterTOS::show(LLFloaterTOS::TOS_CRITICAL_MESSAGE,
																	message_response);
						tos_dialog->startModal();
						// LLFloaterTOS deletes itself.
						return false;
					}
					else
					{
						quit = true;
					}
				}
				if(reason_response == "key")
				{
					// Couldn't login because user/password is wrong
					// Clear the password
					password = "";
				}
				if(reason_response == "update")
				{
					auth_message = LLUserAuth::getInstance()->getResponse("message");
					update = true;
				}
				if(reason_response == "optional")
				{
					LL_DEBUGS("AppInit") << "Login got optional update" << LL_ENDL;
					auth_message = LLUserAuth::getInstance()->getResponse("message");
					if (show_connect_box)
					{
						update_app(FALSE, auth_message);
						LLStartUp::setStartupState( STATE_UPDATE_CHECK );
						gSkipOptionalUpdate = TRUE;
						return false;
					}
				}
			}
			break;
		case LLUserAuth::E_COULDNT_RESOLVE_HOST:
		case LLUserAuth::E_SSL_PEER_CERTIFICATE:
		case LLUserAuth::E_UNHANDLED_ERROR:
		case LLUserAuth::E_SSL_CACERT:
		case LLUserAuth::E_SSL_CONNECT_ERROR:
		default:
			if (LLViewerLogin::getInstance()->tryNextURI())
			{
				static int login_attempt_number = 0;
				std::ostringstream s;
				LLStringUtil::format_map_t args;
				args["[NUMBER]"] = llformat("%d", ++login_attempt_number);
				auth_desc = LLTrans::getString("LoginAttempt", args);
				LLStartUp::setStartupState( STATE_LOGIN_AUTHENTICATE );
				return FALSE;
			}
			else
			{
				emsg << "Unable to connect to " << LLAppViewer::instance()->getSecondLifeTitle() << ".\n";
				emsg << LLUserAuth::getInstance()->errorMessage();
			}
			break;
		}

		if (update || gSavedSettings.getBOOL("ForceMandatoryUpdate"))
		{
			gSavedSettings.setBOOL("ForceMandatoryUpdate", FALSE);
			if (show_connect_box)
			{
				update_app(TRUE, auth_message);
				LLStartUp::setStartupState( STATE_UPDATE_CHECK );
				return false;
			}
			else
			{
				quit = true;
			}
		}

		// Version update and we're not showing the dialog
		if(quit)
		{
			LLUserAuth::getInstance()->reset();
			LLAppViewer::instance()->forceQuit();
			return false;
		}

		if(successful_login)
		{
			std::string text;
			text = LLUserAuth::getInstance()->getResponse("udp_blacklist");
			if(!text.empty())
			{
				apply_udp_blacklist(text);
			}

			// unpack login data needed by the application
			text = LLUserAuth::getInstance()->getResponse("agent_id");
			if(!text.empty()) gAgentID.set(text);
			gDebugInfo["AgentID"] = text;
			
			text = LLUserAuth::getInstance()->getResponse("session_id");
			if(!text.empty()) gAgentSessionID.set(text);
			gDebugInfo["SessionID"] = text;
			
			text = LLUserAuth::getInstance()->getResponse("secure_session_id");
			if(!text.empty()) gAgent.mSecureSessionID.set(text);

			text = LLUserAuth::getInstance()->getResponse("first_name");
			if(!text.empty()) 
			{
				// Remove quotes from string.  Login.cgi sends these to force
				// names that look like numbers into strings.
				firstname.assign(text);
				LLStringUtil::replaceChar(firstname, '"', ' ');
				LLStringUtil::trim(firstname);
			}
			text = LLUserAuth::getInstance()->getResponse("last_name");
			if(!text.empty()) lastname.assign(text);
			gSavedSettings.setString("FirstName", firstname);
			gSavedSettings.setString("LastName", lastname);

			if (gSavedSettings.getBOOL("RememberPassword"))
			{
				// Successful login means the password is valid, so save it.
				LLStartUp::savePasswordToDisk(password);
			}
			else
			{
				// Don't leave password from previous session sitting around
				// during this login session.
				LLStartUp::deletePasswordFromDisk();
				password.assign(""); // clear the password so it isn't saved to login history either
			}
			
			{
				// Save the login history data to disk
				std::string history_file = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "saved_logins.xml");

				LLSavedLogins history_data = LLSavedLogins::loadFile(history_file);
				LLViewerLogin* login_data = LLViewerLogin::getInstance();
				EGridInfo grid_choice = login_data->getGridChoice();
				history_data.deleteEntry(grid_choice, firstname, lastname);
				if (gSavedSettings.getBOOL("RememberLogin"))
				{
					LLSavedLoginEntry login_entry(grid_choice, firstname, lastname, password);
					if (grid_choice == GRID_INFO_OTHER)
					{
						std::string grid_uri = login_data->getCurrentGridURI();
						std::string login_uri = login_data->getLoginPageURI();
						std::string helper_uri = login_data->getHelperURI();
						
						if (!grid_uri.empty()) login_entry.setGridURI(LLURI(grid_uri));
						if (!login_uri.empty()) login_entry.setLoginPageURI(LLURI(login_uri));
						if (!helper_uri.empty()) login_entry.setHelperURI(LLURI(helper_uri));
					}
					history_data.addEntry(login_entry);
				}
				else
				{
					// Clear the old-style login data as well
					gSavedSettings.setString("FirstName", std::string(""));
					gSavedSettings.setString("LastName", std::string(""));
				}
				
				LLSavedLogins::saveFile(history_data, history_file);
			}
			
			// this is their actual ability to access content
			text = LLUserAuth::getInstance()->getResponse("agent_access_max");
			if (!text.empty())
			{
				// agent_access can be 'A', 'M', and 'PG'.
				gAgent.setMaturity(text[0]);
			}
			
			// this is the value of their preference setting for that content
			// which will always be <= agent_access_max
			text = LLUserAuth::getInstance()->getResponse("agent_region_access");
			if (!text.empty())
			{
				int preferredMaturity = LLAgent::convertTextToMaturity(text[0]);
				gSavedSettings.setU32("PreferredMaturity", preferredMaturity);
			}
			// During the AO transition, this flag will be true. Then the flag will
			// go away. After the AO transition, this code and all the code that
			// uses it can be deleted.
			text = LLUserAuth::getInstance()->getResponse("ao_transition");
			if (!text.empty())
			{
				if (text == "1")
				{
					gAgent.setAOTransition();
				}
			}

			text = LLUserAuth::getInstance()->getResponse("start_location");
			if(!text.empty()) agent_start_location.assign(text);
			text = LLUserAuth::getInstance()->getResponse("circuit_code");
			if(!text.empty())
			{
				gMessageSystem->mOurCircuitCode = strtoul(text.c_str(), NULL, 10);
			}
			std::string sim_ip_str = LLUserAuth::getInstance()->getResponse("sim_ip");
			std::string sim_port_str = LLUserAuth::getInstance()->getResponse("sim_port");
			if(!sim_ip_str.empty() && !sim_port_str.empty())
			{
				U32 sim_port = strtoul(sim_port_str.c_str(), NULL, 10);
				first_sim.set(sim_ip_str, sim_port);
				if (first_sim.isOk())
				{
					gMessageSystem->enableCircuit(first_sim, TRUE);
				}
			}
			std::string region_x_str = LLUserAuth::getInstance()->getResponse("region_x");
			std::string region_y_str = LLUserAuth::getInstance()->getResponse("region_y");
			if(!region_x_str.empty() && !region_y_str.empty())
			{
				U32 region_x = strtoul(region_x_str.c_str(), NULL, 10);
				U32 region_y = strtoul(region_y_str.c_str(), NULL, 10);
				first_sim_handle = to_region_handle(region_x, region_y);
			}
			
			const std::string look_at_str = LLUserAuth::getInstance()->getResponse("look_at");
			if (!look_at_str.empty())
			{
				size_t len = look_at_str.size();
				LLMemoryStream mstr((U8*)look_at_str.c_str(), len);
				LLSD sd = LLSDSerialize::fromNotation(mstr, len);
				agent_start_look_at = ll_vector3_from_sd(sd);
			}

			text = LLUserAuth::getInstance()->getResponse("seed_capability");
			if (!text.empty()) first_sim_seed_cap = text;
						
			text = LLUserAuth::getInstance()->getResponse("seconds_since_epoch");
			if(!text.empty())
			{
				U32 server_utc_time = strtoul(text.c_str(), NULL, 10);
				if(server_utc_time)
				{
					time_t now = time(NULL);
					gUTCOffset = (server_utc_time - now);
				}
			}

			std::string home_location = LLUserAuth::getInstance()->getResponse("home");
			if(!home_location.empty())
			{
				size_t len = home_location.size();
				LLMemoryStream mstr((U8*)home_location.c_str(), len);
				LLSD sd = LLSDSerialize::fromNotation(mstr, len);
				S32 region_x = sd["region_handle"][0].asInteger();
				S32 region_y = sd["region_handle"][1].asInteger();
				U64 region_handle = to_region_handle(region_x, region_y);
				LLVector3 position = ll_vector3_from_sd(sd["position"]);
				gAgent.setHomePosRegion(region_handle, position);
			}

			gAgent.mMOTD.assign(LLUserAuth::getInstance()->getResponse("message"));
			LLUserAuth::options_t options;
			if(LLUserAuth::getInstance()->getOptions("inventory-root", options))
			{
				LLUserAuth::response_t::iterator it;
				it = options[0].find("folder_id");
				if(it != options[0].end())
				{
					gAgent.mInventoryRootID.set((*it).second);
					//gInventory.mock(gAgent.getInventoryRootID());
				}
			}

			options.clear();
			if(LLUserAuth::getInstance()->getOptions("login-flags", options))
			{
				LLUserAuth::response_t::iterator it;
				LLUserAuth::response_t::iterator no_flag = options[0].end();
				it = options[0].find("ever_logged_in");
				if(it != no_flag)
				{
					if((*it).second == "N") gAgent.setFirstLogin(TRUE);
					else gAgent.setFirstLogin(FALSE);
				}
				it = options[0].find("stipend_since_login");
				if(it != no_flag)
				{
					if((*it).second == "Y") stipend_since_login = true;
				}
				it = options[0].find("gendered");
				if(it != no_flag)
				{
					if((*it).second == "Y") gAgent.setGenderChosen(TRUE);
				}
				it = options[0].find("daylight_savings");
				if(it != no_flag)
				{
					if((*it).second == "Y")  gPacificDaylightTime = TRUE;
					else gPacificDaylightTime = FALSE;
				}
			}
			options.clear();
			if (LLUserAuth::getInstance()->getOptions("initial-outfit", options)
				&& !options.empty())
			{
				LLUserAuth::response_t::iterator it;
				LLUserAuth::response_t::iterator it_end = options[0].end();
				it = options[0].find("folder_name");
				if(it != it_end)
				{
					// Initial outfit is a folder in your inventory,
					// must be an exact folder-name match.
					sInitialOutfit = (*it).second;
				}
				it = options[0].find("gender");
				if (it != it_end)
				{
					sInitialOutfitGender = (*it).second;
				}
			}

			options.clear();
			if(LLUserAuth::getInstance()->getOptions("global-textures", options))
			{
				// Extract sun and moon texture IDs.  These are used
				// in the LLVOSky constructor, but I can't figure out
				// how to pass them in.  JC
				LLUserAuth::response_t::iterator it;
				LLUserAuth::response_t::iterator no_texture = options[0].end();
				it = options[0].find("sun_texture_id");
				if(it != no_texture)
				{
					gSunTextureID.set((*it).second);
				}
				it = options[0].find("moon_texture_id");
				if(it != no_texture)
				{
					gMoonTextureID.set((*it).second);
				}
				it = options[0].find("cloud_texture_id");
				if(it != no_texture)
				{
					gCloudTextureID.set((*it).second);
				}
			}


			// JC: gesture loading done below, when we have an asset system
			// in place.  Don't delete/clear user_credentials until then.

			if(gAgentID.notNull()
			   && gAgentSessionID.notNull()
			   && gMessageSystem->mOurCircuitCode
			   && first_sim.isOk()
			   && gAgent.mInventoryRootID.notNull())
			{
				LLStartUp::setStartupState( STATE_WORLD_INIT );
			}
			else
			{
				if (gNoRender)
				{
					LL_WARNS("AppInit") << "Bad login - missing return values" << LL_ENDL;
					LL_WARNS("AppInit") << emsg << LL_ENDL;
					exit(0);
				}
				// Bounce back to the login screen.
				LLSD args;
				args["ERROR_MESSAGE"] = emsg.str();
				LLNotifications::instance().add("ErrorMessage", args, LLSD(), login_alert_done);
				reset_login();
				gSavedSettings.setBOOL("AutoLogin", FALSE);
				show_connect_box = true;
			}
			
			// Pass the user information to the voice chat server interface.
			gVoiceClient->userAuthorized(firstname, lastname, gAgentID);
		}
		else // if(successful_login)
		{
			if (gNoRender)
			{
				LL_WARNS("AppInit") << "Failed to login!" << LL_ENDL;
				LL_WARNS("AppInit") << emsg << LL_ENDL;
				exit(0);
			}
			// Bounce back to the login screen.
			LLSD args;
			args["ERROR_MESSAGE"] = emsg.str();
			LLNotifications::instance().add("ErrorMessage", args, LLSD(), login_alert_done);
			reset_login();
			gSavedSettings.setBOOL("AutoLogin", FALSE);
			show_connect_box = true;
		}
		return FALSE;
	}

	//---------------------------------------------------------------------
	// World Init
	//---------------------------------------------------------------------
	if (STATE_WORLD_INIT == LLStartUp::getStartupState())
	{
		set_startup_status(0.40f, LLTrans::getString("LoginInitializingWorld"), gAgent.mMOTD);
		display_startup();
		// We should have an agent id by this point.
		llassert(!(gAgentID == LLUUID::null));

		// Finish agent initialization.  (Requires gSavedSettings, builds camera)
		gAgent.init();
		set_underclothes_menu_options();

		// Since we connected, save off the settings so the user doesn't have to
		// type the name/password again if we crash.
		gSavedSettings.saveToFile(gSavedSettings.getString("ClientSettingsFile"), TRUE);

		//
		// Initialize classes w/graphics stuff.
		//
		gImageList.doPrefetchImages();		
		LLSurface::initClasses();

		LLFace::initClass();

		LLDrawable::initClass();

		// init the shader managers
		LLPostProcess::initClass();
		LLWLParamManager::initClass();
		LLWaterParamManager::initClass();

		// RN: don't initialize VO classes in drone mode, they are too closely tied to rendering
		LLViewerObject::initVOClasses();

		display_startup();

		// This is where we used to initialize gWorldp. Original comment said:
		// World initialization must be done after above window init

		// User might have overridden far clip
		LLWorld::getInstance()->setLandFarClip( gAgent.mDrawDistance );

		// Before we create the first region, we need to set the agent's mOriginGlobal
		// This is necessary because creating objects before this is set will result in a
		// bad mPositionAgent cache.

		gAgent.initOriginGlobal(from_region_handle(first_sim_handle));

		LLWorld::getInstance()->addRegion(first_sim_handle, first_sim);

		LLViewerRegion *regionp = LLWorld::getInstance()->getRegionFromHandle(first_sim_handle);
		LL_INFOS("AppInit") << "Adding initial simulator " << regionp->getOriginGlobal() << LL_ENDL;
		
		regionp->setSeedCapability(first_sim_seed_cap);
		LL_DEBUGS("AppInit") << "Waiting for seed grant ...." << LL_ENDL;
		
		// Set agent's initial region to be the one we just created.
		gAgent.setRegion(regionp);

		// Set agent's initial position, which will be read by LLVOAvatar when the avatar
		// object is created.  I think this must be done after setting the region.  JC
		gAgent.setPositionAgent(agent_start_position_region);

		display_startup();
		LLStartUp::setStartupState( STATE_MULTIMEDIA_INIT );
		return FALSE;
	}


	//---------------------------------------------------------------------
	// Load QuickTime/GStreamer and other multimedia engines, can be slow.
	// Do it while we're waiting on the network for our seed capability. JC
	//---------------------------------------------------------------------
	if (STATE_MULTIMEDIA_INIT == LLStartUp::getStartupState())
	{
		LLStartUp::multimediaInit();
		LLStartUp::setStartupState( STATE_SEED_GRANTED_WAIT );
		return FALSE;
	}

	//---------------------------------------------------------------------
	// Wait for Seed Cap Grant
	//---------------------------------------------------------------------
	if(STATE_SEED_GRANTED_WAIT == LLStartUp::getStartupState())
	{
		return FALSE;
	}


	//---------------------------------------------------------------------
	// Seed Capability Granted
	// no newMessage calls should happen before this point
	//---------------------------------------------------------------------
	if (STATE_SEED_CAP_GRANTED == LLStartUp::getStartupState())
	{
		update_texture_fetch();

		if ( gViewerWindow != NULL)
		{	// This isn't the first logon attempt, so show the UI
			gViewerWindow->setNormalControlsVisible( TRUE );
		}	
		gLoginMenuBarView->setVisible( FALSE );
		gLoginMenuBarView->setEnabled( FALSE );

		LLRect window(0, gViewerWindow->getWindowHeight(), gViewerWindow->getWindowWidth(), 0);
		gViewerWindow->adjustControlRectanglesForFirstUse(window);

		if(gSavedSettings.getBOOL("ShowMiniMap"))
		{
			LLFloaterMap::showInstance();
		}

		if (gSavedSettings.getBOOL("ShowCameraControls"))
		{
			LLFloaterCamera::showInstance();
		}
		if (gSavedSettings.getBOOL("ShowMovementControls"))
		{
			LLFloaterMove::showInstance();
		}

		if (gSavedSettings.getBOOL("ShowActiveSpeakers"))
		{
			LLFloaterActiveSpeakers::showInstance();
		}

		if (gSavedSettings.getBOOL("BeaconAlwaysOn"))
		{
			LLFloaterBeacons::showInstance();
		}

		if (gSavedSettings.getBOOL("ShowAvatarList"))
		{
			LLFloaterAvatarList::showInstance();
		}
		else if (gSavedSettings.getBOOL("EmeraldAvatarListKeepOpen"))
		{
			LLFloaterAvatarList::showInstance();
			LLFloaterAvatarList::toggle(NULL);
		}

		if (!gNoRender)
		{
			// Move the progress view in front of the UI
			gViewerWindow->moveProgressViewToFront();

			LLError::logToFixedBuffer(gDebugView->mDebugConsolep);
			// set initial visibility of debug console
			gDebugView->mDebugConsolep->setVisible(gSavedSettings.getBOOL("ShowDebugConsole"));
			if (gSavedSettings.getBOOL("ShowDebugStats"))
			{
				LLFloaterStats::showInstance();
			}
		}

		//
		// Set message handlers
		//
		LL_INFOS("AppInit") << "Initializing communications..." << LL_ENDL;

		// register callbacks for messages. . . do this after initial handshake to make sure that we don't catch any unwanted
		register_viewer_callbacks(gMessageSystem);

		// Debugging info parameters
		gMessageSystem->setMaxMessageTime( 0.5f );			// Spam if decoding all msgs takes more than 500 ms

		#ifndef	LL_RELEASE_FOR_DOWNLOAD
			gMessageSystem->setTimeDecodes( TRUE );				// Time the decode of each msg
			gMessageSystem->setTimeDecodesSpamThreshold( 0.05f );  // Spam if a single msg takes over 50ms to decode
		#endif

		gXferManager->registerCallbacks(gMessageSystem);

		if ( gCacheName == NULL )
		{
			gCacheName = new LLCacheName(gMessageSystem);
			gCacheName->addObserver(callback_cache_name);
	
			// Load stored cache if possible
            LLAppViewer::instance()->loadNameCache();
		}

		// *Note: this is where gWorldMap used to be initialized.

		// register null callbacks for audio until the audio system is initialized
		gMessageSystem->setHandlerFuncFast(_PREHASH_SoundTrigger, null_message_callback, NULL);
		gMessageSystem->setHandlerFuncFast(_PREHASH_AttachedSound, null_message_callback, NULL);

		//reset statistics
		LLViewerStats::getInstance()->resetStats();

		if (!gNoRender)
		{
			//
			// Set up all of our statistics UI stuff.
			//
			init_stat_view();
		}

		display_startup();
		//
		// Set up region and surface defaults
		//


		// Sets up the parameters for the first simulator

		LL_DEBUGS("AppInit") << "Initializing camera..." << LL_ENDL;
		gFrameTime    = totalTime();
		F32 last_time = gFrameTimeSeconds;
		gFrameTimeSeconds = (S64)(gFrameTime - gStartTime)/SEC_TO_MICROSEC;

		gFrameIntervalSeconds = gFrameTimeSeconds - last_time;
		if (gFrameIntervalSeconds < 0.f)
		{
			gFrameIntervalSeconds = 0.f;
		}

		// Make sure agent knows correct aspect ratio
		// FOV limits depend upon aspect ratio so this needs to happen before initializing the FOV below
		LLViewerCamera::getInstance()->setViewHeightInPixels(gViewerWindow->getWindowDisplayHeight());
		if (gViewerWindow->mWindow->getFullscreen())
		{
			LLViewerCamera::getInstance()->setAspect(gViewerWindow->getDisplayAspectRatio());
		}
		else
		{
			LLViewerCamera::getInstance()->setAspect( (F32) gViewerWindow->getWindowWidth() / (F32) gViewerWindow->getWindowHeight());
		}
		// Initialize FOV
		LLViewerCamera::getInstance()->setDefaultFOV(gSavedSettings.getF32("CameraAngle")); 

		// Move agent to starting location. The position handed to us by
		// the space server is in global coordinates, but the agent frame
		// is in region local coordinates. Therefore, we need to adjust
		// the coordinates handed to us to fit in the local region.

		gAgent.setPositionAgent(agent_start_position_region);
		gAgent.resetAxes(agent_start_look_at);
		gAgent.stopCameraAnimation();
		gAgent.resetCamera();

		// Initialize global class data needed for surfaces (i.e. textures)
		if (!gNoRender)
		{
			LL_DEBUGS("AppInit") << "Initializing sky..." << LL_ENDL;
			// Initialize all of the viewer object classes for the first time (doing things like texture fetches.
			LLGLState::checkStates();
			LLGLState::checkTextureChannels();

			gSky.init(initial_sun_direction);

			LLGLState::checkStates();
			LLGLState::checkTextureChannels();
		}

		LL_DEBUGS("AppInit") << "Decoding images..." << LL_ENDL;
		// For all images pre-loaded into viewer cache, decode them.
		// Need to do this AFTER we init the sky
		const S32 DECODE_TIME_SEC = 2;
		for (int i = 0; i < DECODE_TIME_SEC; i++)
		{
			F32 frac = (F32)i / (F32)DECODE_TIME_SEC;
			set_startup_status(0.45f + frac*0.1f, LLTrans::getString("LoginDecodingImages"), gAgent.mMOTD);
			display_startup();
			gImageList.decodeAllImages(1.f);
		}
		LLStartUp::setStartupState( STATE_WORLD_WAIT );

		// JC - Do this as late as possible to increase likelihood Purify
		// will run.
		LLMessageSystem* msg = gMessageSystem;
		if (!msg->mOurCircuitCode)
		{
			LL_WARNS("AppInit") << "Attempting to connect to simulator with a zero circuit code!" << LL_ENDL;
		}

		gUseCircuitCallbackCalled = FALSE;

		msg->enableCircuit(first_sim, TRUE);
		// now, use the circuit info to tell simulator about us!
		LL_INFOS("AppInit") << "viewer: UserLoginLocationReply() Enabling " << first_sim << " with code " << msg->mOurCircuitCode << LL_ENDL;
		msg->newMessageFast(_PREHASH_UseCircuitCode);
		msg->nextBlockFast(_PREHASH_CircuitCode);
		msg->addU32Fast(_PREHASH_Code, msg->mOurCircuitCode);
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->addUUIDFast(_PREHASH_ID, gAgent.getID());
		msg->sendReliable(
			first_sim,
			MAX_TIMEOUT_COUNT,
			FALSE,
			TIMEOUT_SECONDS,
			use_circuit_callback,
			NULL);

		timeout.reset();

		return FALSE;
	}

	//---------------------------------------------------------------------
	// Agent Send
	//---------------------------------------------------------------------
	if(STATE_WORLD_WAIT == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "Waiting for simulator ack...." << LL_ENDL;
		set_startup_status(0.59f, LLTrans::getString("LoginWaitingForRegionHandshake"), gAgent.mMOTD);
		if(gGotUseCircuitCodeAck)
		{
			LLStartUp::setStartupState( STATE_AGENT_SEND );
		}
		LLMessageSystem* msg = gMessageSystem;
		while (msg->checkAllMessages(gFrameCount, gServicePump))
		{
		}
		msg->processAcks();
		return FALSE;
	}

	//---------------------------------------------------------------------
	// Agent Send
	//---------------------------------------------------------------------
	if (STATE_AGENT_SEND == LLStartUp::getStartupState())
	{
		LL_DEBUGS("AppInit") << "Connecting to region..." << LL_ENDL;
		set_startup_status(0.60f, LLTrans::getString("LoginConnectingToRegion"), gAgent.mMOTD);
		// register with the message system so it knows we're
		// expecting this message
		LLMessageSystem* msg = gMessageSystem;
		msg->setHandlerFuncFast(
			_PREHASH_AgentMovementComplete,
			process_agent_movement_complete);
		LLViewerRegion* regionp = gAgent.getRegion();
		if(regionp)
		{
			send_complete_agent_movement(regionp->getHost());
			gAssetStorage->setUpstream(regionp->getHost());
			gCacheName->setUpstream(regionp->getHost());
			msg->newMessageFast(_PREHASH_EconomyDataRequest);
			gAgent.sendReliableMessage();
		}

		// Create login effect
		// But not on first login, because you can't see your avatar then
		if (!gAgent.isFirstLogin())
		{
			LLHUDEffectSpiral *effectp = (LLHUDEffectSpiral *)LLHUDManager::getInstance()->createViewerEffect(LLHUDObject::LL_HUD_EFFECT_POINT, TRUE);
			effectp->setPositionGlobal(gAgent.getPositionGlobal());
			effectp->setColor(LLColor4U(gAgent.getEffectColor()));
			LLHUDManager::getInstance()->sendEffects();
		}

		LLStartUp::setStartupState( STATE_AGENT_WAIT );		// Go to STATE_AGENT_WAIT

		timeout.reset();
		return FALSE;
	}

	//---------------------------------------------------------------------
	// Agent Wait
	//---------------------------------------------------------------------
	if (STATE_AGENT_WAIT == LLStartUp::getStartupState())
	{
		LLMessageSystem* msg = gMessageSystem;
		while (msg->checkAllMessages(gFrameCount, gServicePump))
		{
			if (gAgentMovementCompleted)
			{
				// Sometimes we have more than one message in the
				// queue. break out of this loop and continue
				// processing. If we don't, then this could skip one
				// or more login steps.
				break;
			}
			else
			{
				LL_DEBUGS("AppInit") << "Awaiting AvatarInitComplete, got "
				<< msg->getMessageName() << LL_ENDL;
			}
		}
		msg->processAcks();

		if (gAgentMovementCompleted)
		{
			LLStartUp::setStartupState( STATE_INVENTORY_SEND );
		}

		return FALSE;
	}

	//---------------------------------------------------------------------
	// Inventory Send
	//---------------------------------------------------------------------
	if (STATE_INVENTORY_SEND == LLStartUp::getStartupState())
	{
		// unpack thin inventory
		LLUserAuth::options_t options;
		options.clear();
		//bool dump_buffer = false;
		
		if(LLUserAuth::getInstance()->getOptions("inventory-lib-root", options)
			&& !options.empty())
		{
			// should only be one
			LLUserAuth::response_t::iterator it;
			it = options[0].find("folder_id");
			if(it != options[0].end())
			{
				gInventoryLibraryRoot.set((*it).second);
			}
		}
 		options.clear();
		if(LLUserAuth::getInstance()->getOptions("inventory-lib-owner", options)
			&& !options.empty())
		{
			// should only be one
			LLUserAuth::response_t::iterator it;
			it = options[0].find("agent_id");
			if(it != options[0].end())
			{
				gInventoryLibraryOwner.set((*it).second);
			}
		}
 		options.clear();
 		if(LLUserAuth::getInstance()->getOptions("inventory-skel-lib", options)
			&& gInventoryLibraryOwner.notNull())
 		{
 			if(!gInventory.loadSkeleton(options, gInventoryLibraryOwner))
 			{
 				LL_WARNS("AppInit") << "Problem loading inventory-skel-lib" << LL_ENDL;
 			}
 		}
 		options.clear();
 		if(LLUserAuth::getInstance()->getOptions("inventory-skeleton", options))
 		{
 			if(!gInventory.loadSkeleton(options, gAgent.getID()))
 			{
 				LL_WARNS("AppInit") << "Problem loading inventory-skel-targets" << LL_ENDL;
 			}
 		}

		options.clear();
 		if(LLUserAuth::getInstance()->getOptions("buddy-list", options))
 		{
			LLUserAuth::options_t::iterator it = options.begin();
			LLUserAuth::options_t::iterator end = options.end();
			LLAvatarTracker::buddy_map_t list;
			LLUUID agent_id;
			S32 has_rights = 0, given_rights = 0;
			for (; it != end; ++it)
			{
				LLUserAuth::response_t::const_iterator option_it;
				option_it = (*it).find("buddy_id");
				if(option_it != (*it).end())
				{
					agent_id.set((*option_it).second);
				}
				option_it = (*it).find("buddy_rights_has");
				if(option_it != (*it).end())
				{
					has_rights = atoi((*option_it).second.c_str());
				}
				option_it = (*it).find("buddy_rights_given");
				if(option_it != (*it).end())
				{
					given_rights = atoi((*option_it).second.c_str());
				}
				list[agent_id] = new LLRelationship(given_rights, has_rights, false);
			}
			LLAvatarTracker::instance().addBuddyList(list);
 		}

		options.clear();
 		if(LLUserAuth::getInstance()->getOptions("ui-config", options))
 		{
			LLUserAuth::options_t::iterator it = options.begin();
			LLUserAuth::options_t::iterator end = options.end();
			for (; it != end; ++it)
			{
				LLUserAuth::response_t::const_iterator option_it;
				option_it = (*it).find("allow_first_life");
				if(option_it != (*it).end())
				{
					if (option_it->second == "Y")
					{
						LLPanelAvatar::sAllowFirstLife = TRUE;
					}
				}
			}
 		}
		options.clear();
		bool show_hud = false;
		if(LLUserAuth::getInstance()->getOptions("tutorial_setting", options))
		{
			LLUserAuth::options_t::iterator it = options.begin();
			LLUserAuth::options_t::iterator end = options.end();
			for (; it != end; ++it)
			{
				LLUserAuth::response_t::const_iterator option_it;
				option_it = (*it).find("tutorial_url");
				if(option_it != (*it).end())
				{
					// Tutorial floater will append language code
					gSavedSettings.setString("TutorialURL", option_it->second);
				}
				option_it = (*it).find("use_tutorial");
				if(option_it != (*it).end())
				{
					if (option_it->second == "true")
					{
						show_hud = true;
					}
				}
			}
		}
		// Either we want to show tutorial because this is the first login
		// to a Linden Help Island or the user quit with the tutorial
		// visible.  JC
		if (show_hud
			|| gSavedSettings.getBOOL("ShowTutorial"))
		{
			LLFloaterHUD::showHUD();
		}

		options.clear();
		if(LLUserAuth::getInstance()->getOptions("event_categories", options))
		{
			LLEventInfo::loadCategories(options);
		}
		if(LLUserAuth::getInstance()->getOptions("event_notifications", options))
		{
			gEventNotifier.load(options);
		}
		options.clear();
		if(LLUserAuth::getInstance()->getOptions("classified_categories", options))
		{
			LLClassifiedInfo::loadCategories(options);
		}
		gInventory.buildParentChildMap();

		llinfos << "Setting Inventory changed mask and notifying observers" << llendl;
		gInventory.addChangedMask(LLInventoryObserver::ALL, LLUUID::null);
		gInventory.notifyObservers();

		// set up callbacks
		llinfos << "Registering Callbacks" << llendl;
		LLMessageSystem* msg = gMessageSystem;
		llinfos << " Inventory" << llendl;
		LLInventoryModel::registerCallbacks(msg);
		llinfos << " AvatarTracker" << llendl;
		LLAvatarTracker::instance().registerCallbacks(msg);
		llinfos << " Landmark" << llendl;
		LLLandmark::registerCallbacks(msg);

		// request mute list
		llinfos << "Requesting Mute List" << llendl;
		LLMuteList::getInstance()->requestFromServer(gAgent.getID());

		// Get L$ and ownership credit information
		llinfos << "Requesting Money Balance" << llendl;
		LLStatusBar::sendMoneyBalanceRequest();

		// request all group information
		llinfos << "Requesting Agent Data" << llendl;
		gAgent.sendAgentDataUpdateRequest();

		bool shown_at_exit = gSavedSettings.getBOOL("ShowInventory");

		// Create the inventory views
		llinfos << "Creating Inventory Views" << llendl;
		LLInventoryView::showAgentInventory();

		// Hide the inventory if it wasn't shown at exit
		if(!shown_at_exit)
		{
			LLInventoryView::toggleVisibility(NULL);
		}

		LLStartUp::setStartupState( STATE_MISC );
		return FALSE;
	}


	//---------------------------------------------------------------------
	// Misc
	//---------------------------------------------------------------------
	if (STATE_MISC == LLStartUp::getStartupState())
	{
		// We have a region, and just did a big inventory download.
		// We can estimate the user's connection speed, and set their
		// max bandwidth accordingly.  JC
		if (gSavedSettings.getBOOL("FirstLoginThisInstall"))
		{
			// This is actually a pessimistic computation, because TCP may not have enough
			// time to ramp up on the (small) default inventory file to truly measure max
			// bandwidth. JC
			F64 rate_bps = LLUserAuth::getInstance()->getLastTransferRateBPS();
			const F32 FAST_RATE_BPS = 600.f * 1024.f;
			const F32 FASTER_RATE_BPS = 750.f * 1024.f;
			F32 max_bandwidth = gViewerThrottle.getMaxBandwidth();
			if (rate_bps > FASTER_RATE_BPS
				&& rate_bps > max_bandwidth)
			{
				LL_DEBUGS("AppInit") << "Fast network connection, increasing max bandwidth to " 
					<< FASTER_RATE_BPS/1024.f 
					<< " kbps" << LL_ENDL;
				gViewerThrottle.setMaxBandwidth(FASTER_RATE_BPS / 1024.f);
			}
			else if (rate_bps > FAST_RATE_BPS
				&& rate_bps > max_bandwidth)
			{
				LL_DEBUGS("AppInit") << "Fast network connection, increasing max bandwidth to " 
					<< FAST_RATE_BPS/1024.f 
					<< " kbps" << LL_ENDL;
				gViewerThrottle.setMaxBandwidth(FAST_RATE_BPS / 1024.f);
			}
		}

		// We're successfully logged in.
		gSavedSettings.setBOOL("FirstLoginThisInstall", FALSE);


		// based on the comments, we've successfully logged in so we can delete the 'forced'
		// URL that the updater set in settings.ini (in a mostly paranoid fashion)
		std::string nextLoginLocation = gSavedSettings.getString( "NextLoginLocation" );
		if ( nextLoginLocation.length() )
		{
			// clear it
			gSavedSettings.setString( "NextLoginLocation", "" );

			// and make sure it's saved
			gSavedSettings.saveToFile( gSavedSettings.getString("ClientSettingsFile") , TRUE );
		};

		if (!gNoRender)
		{
			// JC: Initializing audio requests many sounds for download.
			init_audio();

			// Zwag: Moving shader init here. LL picked a dumb spot.
			// should fix all the shader loading problems.
			if (!LLViewerShaderMgr::sInitialized)
			{
				LLViewerShaderMgr::sInitialized = TRUE;
				LLViewerShaderMgr::instance()->setShaders();
			}

			// JC: Initialize "active" gestures.  This may also trigger
			// many gesture downloads, if this is the user's first
			// time on this machine or -purge has been run.
			LLUserAuth::options_t gesture_options;
			if (LLUserAuth::getInstance()->getOptions("gestures", gesture_options))
			{
				LL_DEBUGS("AppInit") << "Gesture Manager loading " << gesture_options.size()
					<< LL_ENDL;
				std::vector<LLUUID> item_ids;
				LLUserAuth::options_t::iterator resp_it;
				for (resp_it = gesture_options.begin();
					 resp_it != gesture_options.end();
					 ++resp_it)
				{
					const LLUserAuth::response_t& response = *resp_it;
					LLUUID item_id;
					LLUUID asset_id;
					LLUserAuth::response_t::const_iterator option_it;

					option_it = response.find("item_id");
					if (option_it != response.end())
					{
						const std::string& uuid_string = (*option_it).second;
						item_id.set(uuid_string);
					}
					option_it = response.find("asset_id");
					if (option_it != response.end())
					{
						const std::string& uuid_string = (*option_it).second;
						asset_id.set(uuid_string);
					}

					if (item_id.notNull() && asset_id.notNull())
					{
						// Could schedule and delay these for later.
						const BOOL no_inform_server = FALSE;
						const BOOL no_deactivate_similar = FALSE;
						gGestureManager.activateGestureWithAsset(item_id, asset_id,
											 no_inform_server,
											 no_deactivate_similar);
						// We need to fetch the inventory items for these gestures
						// so we have the names to populate the UI.
						item_ids.push_back(item_id);
					}
				}

				LLGestureInventoryFetchObserver* fetch = new LLGestureInventoryFetchObserver();
				fetch->fetchItems(item_ids);
				// deletes itself when done
				gInventory.addObserver(fetch);
			}
		}
		gDisplaySwapBuffers = TRUE;

		LLMessageSystem* msg = gMessageSystem;
		msg->setHandlerFuncFast(_PREHASH_SoundTrigger,				pass_process_sound_trigger);
		msg->setHandlerFuncFast(_PREHASH_PreloadSound,				process_preload_sound);
		msg->setHandlerFuncFast(_PREHASH_AttachedSound,				process_attached_sound);
		msg->setHandlerFuncFast(_PREHASH_AttachedSoundGainChange,	process_attached_sound_gain_change);


		LL_DEBUGS("AppInit") << "Initialization complete" << LL_ENDL;

		gRenderStartTime.reset();
		gForegroundTime.reset();

		// HACK: Inform simulator of window size.
		// Do this here so it's less likely to race with RegisterNewAgent.
		// TODO: Put this into RegisterNewAgent
		// JC - 7/20/2002
		gViewerWindow->sendShapeToSim();

		// Inform simulator of our language preference
		LLAgentLanguage::update();

		
		// Ignore stipend information for now.  Money history is on the web site.
		// if needed, show the L$ history window
		//if (stipend_since_login && !gNoRender)
		//{
		//}

		if (!gAgent.isFirstLogin())
		{
			bool url_ok = LLURLSimString::sInstance.parse();
			if (!((agent_start_location == "url" && url_ok) ||
                  (!url_ok && ((agent_start_location == "last" && gSavedSettings.getBOOL("LoginLastLocation")) ||
							   (agent_start_location == "home" && !gSavedSettings.getBOOL("LoginLastLocation"))))))
			{
				// The reason we show the alert is because we want to
				// reduce confusion for when you log in and your provided
				// location is not your expected location. So, if this is
				// your first login, then you do not have an expectation,
				// thus, do not show this alert.
				LLSD args;
				if (url_ok)
				{
					args["TYPE"] = "desired";
					args["HELP"] = "";
				}
				else if (gSavedSettings.getBOOL("LoginLastLocation"))
				{
					args["TYPE"] = "last";
					args["HELP"] = "";
				}
				else
				{
					args["TYPE"] = "home";
					args["HELP"] = "You may want to set a new home location.";
				}
				LLNotifications::instance().add("AvatarMoved", args);
			}
			else
			{
				if (samename)
				{
					// restore old camera pos
					gAgent.setFocusOnAvatar(FALSE, FALSE);
					gAgent.setCameraPosAndFocusGlobal(gSavedSettings.getVector3d("CameraPosOnLogout"), gSavedSettings.getVector3d("FocusPosOnLogout"), LLUUID::null);
					BOOL limit_hit = FALSE;
					gAgent.calcCameraPositionTargetGlobal(&limit_hit);
					if (limit_hit)
					{
						gAgent.setFocusOnAvatar(TRUE, FALSE);
					}
					gAgent.stopCameraAnimation();
				}
			}
		}

        //DEV-17797.  get null folder.  Any items found here moved to Lost and Found
        LLInventoryModel::findLostItems();

		LLStartUp::setStartupState( STATE_PRECACHE );
		timeout.reset();
		return FALSE;
	}

	if (STATE_PRECACHE == LLStartUp::getStartupState())
	{
		F32 timeout_frac = timeout.getElapsedTimeF32()/(gSavedSettings.getF32("PrecachingDelay")+0.1f);

		// We now have an inventory skeleton, so if this is a user's first
		// login, we can start setting up their clothing and avatar 
		// appearance.  This helps to avoid the generic "Ruth" avatar in
		// the orientation island tutorial experience. JC
		if (gAgent.isFirstLogin()
			&& !sInitialOutfit.empty()    // registration set up an outfit
			&& !sInitialOutfitGender.empty() // and a gender
			&& gAgent.getAvatarObject()	  // can't wear clothes without object
			&& !gAgent.isGenderChosen() ) // nothing already loading
		{
			// Start loading the wearables, textures, gestures
			LLStartUp::loadInitialOutfit( sInitialOutfit, sInitialOutfitGender );
		}


		// We now have an inventory skeleton, so if this is a user's first
		// login, we can start setting up their clothing and avatar 
		// appearance.  This helps to avoid the generic "Ruth" avatar in
		// the orientation island tutorial experience. JC
		if (gAgent.isFirstLogin()
			&& !sInitialOutfit.empty()    // registration set up an outfit
			&& !sInitialOutfitGender.empty() // and a gender
			&& gAgent.getAvatarObject()	  // can't wear clothes without object
			&& !gAgent.isGenderChosen() ) // nothing already loading
		{
			// Start loading the wearables, textures, gestures
			LLStartUp::loadInitialOutfit( sInitialOutfit, sInitialOutfitGender );
		}

		// wait precache-delay and for agent's avatar or a lot longer.
		if(((timeout_frac > 1.f) && gAgent.getAvatarObject())
		   || (timeout_frac > 3.f))
		{
			LLStartUp::setStartupState( STATE_WEARABLES_WAIT );
		}
		else
		{
			update_texture_fetch();
			set_startup_status(0.60f + 0.30f * timeout_frac,
				LLTrans::getString("LoginPrecaching"),
					gAgent.mMOTD);
			display_startup();
		}

		return TRUE;
	}

	if (STATE_WEARABLES_WAIT == LLStartUp::getStartupState())
	{
		static LLFrameTimer wearables_timer;

		const F32 wearables_time = wearables_timer.getElapsedTimeF32();
		const F32 MAX_WEARABLES_TIME = 10.f;

		if (!gAgent.isGenderChosen())
		{
			// No point in waiting for clothing, we don't even
			// know what gender we are.  Pop a dialog to ask and
			// proceed to draw the world. JC
			//
			// *NOTE: We might hit this case even if we have an
			// initial outfit, but if the load hasn't started
			// already then something is wrong so fall back
			// to generic outfits. JC
			LLNotifications::instance().add("WelcomeChooseSex", LLSD(), LLSD(),
				callback_choose_gender);
			LLStartUp::setStartupState( STATE_CLEANUP );
			return TRUE;
		}
		
		if (wearables_time > MAX_WEARABLES_TIME)
		{
			LLNotifications::instance().add("ClothingLoading");
			LLViewerStats::getInstance()->incStat(LLViewerStats::ST_WEARABLES_TOO_LONG);
			LLStartUp::setStartupState( STATE_CLEANUP );
			return TRUE;
		}

		if (gAgent.isFirstLogin())
		{
			// wait for avatar to be completely loaded
			if (gAgent.getAvatarObject()
				&& gAgent.getAvatarObject()->isFullyLoaded())
			{
				//llinfos << "avatar fully loaded" << llendl;
				LLStartUp::setStartupState( STATE_CLEANUP );
				return TRUE;
			}
		}
		else
		{
			// OK to just get the wearables
			if ( gAgent.areWearablesLoaded() )
			{
				// We have our clothing, proceed.
				//llinfos << "wearables loaded" << llendl;
				LLStartUp::setStartupState( STATE_CLEANUP );
				return TRUE;
			}
		}

		update_texture_fetch();
		set_startup_status(0.9f + 0.1f * wearables_time / MAX_WEARABLES_TIME,
						 LLTrans::getString("LoginDownloadingClothing").c_str(),
						 gAgent.mMOTD.c_str());
		return TRUE;
	}

	if (STATE_CLEANUP == LLStartUp::getStartupState())
	{
		set_startup_status(1.0, "", "");

		LLFirstUse::ClientTags();
		LLFirstUse::EmeraldOTR();
		LLFirstUse::EmeraldBridge();

		// Add login location to teleport history 'teleported-into'
		LLVector3 agent_pos=gAgent.getPositionAgent();
		LLViewerRegion* regionp = gAgent.getRegion();
		gFloaterTeleportHistory->addEntry(regionp->getName(),(S16)agent_pos.mV[0],(S16)agent_pos.mV[1],(S16)agent_pos.mV[2],false);

		// Let the map know about the inventory.
		if(gFloaterWorldMap)
		{
			gFloaterWorldMap->observeInventory(&gInventory);
			gFloaterWorldMap->observeFriends();
		}

		gViewerWindow->showCursor();
		gViewerWindow->getWindow()->resetBusyCount();
		gViewerWindow->getWindow()->setCursor(UI_CURSOR_ARROW);
		LL_DEBUGS("AppInit") << "Done releasing bitmap" << LL_ENDL;
		gViewerWindow->setShowProgress(FALSE);
		gViewerWindow->setProgressCancelButtonVisible(FALSE);

		// We're not away from keyboard, even though login might have taken
		// a while. JC
		gAgent.clearAFK();

		//lgg starting up auto connect irc things here
		//but that crashed.. so i duno
		glggIrcGroupHandler.startUpAutoRunIRC();

		// Have the agent start watching the friends list so we can update proxies
		gAgent.observeFriends();
		if (gSavedSettings.getBOOL("LoginAsGod"))
		{
			gAgent.requestEnterGodMode();
		}
		gInventory.startBackgroundFetch();
#if USE_OTR         // [$PLOTR$]
        OTR_Wrapper::init();
#endif // USE_OTR   // [/$PLOTR$]
		
		// Start automatic replay if the flag is set.
		if (gSavedSettings.getBOOL("StatsAutoRun"))
		{
			LLUUID id;
			LL_DEBUGS("AppInit") << "Starting automatic playback" << LL_ENDL;
			gAgentPilot.startPlayback();
		}

		// If we've got a startup URL, dispatch it
		LLStartUp::dispatchURL();

		// Retrieve information about the land data
		// (just accessing this the first time will fetch it,
		// then the data is cached for the viewer's lifetime)
		LLProductInfoRequestManager::instance();
		
		// Clean up the userauth stuff.
		LLUserAuth::getInstance()->reset();

		LLStartUp::setStartupState( STATE_STARTED );

		// Unmute audio if desired and setup volumes.
		// Unmute audio if desired and setup volumes.
		// This is a not-uncommon crash site, so surround it with
		// llinfos output to aid diagnosis.
		LL_INFOS("AppInit") << "Doing first audio_update_volume..." << LL_ENDL;
		audio_update_volume();
		LL_INFOS("AppInit") << "Done first audio_update_volume." << LL_ENDL;

		// reset keyboard focus to sane state of pointing at world
		gFocusMgr.setKeyboardFocus(NULL);
		//Fix this panel that refuses to obey the gods.
		wlfPanel_AdvSettings::fixPanel();

#if 0 // sjb: enable for auto-enabling timer display 
		gDebugView->mFastTimerView->setVisible(TRUE);
#endif

		LLAppViewer::instance()->handleLoginComplete();

		return TRUE;
	}

	LL_WARNS("AppInit") << "Reached end of idle_startup for state " << LLStartUp::getStartupState() << LL_ENDL;
	return TRUE;
}

//
// local function definition
//

void login_show()
{
	LL_INFOS("AppInit") << "Initializing Login Screen" << LL_ENDL;

#ifdef LL_RELEASE_FOR_DOWNLOAD
	BOOL bUseDebugLogin = gSavedSettings.getBOOL("UseDebugLogin");
#else
	BOOL bUseDebugLogin = TRUE;
#endif

	LLPanelLogin::show(	gViewerWindow->getVirtualWindowRect(),
						bUseDebugLogin,
						login_callback, NULL );

	// UI textures have been previously loaded in doPreloadImages()
					
	LL_DEBUGS("AppInit") << "Setting Servers" << LL_ENDL;

	LLViewerLogin* vl = LLViewerLogin::getInstance();
	LLPanelLogin::addServer(vl->getGridLabel(), vl->getGridChoice());

	for(int grid_index = GRID_INFO_ADITI; grid_index < GRID_INFO_OTHER; ++grid_index)
	{
		LLPanelLogin::addServer(vl->getKnownGridLabel((EGridInfo)grid_index), grid_index);
	}
}

// Callback for when login screen is closed.  Option 0 = connect, option 1 = quit.
void login_callback(S32 option, void *userdata)
{
	const S32 CONNECT_OPTION = 0;
	const S32 QUIT_OPTION = 1;

	if (CONNECT_OPTION == option)
	{
		LLStartUp::setStartupState( STATE_LOGIN_CLEANUP );
		return;
	}
	else if (QUIT_OPTION == option)
	{
		// Make sure we don't save the password if the user is trying to clear it.
		std::string first, last, password;
		LLPanelLogin::getFields(&first, &last, &password);
		if (!gSavedSettings.getBOOL("RememberPassword"))
		{
			// turn off the setting and write out to disk
			gSavedSettings.saveToFile( gSavedSettings.getString("ClientSettingsFile") , TRUE );
		}

		// Next iteration through main loop should shut down the app cleanly.
		LLAppViewer::instance()->userQuit();
		
		if (LLAppViewer::instance()->quitRequested())
		{
			LLPanelLogin::close();
		}
		return;
	}
	else
	{
		LL_WARNS("AppInit") << "Unknown login button clicked" << LL_ENDL;
	}
}


// static
std::string LLStartUp::loadPasswordFromDisk()
{
	// Only load password if we also intend to save it (otherwise the user
	// wonders what we're doing behind his back).  JC
	BOOL remember_password = gSavedSettings.getBOOL("RememberPassword");
	if (!remember_password)
	{
		return std::string("");
	}

	std::string hashed_password("");

	// Look for legacy "marker" password from settings.ini
	hashed_password = gSavedSettings.getString("Marker");
	if (!hashed_password.empty())
	{
		// Stomp the Marker entry.
		gSavedSettings.setString("Marker", "");

		// Return that password.
		return hashed_password;
	}

	std::string filepath = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS,
													   "password.dat");
	LLFILE* fp = LLFile::fopen(filepath, "rb");		/* Flawfinder: ignore */
	if (!fp)
	{
		return hashed_password;
	}

	// UUID is 16 bytes, written into ASCII is 32 characters
	// without trailing \0
	const S32 HASHED_LENGTH = 32;
	U8 buffer[HASHED_LENGTH+1];

	if (1 != fread(buffer, HASHED_LENGTH, 1, fp))
	{
		return hashed_password;
	}

	fclose(fp);

	// Decipher with MAC address
	LLXORCipher cipher(gMACAddress, 6);
	cipher.decrypt(buffer, HASHED_LENGTH);

	buffer[HASHED_LENGTH] = '\0';

	// Check to see if the mac address generated a bad hashed
	// password. It should be a hex-string or else the mac adress has
	// changed. This is a security feature to make sure that if you
	// get someone's password.dat file, you cannot hack their account.
	if(LLStringOps::isHexString(std::string(reinterpret_cast<const char*>(buffer), HASHED_LENGTH)))
	{
		hashed_password.assign((char*)buffer);
	}

	return hashed_password;
}


// static
void LLStartUp::savePasswordToDisk(const std::string& hashed_password)
{
	std::string filepath = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS,
													   "password.dat");
	LLFILE* fp = LLFile::fopen(filepath, "wb");		/* Flawfinder: ignore */
	if (!fp)
	{
		return;
	}

	// Encipher with MAC address
	const S32 HASHED_LENGTH = 32;
	U8 buffer[HASHED_LENGTH+1];

	LLStringUtil::copy((char*)buffer, hashed_password.c_str(), HASHED_LENGTH+1);

	LLXORCipher cipher(gMACAddress, 6);
	cipher.encrypt(buffer, HASHED_LENGTH);

	if (fwrite(buffer, HASHED_LENGTH, 1, fp) != 1)
	{
		LL_WARNS("AppInit") << "Short write" << LL_ENDL;
	}

	fclose(fp);
}


// static
void LLStartUp::deletePasswordFromDisk()
{
	std::string filepath = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS,
														  "password.dat");
	LLFile::remove(filepath);
}

void show_first_run_dialog()
{
	LLNotifications::instance().add("FirstRun", LLSD(), LLSD(), first_run_dialog_callback);
}

bool first_run_dialog_callback(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);
	if (0 == option)
	{
		LL_DEBUGS("AppInit") << "First run dialog cancelling" << LL_ENDL;
		LLWeb::loadURL( CREATE_ACCOUNT_URL );
	}

	LLPanelLogin::giveFocus();
	return false;
}


std::string last_d;
void set_startup_status(const F32 frac, const std::string& string, const std::string& msg)
{
	if(gSavedSettings.getBOOL("EmeraldDisableLoginScreens"))
	{
		std::string new_d = string;
		if(new_d != last_d)
		{
			last_d = new_d;
			LLChat chat;
			chat.mText = new_d;
			chat.mSourceType = (EChatSourceType)(CHAT_SOURCE_OBJECT+1);
			LLFloaterChat::addChat(chat);
			if(new_d == LLTrans::getString("LoginWaitingForRegionHandshake"))
			{
				chat.mText = "MOTD: "+msg;
				chat.mSourceType = (EChatSourceType)(CHAT_SOURCE_OBJECT+1);
				LLFloaterChat::addChat(chat);
			}
		}
	}else
	{
		gViewerWindow->setProgressPercent(frac*100);
		gViewerWindow->setProgressString(string);

		gViewerWindow->setProgressMessage(msg);
	}
}

bool login_alert_status(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);
    // Buttons
    switch( option )
    {
        case 0:     // OK
            break;
        case 1:     // Help
            LLWeb::loadURL( SUPPORT_URL );
            break;
        case 2:     // Teleport
            // Restart the login process, starting at our home locaton
            LLURLSimString::setString(LLURLSimString::sLocationStringHome);
            LLStartUp::setStartupState( STATE_LOGIN_CLEANUP );
            break;
        default:
            LL_WARNS("AppInit") << "Missing case in login_alert_status switch" << LL_ENDL;
    }

	LLPanelLogin::giveFocus();
	return false;
}

void update_app(BOOL mandatory, const std::string& auth_msg)
{
	// store off config state, as we might quit soon
	gSavedSettings.saveToFile(gSavedSettings.getString("ClientSettingsFile"), TRUE);	

	std::ostringstream message;

	//*TODO:translate
	std::string msg;
	if (!auth_msg.empty())
	{
		msg = "(" + auth_msg + ") \n";
	}

	LLSD args;
	args["MESSAGE"] = msg;
	
	LLSD payload;
	payload["mandatory"] = mandatory;

/*
 We're constructing one of the following 6 strings here:
	 "DownloadWindowsMandatory"
	 "DownloadWindowsReleaseForDownload"
	 "DownloadWindows"
	 "DownloadMacMandatory"
	 "DownloadMacReleaseForDownload"
	 "DownloadMac"
 
 I've called them out explicitly in this comment so that they can be grepped for.
 
 Also, we assume that if we're not Windows we're Mac. If we ever intend to support 
 Linux with autoupdate, this should be an explicit #elif LL_DARWIN, but 
 we'd rather deliver the wrong message than no message, so until Linux is supported
 we'll leave it alone.
 */
	std::string notification_name = "Download";
	
#if LL_WINDOWS
	notification_name += "Windows";
#else
	notification_name += "Mac";
#endif
	
	if (mandatory)
	{
		notification_name += "Mandatory";
	}
	else
	{
#if LL_RELEASE_FOR_DOWNLOAD
		notification_name += "ReleaseForDownload";
#endif
	}
	
	LLNotifications::instance().add(notification_name, args, payload, update_dialog_callback);
	
}

bool update_dialog_callback(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);
	std::string update_exe_path;
	bool mandatory = notification["payload"]["mandatory"].asBoolean();

#if !LL_RELEASE_FOR_DOWNLOAD
	if (option == 2)
	{
		LLStartUp::setStartupState( STATE_LOGIN_AUTH_INIT ); 
		return false;
	}
#endif

	if (option == 1)
	{
		// ...user doesn't want to do it
		if (mandatory)
		{
			LLAppViewer::instance()->forceQuit();
			// Bump them back to the login screen.
			//reset_login();
		}
		else
		{
			LLStartUp::setStartupState( STATE_LOGIN_AUTH_INIT );
		}
		return false;
	}
	
	LLSD query_map = LLSD::emptyMap();
	// *TODO place os string in a global constant
#if LL_WINDOWS  
	query_map["os"] = "win";
#elif LL_DARWIN
	query_map["os"] = "mac";
#elif LL_LINUX
	query_map["os"] = "lnx";
#elif LL_SOLARIS
	query_map["os"] = "sol";
#endif
	// *TODO change userserver to be grid on both viewer and sim, since
	// userserver no longer exists.
	query_map["userserver"] = LLViewerLogin::getInstance()->getGridLabel();
	query_map["channel"] = LL_CHANNEL;
	// *TODO constantize this guy
	// *NOTE: This URL is also used in win_setup/lldownloader.cpp
	LLURI update_url = LLURI::buildHTTP("secondlife.com", 80, "update.php", query_map);
	
	if(LLAppViewer::sUpdaterInfo)
	{
		delete LLAppViewer::sUpdaterInfo ;
	}
	LLAppViewer::sUpdaterInfo = new LLAppViewer::LLUpdaterInfo() ;
	
#if LL_WINDOWS
	LLAppViewer::sUpdaterInfo->mUpdateExePath = gDirUtilp->getTempFilename();
	if (LLAppViewer::sUpdaterInfo->mUpdateExePath.empty())
	{
		delete LLAppViewer::sUpdaterInfo ;
		LLAppViewer::sUpdaterInfo = NULL ;

		// We're hosed, bail
		LL_WARNS("AppInit") << "LLDir::getTempFilename() failed" << LL_ENDL;
		LLAppViewer::instance()->forceQuit();
		return false;
	}

	LLAppViewer::sUpdaterInfo->mUpdateExePath += ".exe";

	std::string updater_source = gDirUtilp->getAppRODataDir();
	updater_source += gDirUtilp->getDirDelimiter();
	updater_source += "updater.exe";

	LL_DEBUGS("AppInit") << "Calling CopyFile source: " << updater_source
			<< " dest: " << LLAppViewer::sUpdaterInfo->mUpdateExePath
			<< LL_ENDL;


	if (!CopyFileA(updater_source.c_str(), LLAppViewer::sUpdaterInfo->mUpdateExePath.c_str(), FALSE))
	{
		delete LLAppViewer::sUpdaterInfo ;
		LLAppViewer::sUpdaterInfo = NULL ;

		LL_WARNS("AppInit") << "Unable to copy the updater!" << LL_ENDL;
		LLAppViewer::instance()->forceQuit();
		return false;
	}

	// if a sim name was passed in via command line parameter (typically through a SLURL)
	if ( LLURLSimString::sInstance.mSimString.length() )
	{
		// record the location to start at next time
		gSavedSettings.setString( "NextLoginLocation", LLURLSimString::sInstance.mSimString ); 
	};

	LLAppViewer::sUpdaterInfo->mParams << "-url \"" << update_url.asString() << "\"";

	LL_DEBUGS("AppInit") << "Calling updater: " << LLAppViewer::sUpdaterInfo->mUpdateExePath << " " << LLAppViewer::sUpdaterInfo->mParams.str() << LL_ENDL;

	//Explicitly remove the marker file, otherwise we pass the lock onto the child process and things get weird.
	LLAppViewer::instance()->removeMarkerFile(); // In case updater fails
	
#elif LL_DARWIN
	// if a sim name was passed in via command line parameter (typically through a SLURL)
	if ( LLURLSimString::sInstance.mSimString.length() )
	{
		// record the location to start at next time
		gSavedSettings.setString( "NextLoginLocation", LLURLSimString::sInstance.mSimString ); 
	};
	
	LLAppViewer::sUpdaterInfo->mUpdateExePath = "'";
	LLAppViewer::sUpdaterInfo->mUpdateExePath += gDirUtilp->getAppRODataDir();
	LLAppViewer::sUpdaterInfo->mUpdateExePath += "/mac-updater.app/Contents/MacOS/mac-updater' -url \"";
	LLAppViewer::sUpdaterInfo->mUpdateExePath += update_url.asString();
	LLAppViewer::sUpdaterInfo->mUpdateExePath += "\" -name \"";
	LLAppViewer::sUpdaterInfo->mUpdateExePath += LLAppViewer::instance()->getSecondLifeTitle();
	LLAppViewer::sUpdaterInfo->mUpdateExePath += "\" &";

	LL_DEBUGS("AppInit") << "Calling updater: " << LLAppViewer::sUpdaterInfo->mUpdateExePath << LL_ENDL;

	// Run the auto-updater.
	system(LLAppViewer::sUpdaterInfo->mUpdateExePath.c_str()); /* Flawfinder: ignore */

#elif LL_LINUX || LL_SOLARIS
	OSMessageBox("Automatic updating is not yet implemented for Linux.\n"
		"Please download the latest version from www.secondlife.com.",
		LLStringUtil::null, OSMB_OK);
#endif
	LLAppViewer::instance()->forceQuit();
	return false;
}

void use_circuit_callback(void**, S32 result)
{
	// bail if we're quitting.
	if(LLApp::isExiting()) return;
	if( !gUseCircuitCallbackCalled )
	{
		gUseCircuitCallbackCalled = true;
		if (result)
		{
			// Make sure user knows something bad happened. JC
			LL_WARNS("AppInit") << "Backing up to login screen!" << LL_ENDL;
			LLNotifications::instance().add("LoginPacketNeverReceived", LLSD(), LLSD(), login_alert_status);
			reset_login();
		}
		else
		{
			gGotUseCircuitCodeAck = true;
		}
	}
}

void pass_processAvatarPropertiesReply(LLMessageSystem *msg, void**)
{
	// send it to 'observers'
	LLPanelAvatar::processAvatarPropertiesReply(msg,0);
	LLFloaterAvatarList::processAvatarPropertiesReply(msg,0);
}

void pass_processObjectPropertiesFamily(LLMessageSystem *msg, void**)
{
	// send it to 'observers'
	LLSelectMgr::processObjectPropertiesFamily(msg,0);
	JCFloaterAnimList::processObjectPropertiesFamily(msg,0);
	JCFloaterAreaSearch::processObjectPropertiesFamily(msg,0);
}

void pass_processObjectProperties(LLMessageSystem *msg, void**)
{
	// send it to 'observers'
	JCExportTracker::processObjectProperties(msg,0);
	LLSelectMgr::processObjectProperties(msg,0);
}

void register_viewer_callbacks(LLMessageSystem* msg)
{
	msg->setHandlerFuncFast(_PREHASH_LayerData,				process_layer_data );
	msg->setHandlerFuncFast(_PREHASH_ImageData,				LLViewerImageList::receiveImageHeader );
	msg->setHandlerFuncFast(_PREHASH_ImagePacket,				LLViewerImageList::receiveImagePacket );
	msg->setHandlerFuncFast(_PREHASH_ObjectUpdate,				process_object_update );
	msg->setHandlerFunc("ObjectUpdateCompressed",				process_compressed_object_update );
	msg->setHandlerFunc("ObjectUpdateCached",					process_cached_object_update );
	msg->setHandlerFuncFast(_PREHASH_ImprovedTerseObjectUpdate, process_terse_object_update_improved );
	msg->setHandlerFunc("SimStats",				process_sim_stats);
	msg->setHandlerFuncFast(_PREHASH_HealthMessage,			process_health_message );
	msg->setHandlerFuncFast(_PREHASH_EconomyData,				process_economy_data);
	msg->setHandlerFunc("RegionInfo", LLViewerRegion::processRegionInfo);

	msg->setHandlerFuncFast(_PREHASH_ChatFromSimulator,		process_chat_from_simulator);
	msg->setHandlerFuncFast(_PREHASH_KillObject,				process_kill_object,	NULL);
	msg->setHandlerFuncFast(_PREHASH_SimulatorViewerTimeMessage,	process_time_synch,		NULL);
	msg->setHandlerFuncFast(_PREHASH_EnableSimulator,			process_enable_simulator);
	msg->setHandlerFuncFast(_PREHASH_DisableSimulator,			process_disable_simulator);
	msg->setHandlerFuncFast(_PREHASH_KickUser,					process_kick_user,		NULL);

	msg->setHandlerFunc("CrossedRegion", process_crossed_region);
	msg->setHandlerFuncFast(_PREHASH_TeleportFinish, process_teleport_finish);

	msg->setHandlerFuncFast(_PREHASH_AlertMessage,             process_alert_message);
	msg->setHandlerFunc("AgentAlertMessage", process_agent_alert_message);
	msg->setHandlerFuncFast(_PREHASH_MeanCollisionAlert,             process_mean_collision_alert_message,  NULL);
	msg->setHandlerFunc("ViewerFrozenMessage",             process_frozen_message);

	msg->setHandlerFuncFast(_PREHASH_NameValuePair,			process_name_value);
	msg->setHandlerFuncFast(_PREHASH_RemoveNameValuePair,	process_remove_name_value);
	msg->setHandlerFuncFast(_PREHASH_AvatarAnimation,		process_avatar_animation);
	msg->setHandlerFuncFast(_PREHASH_AvatarAppearance,		process_avatar_appearance);
	msg->setHandlerFunc("AgentCachedTextureResponse",	LLAgent::processAgentCachedTextureResponse);
	msg->setHandlerFunc("RebakeAvatarTextures", LLVOAvatar::processRebakeAvatarTextures);
	msg->setHandlerFuncFast(_PREHASH_CameraConstraint,		process_camera_constraint);
	msg->setHandlerFuncFast(_PREHASH_AvatarSitResponse,		process_avatar_sit_response);
	msg->setHandlerFunc("SetFollowCamProperties",			process_set_follow_cam_properties);
	msg->setHandlerFunc("ClearFollowCamProperties",			process_clear_follow_cam_properties);

	msg->setHandlerFuncFast(_PREHASH_ImprovedInstantMessage,	process_improved_im);
	msg->setHandlerFuncFast(_PREHASH_ScriptQuestion,			process_script_question);
	msg->setHandlerFuncFast(_PREHASH_ObjectProperties,			pass_processObjectProperties, NULL);
	msg->setHandlerFuncFast(_PREHASH_ObjectPropertiesFamily,	pass_processObjectPropertiesFamily, NULL);
	msg->setHandlerFunc("ForceObjectSelect", LLSelectMgr::processForceObjectSelect);

	msg->setHandlerFuncFast(_PREHASH_MoneyBalanceReply,		process_money_balance_reply,	NULL);
	msg->setHandlerFuncFast(_PREHASH_CoarseLocationUpdate,		LLWorld::processCoarseUpdate, NULL);
	msg->setHandlerFuncFast(_PREHASH_ReplyTaskInventory, 		LLViewerObject::processTaskInv,	NULL);
	msg->setHandlerFuncFast(_PREHASH_DerezContainer,			process_derez_container, NULL);
	msg->setHandlerFuncFast(_PREHASH_ScriptRunningReply,
						&LLLiveLSLEditor::processScriptRunningReply);

	msg->setHandlerFuncFast(_PREHASH_DeRezAck, process_derez_ack);

	msg->setHandlerFunc("LogoutReply", process_logout_reply);

	//msg->setHandlerFuncFast(_PREHASH_AddModifyAbility,
	//					&LLAgent::processAddModifyAbility);
	//msg->setHandlerFuncFast(_PREHASH_RemoveModifyAbility,
	//					&LLAgent::processRemoveModifyAbility);
	msg->setHandlerFuncFast(_PREHASH_AgentDataUpdate,
						&LLAgent::processAgentDataUpdate);
	msg->setHandlerFuncFast(_PREHASH_AgentGroupDataUpdate,
						&LLAgent::processAgentGroupDataUpdate);
	msg->setHandlerFunc("AgentDropGroup",
						&LLAgent::processAgentDropGroup);
	// land ownership messages
	msg->setHandlerFuncFast(_PREHASH_ParcelOverlay,
						LLViewerParcelMgr::processParcelOverlay);
	msg->setHandlerFuncFast(_PREHASH_ParcelProperties,
						LLViewerParcelMgr::processParcelProperties);
	msg->setHandlerFunc("ParcelAccessListReply",
		LLViewerParcelMgr::processParcelAccessListReply);
	msg->setHandlerFunc("ParcelDwellReply",
		LLViewerParcelMgr::processParcelDwellReply);

	msg->setHandlerFunc("AvatarPropertiesReply",
						pass_processAvatarPropertiesReply);
	msg->setHandlerFunc("AvatarInterestsReply",
						LLPanelAvatar::processAvatarInterestsReply);
	msg->setHandlerFunc("AvatarGroupsReply",
						LLPanelAvatar::processAvatarGroupsReply);
	// ratings deprecated
	//msg->setHandlerFuncFast(_PREHASH_AvatarStatisticsReply,
	//					LLPanelAvatar::processAvatarStatisticsReply);
	msg->setHandlerFunc("AvatarNotesReply",
						LLPanelAvatar::processAvatarNotesReply);
	msg->setHandlerFunc("AvatarPicksReply",
						LLPanelAvatar::processAvatarPicksReply);
	msg->setHandlerFunc("AvatarClassifiedReply",
						LLPanelAvatar::processAvatarClassifiedReply);

	msg->setHandlerFuncFast(_PREHASH_CreateGroupReply,
						LLGroupMgr::processCreateGroupReply);
	msg->setHandlerFuncFast(_PREHASH_JoinGroupReply,
						LLGroupMgr::processJoinGroupReply);
	msg->setHandlerFuncFast(_PREHASH_EjectGroupMemberReply,
						LLGroupMgr::processEjectGroupMemberReply);
	msg->setHandlerFuncFast(_PREHASH_LeaveGroupReply,
						LLGroupMgr::processLeaveGroupReply);
	msg->setHandlerFuncFast(_PREHASH_GroupProfileReply,
						LLGroupMgr::processGroupPropertiesReply);

	// ratings deprecated
	// msg->setHandlerFuncFast(_PREHASH_ReputationIndividualReply,
	//					LLFloaterRate::processReputationIndividualReply);

	msg->setHandlerFuncFast(_PREHASH_AgentWearablesUpdate,
						LLAgent::processAgentInitialWearablesUpdate );

	msg->setHandlerFunc("ScriptControlChange",
						LLAgent::processScriptControlChange );

	msg->setHandlerFuncFast(_PREHASH_ViewerEffect, LLHUDManager::processViewerEffect);

	msg->setHandlerFuncFast(_PREHASH_GrantGodlikePowers, process_grant_godlike_powers);

	msg->setHandlerFuncFast(_PREHASH_GroupAccountSummaryReply,
							LLPanelGroupLandMoney::processGroupAccountSummaryReply);
	msg->setHandlerFuncFast(_PREHASH_GroupAccountDetailsReply,
							LLPanelGroupLandMoney::processGroupAccountDetailsReply);
	msg->setHandlerFuncFast(_PREHASH_GroupAccountTransactionsReply,
							LLPanelGroupLandMoney::processGroupAccountTransactionsReply);

	msg->setHandlerFuncFast(_PREHASH_UserInfoReply,
		process_user_info_reply);

	msg->setHandlerFunc("RegionHandshake", process_region_handshake, NULL);

	msg->setHandlerFunc("TeleportStart", process_teleport_start );
	msg->setHandlerFunc("TeleportProgress", process_teleport_progress);
	msg->setHandlerFunc("TeleportFailed", process_teleport_failed, NULL);
	msg->setHandlerFunc("TeleportLocal", process_teleport_local, NULL);

	msg->setHandlerFunc("ImageNotInDatabase", LLViewerImageList::processImageNotInDatabase, NULL);

	msg->setHandlerFuncFast(_PREHASH_GroupMembersReply,
						LLGroupMgr::processGroupMembersReply);
	msg->setHandlerFunc("GroupRoleDataReply",
						LLGroupMgr::processGroupRoleDataReply);
	msg->setHandlerFunc("GroupRoleMembersReply",
						LLGroupMgr::processGroupRoleMembersReply);
	msg->setHandlerFunc("GroupTitlesReply",
						LLGroupMgr::processGroupTitlesReply);
	// Special handler as this message is sometimes used for group land.
	msg->setHandlerFunc("PlacesReply", process_places_reply);
	msg->setHandlerFunc("GroupNoticesListReply", LLPanelGroupNotices::processGroupNoticesListReply);

	msg->setHandlerFunc("DirPlacesReply", LLPanelDirBrowser::processDirPlacesReply);
	msg->setHandlerFunc("DirPeopleReply", LLPanelDirBrowser::processDirPeopleReply);
	msg->setHandlerFunc("DirEventsReply", LLPanelDirBrowser::processDirEventsReply);
	msg->setHandlerFunc("DirGroupsReply", LLPanelDirBrowser::processDirGroupsReply);
	//msg->setHandlerFunc("DirPicksReply",  LLPanelDirBrowser::processDirPicksReply);
	msg->setHandlerFunc("DirClassifiedReply",  LLPanelDirBrowser::processDirClassifiedReply);
	msg->setHandlerFunc("DirLandReply",   LLPanelDirBrowser::processDirLandReply);
	//msg->setHandlerFunc("DirPopularReply",LLPanelDirBrowser::processDirPopularReply);

	msg->setHandlerFunc("AvatarPickerReply", LLFloaterAvatarPicker::processAvatarPickerReply);

	msg->setHandlerFunc("MapLayerReply", LLWorldMap::processMapLayerReply);
	msg->setHandlerFunc("MapBlockReply", LLWorldMap::processMapBlockReply);
	msg->setHandlerFunc("MapItemReply", LLWorldMap::processMapItemReply);

	msg->setHandlerFunc("EventInfoReply", LLPanelEvent::processEventInfoReply);
	msg->setHandlerFunc("PickInfoReply", LLPanelPick::processPickInfoReply);
	msg->setHandlerFunc("ClassifiedInfoReply", LLPanelClassified::processClassifiedInfoReply);
	msg->setHandlerFunc("ParcelInfoReply", LLPanelPlace::processParcelInfoReply);
	msg->setHandlerFunc("ScriptDialog", process_script_dialog);
	msg->setHandlerFunc("LoadURL", process_load_url);
	msg->setHandlerFunc("ScriptTeleportRequest", process_script_teleport_request);
	msg->setHandlerFunc("EstateCovenantReply", process_covenant_reply);

	// calling cards
	msg->setHandlerFunc("OfferCallingCard", process_offer_callingcard);
	msg->setHandlerFunc("AcceptCallingCard", process_accept_callingcard);
	msg->setHandlerFunc("DeclineCallingCard", process_decline_callingcard);

	msg->setHandlerFunc("ParcelObjectOwnersReply", LLPanelLandObjects::processParcelObjectOwnersReply);

	msg->setHandlerFunc("InitiateDownload", process_initiate_download);
	msg->setHandlerFunc("LandStatReply", LLFloaterTopObjects::handle_land_reply);
	msg->setHandlerFunc("GenericMessage", process_generic_message);

	msg->setHandlerFuncFast(_PREHASH_FeatureDisabled, process_feature_disabled_message);
}

void init_stat_view()
{
	LLFrameStatView *frameviewp = gDebugView->mFrameStatView;
	frameviewp->setup(gFrameStats);
	frameviewp->mShowPercent = FALSE;
}

void asset_callback_nothing(LLVFS*, const LLUUID&, LLAssetType::EType, void*, S32)
{
	// nothing
}

// *HACK: Must match name in Library or agent inventory
const std::string COMMON_GESTURES_FOLDER = "Common Gestures";
const std::string MALE_GESTURES_FOLDER = "Male Gestures";
const std::string FEMALE_GESTURES_FOLDER = "Female Gestures";
const std::string MALE_OUTFIT_FOLDER = "Male Shape & Outfit";
const std::string FEMALE_OUTFIT_FOLDER = "Female Shape & Outfit";
const S32 OPT_CLOSED_WINDOW = -1;
const S32 OPT_MALE = 0;
const S32 OPT_FEMALE = 1;

bool callback_choose_gender(const LLSD& notification, const LLSD& response)
{	
	S32 option = LLNotification::getSelectedOption(notification, response);
	switch(option)
	{
	case OPT_MALE:
		LLStartUp::loadInitialOutfit( MALE_OUTFIT_FOLDER, "male" );
		break;

	case OPT_FEMALE:
	case OPT_CLOSED_WINDOW:
	default:
		LLStartUp::loadInitialOutfit( FEMALE_OUTFIT_FOLDER, "female" );
		break;
	}
	return false;
}

void LLStartUp::loadInitialOutfit( const std::string& outfit_folder_name,
								   const std::string& gender_name )
{
	S32 gender = 0;
	std::string gestures;
	if (gender_name == "male")
	{
		gender = OPT_MALE;
		gestures = MALE_GESTURES_FOLDER;
	}
	else
	{
		gender = OPT_FEMALE;
		gestures = FEMALE_GESTURES_FOLDER;
	}

	// try to find the outfit - if not there, create some default
	// wearables.
	LLInventoryModel::cat_array_t cat_array;
	LLInventoryModel::item_array_t item_array;
	LLNameCategoryCollector has_name(outfit_folder_name);
	gInventory.collectDescendentsIf(LLUUID::null,
									cat_array,
									item_array,
									LLInventoryModel::EXCLUDE_TRASH,
									has_name);
	if (0 == cat_array.count())
	{
		gAgent.createStandardWearables(gender);
	}
	else
	{
		wear_outfit_by_name(outfit_folder_name);
	}
	wear_outfit_by_name(gestures);
	wear_outfit_by_name(COMMON_GESTURES_FOLDER);

	// This is really misnamed -- it means we have started loading
	// an outfit/shape that will give the avatar a gender eventually. JC
	gAgent.setGenderChosen(TRUE);

}

// Loads a bitmap to display during load
// location_id = 0 => last position
// location_id = 1 => home position
void init_start_screen(S32 location_id)
{
	if (gStartImageGL.notNull())
	{
		gStartImageGL = NULL;
		LL_INFOS("AppInit") << "re-initializing start screen" << LL_ENDL;
	}

	LL_DEBUGS("AppInit") << "Loading startup bitmap..." << LL_ENDL;

	std::string temp_str = gDirUtilp->getLindenUserDir() + gDirUtilp->getDirDelimiter();

	if ((S32)START_LOCATION_ID_LAST == location_id)
	{
		temp_str += SCREEN_LAST_FILENAME;
	}
	else
	{
		temp_str += SCREEN_HOME_FILENAME;
	}

	LLPointer<LLImageBMP> start_image_bmp = new LLImageBMP;
	
	// Turn off start screen to get around the occasional readback 
	// driver bug
	if(!gSavedSettings.getBOOL("UseStartScreen"))
	{
		LL_INFOS("AppInit")  << "Bitmap load disabled" << LL_ENDL;
		return;
	}
	else if(!start_image_bmp->load(temp_str) )
	{
		LL_WARNS("AppInit") << "Bitmap load failed" << LL_ENDL;
		return;
	}

	gStartImageGL = new LLImageGL(FALSE);
	gStartImageWidth = start_image_bmp->getWidth();
	gStartImageHeight = start_image_bmp->getHeight();

	LLPointer<LLImageRaw> raw = new LLImageRaw;
	if (!start_image_bmp->decode(raw, 0.0f))
	{
		LL_WARNS("AppInit") << "Bitmap decode failed" << LL_ENDL;
		gStartImageGL = NULL;
		return;
	}

	raw->expandToPowerOfTwo();
	gStartImageGL->createGLTexture(0, raw);
}


// frees the bitmap
void release_start_screen()
{
	LL_DEBUGS("AppInit") << "Releasing bitmap..." << LL_ENDL;
	gStartImageGL = NULL;
}


// static
std::string LLStartUp::startupStateToString(EStartupState state)
{
#define RTNENUM(E) case E: return #E
	switch(state){
		RTNENUM( STATE_FIRST );
		RTNENUM( STATE_LOGIN_SHOW );
		RTNENUM( STATE_LOGIN_WAIT );
		RTNENUM( STATE_LOGIN_CLEANUP );
		RTNENUM( STATE_UPDATE_CHECK );
		RTNENUM( STATE_LOGIN_AUTH_INIT );
		RTNENUM( STATE_LOGIN_AUTHENTICATE );
		RTNENUM( STATE_LOGIN_NO_DATA_YET );
		RTNENUM( STATE_LOGIN_DOWNLOADING );
		RTNENUM( STATE_LOGIN_PROCESS_RESPONSE );
		RTNENUM( STATE_WORLD_INIT );
		RTNENUM( STATE_SEED_GRANTED_WAIT );
		RTNENUM( STATE_SEED_CAP_GRANTED );
		RTNENUM( STATE_WORLD_WAIT );
		RTNENUM( STATE_AGENT_SEND );
		RTNENUM( STATE_AGENT_WAIT );
		RTNENUM( STATE_INVENTORY_SEND );
		RTNENUM( STATE_MISC );
		RTNENUM( STATE_PRECACHE );
		RTNENUM( STATE_WEARABLES_WAIT );
		RTNENUM( STATE_CLEANUP );
		RTNENUM( STATE_STARTED );
	default:
		return llformat("(state #%d)", state);
	}
#undef RTNENUM
}


// static
void LLStartUp::setStartupState( EStartupState state )
{
	LL_INFOS("AppInit") << "Startup state changing from " <<  
		startupStateToString(gStartupState) << " to " <<  
		startupStateToString(state) << LL_ENDL;
	gStartupState = state;
}


void reset_login()
{
	LLStartUp::setStartupState( STATE_LOGIN_SHOW );

	if ( gViewerWindow )
	{	// Hide menus and normal buttons
		gViewerWindow->setNormalControlsVisible( FALSE );
		gLoginMenuBarView->setVisible( TRUE );
		gLoginMenuBarView->setEnabled( TRUE );
	}

	// Hide any other stuff
	LLFloaterMap::hideInstance();
}

//---------------------------------------------------------------------------

std::string LLStartUp::sSLURLCommand;

bool LLStartUp::canGoFullscreen()
{
	return gStartupState >= STATE_WORLD_INIT;
}

// Initialize all plug-ins except the web browser (which was initialized
// early, before the login screen). JC
void LLStartUp::multimediaInit()
{
	LL_DEBUGS("AppInit") << "Initializing Multimedia...." << LL_ENDL;
	std::string msg = LLTrans::getString("LoginInitializingMultimedia");
	set_startup_status(0.42f, msg.c_str(), gAgent.mMOTD.c_str());
	display_startup();

	LLViewerMedia::initClass();
	LLViewerParcelMedia::initClass();
}

bool LLStartUp::dispatchURL()
{
	// ok, if we've gotten this far and have a startup URL
	if (!sSLURLCommand.empty())
	{
		LLWebBrowserCtrl* web = NULL;
		const bool trusted_browser = false;
		LLURLDispatcher::dispatch(sSLURLCommand, web, trusted_browser);
	}
	else if (LLURLSimString::parse())
	{
		// If we started with a location, but we're already
		// at that location, don't pop dialogs open.
		LLVector3 pos = gAgent.getPositionAgent();
		F32 dx = pos.mV[VX] - (F32)LLURLSimString::sInstance.mX;
		F32 dy = pos.mV[VY] - (F32)LLURLSimString::sInstance.mY;
		const F32 SLOP = 2.f;	// meters

		if( LLURLSimString::sInstance.mSimName != gAgent.getRegion()->getName()
			|| (dx*dx > SLOP*SLOP)
			|| (dy*dy > SLOP*SLOP) )
		{
			std::string url = LLURLSimString::getURL();
			LLWebBrowserCtrl* web = NULL;
			const bool trusted_browser = false;
			LLURLDispatcher::dispatch(url, web, trusted_browser);
		}
		return true;
	}
	return false;
}

bool login_alert_done(const LLSD& notification, const LLSD& response)
{
	LLPanelLogin::giveFocus();
	return false;
}


void apply_udp_blacklist(const std::string& csv)
{

	std::string::size_type start = 0;
	std::string::size_type comma = 0;
	do 
	{
		comma = csv.find(",", start);
		if (comma == std::string::npos)
		{
			comma = csv.length();
		}
		std::string item(csv, start, comma-start);

		lldebugs << "udp_blacklist " << item << llendl;
		gMessageSystem->banUdpMessage(item);
		
		start = comma + 1;

	}
	while(comma < csv.length());
	
}
