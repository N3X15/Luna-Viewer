/** 
 * @file llviewermenufile.cpp
 * @brief "File" menu in the main menu bar.
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2002-2009, Linden Research, Inc.
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

#include "llviewermenufile.h"

// project includes
#include "llagent.h"

#include "llfilepicker.h"
#include "llfloateranimpreview.h"
#include "llfloaterbuycurrency.h"

#include "llfloaterimagepreview.h"
#include "llfloaternamedesc.h"
#include "llfloatersnapshot.h"

#include "llinventorymodel.h"	// gInventory
#include "llresourcedata.h"
#include "llfloaterperms.h"
#include "llstatusbar.h"

#include "llviewercontrol.h"	// gSavedSettings
#include "llviewerimagelist.h"
#include "lluictrlfactory.h"
#include "llviewermenu.h"	// gMenuHolder
#include "llviewerregion.h"
#include "llviewerstats.h"
#include "llviewerwindow.h"
#include "llappviewer.h"
#include "lluploaddialog.h"
// <edit>
#include "llselectmgr.h"
#include "llfloaterimport.h"
#include "llfloaterexport.h"
#include "llassettype.h"
#include "llinventorytype.h"
#include "llbvhloader.h"
#include "lllocalinventory.h"
// </edit>


// linden libraries
#include "llassetuploadresponders.h"
#include "lleconomy.h"
#include "llhttpclient.h"
#include "llmemberlistener.h"
#include "llsdserialize.h"
#include "llstring.h"
#include "lltransactiontypes.h"
#include "lluuid.h"
#include "llvorbisencode.h"

// system libraries
#include <boost/tokenizer.hpp>

//#include "importtracker.h"

typedef LLMemberListener<LLView> view_listener_t;


class LLFileEnableSaveAs : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool new_value = gFloaterView->getFrontmost() && gFloaterView->getFrontmost()->canSaveAs();
		gMenuHolder->findControl(userdata["control"].asString())->setValue(new_value);
		return true;
	}
};

class LLFileEnableUpload : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool new_value = gStatusBar && LLGlobalEconomy::Singleton::getInstance() && (gStatusBar->getBalance() >= LLGlobalEconomy::Singleton::getInstance()->getPriceUpload());
		gMenuHolder->findControl(userdata["control"].asString())->setValue(new_value);
		return true;
	}
};

//============================================================================

#if LL_WINDOWS
static std::string SOUND_EXTENSIONS = "wav";
static std::string IMAGE_EXTENSIONS = "tga bmp jpg jpeg png";
static std::string ANIM_EXTENSIONS =  "bvh anim animatn";
#ifdef _CORY_TESTING
static std::string GEOMETRY_EXTENSIONS = "slg";
#endif
static std::string XML_EXTENSIONS = "xml";
static std::string SLOBJECT_EXTENSIONS = "slobject";
#endif
static std::string ALL_FILE_EXTENSIONS = "*.*";

std::string build_extensions_string(LLFilePicker::ELoadFilter filter)
{
	switch(filter)
	{
#if LL_WINDOWS
	case LLFilePicker::FFLOAD_IMAGE:
		return IMAGE_EXTENSIONS;
	case LLFilePicker::FFLOAD_WAV:
		return SOUND_EXTENSIONS;
	case LLFilePicker::FFLOAD_ANIM:
		return ANIM_EXTENSIONS;
	case LLFilePicker::FFLOAD_SLOBJECT:
		return SLOBJECT_EXTENSIONS;
#ifdef _CORY_TESTING
	case LLFilePicker::FFLOAD_GEOMETRY:
		return GEOMETRY_EXTENSIONS;
#endif
	case LLFilePicker::FFLOAD_XML:
	    return XML_EXTENSIONS;
	case LLFilePicker::FFLOAD_ALL:
		return ALL_FILE_EXTENSIONS;
#endif
    default:
	return ALL_FILE_EXTENSIONS;
	}
}

/**
   char* upload_pick(void* data)

   If applicable, brings up a file chooser in which the user selects a file
   to upload for a particular task.  If the file is valid for the given action,
   returns the string to the full path filename, else returns NULL.
   Data is the load filter for the type of file as defined in LLFilePicker.

	Eventually I'd really like to have a built-in browser that gave you all 
	valid filetypes, default permissions, "Temp when available", and a single 
	upload button. Maybe let it show the contents of multiple folders. The main 
	purpose of this features is to deal with the problem of SL just up and 
	disconnecting if you take more than like 30 seconds to look for a file. -HgB
**/
const std::string upload_pick(void* data)
{
 	if( gAgent.cameraMouselook() )
	{
		gAgent.changeCameraToDefault();
		// This doesn't seem necessary. JC
		// display();
	}

	LLFilePicker::ELoadFilter type;
	if(data)
	{
		type = (LLFilePicker::ELoadFilter)((intptr_t)data);
	}
	else
	{
		type = LLFilePicker::FFLOAD_ALL;
	}

	LLFilePicker& picker = LLFilePicker::instance();
	if (!picker.getOpenFile(type))
	{
		llinfos << "Couldn't import objects from file" << llendl;
		return std::string();
	}

	
	const std::string& filename = picker.getFirstFile();
	std::string ext = gDirUtilp->getExtension(filename);

	//strincmp doesn't like NULL pointers
	if (ext.empty())
	{
		std::string short_name = gDirUtilp->getBaseFileName(filename);
		
		// No extension
		LLSD args;
		args["FILE"] = short_name;
		LLNotifications::instance().add("NoFileExtension", args);
		return std::string();
	}
	else
	{
		//so there is an extension
		//loop over the valid extensions and compare to see
		//if the extension is valid

		//now grab the set of valid file extensions
		std::string valid_extensions = build_extensions_string(type);

		BOOL ext_valid = FALSE;
		
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" ");
		tokenizer tokens(valid_extensions, sep);
		tokenizer::iterator token_iter;

		//now loop over all valid file extensions
		//and compare them to the extension of the file
		//to be uploaded
		for( token_iter = tokens.begin();
			 token_iter != tokens.end() && ext_valid != TRUE;
			 ++token_iter)
		{
			const std::string& cur_token = *token_iter;

			if (cur_token == ext || cur_token == "*.*")
			{
				//valid extension
				//or the acceptable extension is any
				ext_valid = TRUE;
			}
		}//end for (loop over all tokens)

		if (ext_valid == FALSE)
		{
			//should only get here if the extension exists
			//but is invalid
			LLSD args;
			args["EXTENSION"] = ext;
			args["VALIDS"] = valid_extensions;
			LLNotifications::instance().add("InvalidFileExtension", args);
			return NULL;
		}
	}//end else (non-null extension)

	//valid file extension
	
	//now we check to see
	//if the file is actually a valid image/sound/etc.
	//Consider completely disabling this, see how SL handles it. Maybe we can get full song uploads again! -HgB
	if (type == LLFilePicker::FFLOAD_WAV)
	{
		// pre-qualify wavs to make sure the format is acceptable
		std::string error_msg;
		if (check_for_invalid_wav_formats(filename,error_msg))
		{
			llinfos << error_msg << ": " << filename << llendl;
			LLSD args;
			args["FILE"] = filename;
			LLNotifications::instance().add( error_msg, args );
			return std::string();
		}
	}//end if a wave/sound file

	return filename;
}

class LLFileUploadImage : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		std::string filename = upload_pick((void *)LLFilePicker::FFLOAD_IMAGE);
		if (!filename.empty())
		{
			LLFloaterImagePreview* floaterp = new LLFloaterImagePreview(filename);
			LLUICtrlFactory::getInstance()->buildFloater(floaterp, "floater_image_preview.xml");
		}
		return TRUE;
	}
};

class LLFileUploadSound : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		std::string filename = upload_pick((void*)LLFilePicker::FFLOAD_WAV);
		if (!filename.empty())
		{
			LLFloaterNameDesc* floaterp = new LLFloaterNameDesc(filename);
			LLUICtrlFactory::getInstance()->buildFloater(floaterp, "floater_sound_preview.xml");
			floaterp->childSetLabelArg("ok_btn", "[AMOUNT]", llformat("%d", LLGlobalEconomy::Singleton::getInstance()->getPriceUpload() ));
		}
		return true;
	}
};

class LLFileUploadAnim : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		const std::string filename = upload_pick((void*)LLFilePicker::FFLOAD_ANIM);
		if (!filename.empty())
		{
			LLFloaterAnimPreview* floaterp = new LLFloaterAnimPreview(filename);
			LLUICtrlFactory::getInstance()->buildFloater(floaterp, "floater_animation_preview.xml");
		}
		return true;
	}
};

class LLFileUploadBulk : public view_listener_t
{
	static bool onConfirmBulkUploadTemp(const LLSD& notification, const LLSD& response )
	{
		S32 option = LLNotification::getSelectedOption(notification, response);
		BOOL enabled;
		if(option == 0) // yes
			enabled = TRUE;
		else if(option == 1) //no
			enabled = FALSE;
		else //cancel
			return false;

		LLFilePicker& picker = LLFilePicker::instance();
		if (picker.getMultipleOpenFiles())
		{			
			//const std::string& filename = picker.getFirstFile();
			std::string filename;
			while(!(filename = picker.getNextFile()).empty())
			{
			std::string name = gDirUtilp->getBaseFileName(filename, true);
			
			std::string asset_name = name;
			LLStringUtil::replaceNonstandardASCII( asset_name, '?' );
			LLStringUtil::replaceChar(asset_name, '|', '?');
			LLStringUtil::stripNonprintable(asset_name);
			LLStringUtil::trim(asset_name);
			
			std::string display_name = LLStringUtil::null;
			LLAssetStorage::LLStoreAssetCallback callback = NULL;
			S32 expected_upload_cost = LLGlobalEconomy::Singleton::getInstance()->getPriceUpload();
			void *userdata = NULL;
			gSavedSettings.setBOOL("TemporaryUpload",enabled);
			upload_new_resource(filename, asset_name, asset_name, 0, LLAssetType::AT_NONE, LLInventoryType::IT_NONE,
				LLFloaterPerms::getNextOwnerPerms(), LLFloaterPerms::getGroupPerms(), LLFloaterPerms::getEveryonePerms(),
					    display_name,
					    callback, expected_upload_cost, userdata);

			}
		}
		else
		{
			llinfos << "Couldn't import objects from file" << llendl;
			gSavedSettings.setBOOL("TemporaryUpload",FALSE);
		}
		return false;
	}
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		if( gAgent.cameraMouselook() )
		{
			gAgent.changeCameraToDefault();
		}

		// TODO:
		// Iterate over all files
		// Check extensions for uploadability, cost
		// Check user balance for entire cost
		// Charge user entire cost
		// Loop, uploading
		// If an upload fails, refund the user for that one
		//
		// Also fix single upload to charge first, then refund
		// <edit>
		S32 expected_upload_cost = LLGlobalEconomy::Singleton::getInstance()->getPriceUpload();
		LLSD args;
		std::string msg = "Would you like to bulk upload the files as temporary files?\nOnly textures will upload as temporary on Agni and Aditi.";
		if(expected_upload_cost)
			msg.append(llformat("\nWARNING: Each upload costs L$%d if it's not temporary.",expected_upload_cost));
		args["MESSAGE"] = msg;
		LLNotifications::instance().add("GenericAlertYesNoCancel", args, LLSD(), onConfirmBulkUploadTemp);
		/* moved to the callback for the above
		LLFilePicker& picker = LLFilePicker::instance();
		if (picker.getMultipleOpenFiles())
		{
			// <edit>
			
			//const std::string& filename = picker.getFirstFile();
			std::string filename;
			while(!(filename = picker.getNextFile()).empty())
			{
			// </edit>
			std::string name = gDirUtilp->getBaseFileName(filename, true);
			
			std::string asset_name = name;
			LLStringUtil::replaceNonstandardASCII( asset_name, '?' );
			LLStringUtil::replaceChar(asset_name, '|', '?');
			LLStringUtil::stripNonprintable(asset_name);
			LLStringUtil::trim(asset_name);
			
			std::string display_name = LLStringUtil::null;
			LLAssetStorage::LLStoreAssetCallback callback = NULL;
			S32 expected_upload_cost = LLGlobalEconomy::Singleton::getInstance()->getPriceUpload();
			void *userdata = NULL;
			upload_new_resource(filename, asset_name, asset_name, 0, LLAssetType::AT_NONE, LLInventoryType::IT_NONE,
				LLFloaterPerms::getNextOwnerPerms(), LLFloaterPerms::getGroupPerms(), LLFloaterPerms::getEveryonePerms(),
					    display_name,
					    callback, expected_upload_cost, userdata);

			// *NOTE: Ew, we don't iterate over the file list here,
			// we handle the next files in upload_done_callback()
			// </edit> not anymore!
			}
		}
		else
		{
			llinfos << "Couldn't import objects from file" << llendl;
		}
		*/
		return true;
	}
};

// <edit>
class LLFileImportXML : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLFilePicker& picker = LLFilePicker::instance();
		if (!picker.getOpenFile(LLFilePicker::FFLOAD_XML))
		{
			return true;
		}
		std::string file_name = picker.getFirstFile();
		new LLFloaterXmlImportOptions(new LLXmlImportOptions(file_name));
		return true;
	}
};

class LLFileEnableImportXML : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool new_value = !LLXmlImport::sImportInProgress;

		// horrendously opaque, this code
		gMenuHolder->findControl(userdata["control"].asString())->setValue(new_value);
		return true;
	}
};
// </edit>

void upload_error(const std::string& error_message, const std::string& label, const std::string& filename, const LLSD& args) 
{
	llwarns << error_message << llendl;
	LLNotifications::instance().add(label, args);
	if(LLFile::remove(filename) == -1)
	{
		lldebugs << "unable to remove temp file" << llendl;
	}
	LLFilePicker::instance().reset();						
}


































































































































































class LLFileEnableCloseWindow : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool new_value = NULL != LLFloater::getClosableFloaterFromFocus();

		// horrendously opaque, this code
		gMenuHolder->findControl(userdata["control"].asString())->setValue(new_value);
		return true;
	}
};

class LLFileCloseWindow : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLFloater::closeFocusedFloater();

		return true;
	}
};

class LLFileEnableCloseAllWindows : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool open_children = gFloaterView->allChildrenClosed();
		gMenuHolder->findControl(userdata["control"].asString())->setValue(!open_children);
		return true;
	}
};

class LLFileCloseAllWindows : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		bool app_quitting = false;
		gFloaterView->closeAllChildren(app_quitting);

		return true;
	}
};

// <edit>
class LLFileMinimizeAllWindows : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		gFloaterView->minimizeAllChildren();
		return true;
	}
};
// </edit>

class LLFileSaveTexture : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLFloater* top = gFloaterView->getFrontmost();
		if (top)
		{
			// <edit>
			if(top->canSaveAs())
			{
			// </edit>
				top->saveAs();
			// <edit>
				return true;
			}
			// </edit>
		}
		// <edit>
		top = new LLFloaterExport();
		top->center();
		// </edit>
		return true;
	}
};

class LLFileTakeSnapshot : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLFloaterSnapshot::show(NULL);
		return true;
	}
};

class LLFileTakeSnapshotToDisk : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLPointer<LLImageRaw> raw = new LLImageRaw;

		S32 width = gViewerWindow->getWindowDisplayWidth();
		S32 height = gViewerWindow->getWindowDisplayHeight();

		if (gSavedSettings.getBOOL("HighResSnapshot"))
		{
			width *= 2;
			height *= 2;
		}

		if (gViewerWindow->rawSnapshot(raw,
									   width,
									   height,
									   TRUE,
									   FALSE,
									   gSavedSettings.getBOOL("RenderUIInSnapshot"),
									   FALSE))
		{
			gViewerWindow->playSnapshotAnimAndSound();
			
			LLImageBase::setSizeOverride(TRUE);
			LLPointer<LLImageFormatted> formatted;
			switch(LLFloaterSnapshot::ESnapshotFormat(gSavedSettings.getS32("SnapshotFormat")))
			{
			  case LLFloaterSnapshot::SNAPSHOT_FORMAT_JPEG:
				formatted = new LLImageJPEG(gSavedSettings.getS32("SnapshotQuality"));
				break;
			  case LLFloaterSnapshot::SNAPSHOT_FORMAT_PNG:
				formatted = new LLImagePNG;
				break;
			  case LLFloaterSnapshot::SNAPSHOT_FORMAT_BMP: 
				formatted = new LLImageBMP;
				break;
			  default: 
				llwarns << "Unknown Local Snapshot format" << llendl;
				LLImageBase::setSizeOverride(FALSE);
				return true;
			}

			formatted->encode(raw, 0);
			LLImageBase::setSizeOverride(FALSE);
			gViewerWindow->saveImageNumbered(formatted);
		}
		return true;
	}
};

class LLFileLogOut : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		std::string command(gDirUtilp->getExecutableDir() + gDirUtilp->getDirDelimiter() + gDirUtilp->getExecutableFilename());
		gSavedSettings.setBOOL("ShowConsoleWindow", FALSE);
		gViewerWindow->getWindow()->ShellEx(command);
		gSavedSettings.setBOOL("ShowConsoleWindow", FALSE);
		LLAppViewer::instance()->userQuit();
		gSavedSettings.setBOOL("ShowConsoleWindow", FALSE);
		return true;
	}
};

class LLFileQuit : public view_listener_t
{
	bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
	{
		LLAppViewer::instance()->userQuit();
		return true;
	}
};

void handle_upload(void* data)
{
	const std::string filename = upload_pick(data);
	if (!filename.empty())
	{
		LLFloaterNameDesc* floaterp = new LLFloaterNameDesc(filename);
		LLUICtrlFactory::getInstance()->buildFloater(floaterp, "floater_name_description.xml");
		floaterp->childSetLabelArg("ok_btn", "[AMOUNT]", llformat("%d", LLGlobalEconomy::Singleton::getInstance()->getPriceUpload() ));
	}
}

void handle_compress_image(void*)
{
	LLFilePicker& picker = LLFilePicker::instance();
	if (picker.getMultipleOpenFiles(LLFilePicker::FFLOAD_IMAGE))
	{
		std::string infile = picker.getFirstFile();
		while (!infile.empty())
		{
			std::string outfile = infile + ".j2c";

			llinfos << "Input:  " << infile << llendl;
			llinfos << "Output: " << outfile << llendl;

			BOOL success;

			success = LLViewerImageList::createUploadFile(infile, outfile, IMG_CODEC_TGA);

			if (success)
			{
				llinfos << "Compression complete" << llendl;
			}
			else
			{
				llinfos << "Compression failed: " << LLImage::getLastError() << llendl;
			}

			infile = picker.getNextFile();
		}
	}
}

void upload_new_resource(const std::string& src_filename, std::string name,
			 std::string desc, S32 compression_info,
			 LLAssetType::EType destination_folder_type,
			 LLInventoryType::EType inv_type,
			 U32 next_owner_perms,
			 U32 group_perms,
			 U32 everyone_perms,
			 const std::string& display_name,
			 LLAssetStorage::LLStoreAssetCallback callback,
			 S32 expected_upload_cost,
			 void *userdata)
{	
	// Generate the temporary UUID.
	std::string filename = gDirUtilp->getTempFilename();
	LLTransactionID tid;
	LLAssetID uuid;
	
	LLSD args;

	std::string exten = gDirUtilp->getExtension(src_filename);
	LLAssetType::EType asset_type = LLAssetType::AT_NONE;
	std::string error_message;

	BOOL error = FALSE;
	
	if (exten.empty())
	{
		std::string short_name = gDirUtilp->getBaseFileName(filename);
		
		// No extension
		error_message = llformat(
				"No file extension for the file: '%s'\nPlease make sure the file has a correct file extension",
				short_name.c_str());
		args["FILE"] = short_name;
 		upload_error(error_message, "NofileExtension", filename, args);
		return;
	}
	else if( exten == "bmp")
	{
		asset_type = LLAssetType::AT_TEXTURE;
		if (!LLViewerImageList::createUploadFile(src_filename,
												 filename,
												 IMG_CODEC_BMP ))
		{
			error_message = llformat( "Problem with file %s:\n\n%s\n",
					src_filename.c_str(), LLImage::getLastError().c_str());
			args["FILE"] = src_filename;
			args["ERROR"] = LLImage::getLastError();
			upload_error(error_message, "ProblemWithFile", filename, args);
			return;
		}
	}
	else if( exten == "tga")
	{
		asset_type = LLAssetType::AT_TEXTURE;
		if (!LLViewerImageList::createUploadFile(src_filename,
												 filename,
												 IMG_CODEC_TGA ))
		{
			error_message = llformat("Problem with file %s:\n\n%s\n",
					src_filename.c_str(), LLImage::getLastError().c_str());
			args["FILE"] = src_filename;
			args["ERROR"] = LLImage::getLastError();
			upload_error(error_message, "ProblemWithFile", filename, args);
			return;
		}
	}
	else if( exten == "jpg" || exten == "jpeg")
	{
		asset_type = LLAssetType::AT_TEXTURE;
		if (!LLViewerImageList::createUploadFile(src_filename,
												 filename,
												 IMG_CODEC_JPEG ))
		{
			error_message = llformat("Problem with file %s:\n\n%s\n",
					src_filename.c_str(), LLImage::getLastError().c_str());
			args["FILE"] = src_filename;
			args["ERROR"] = LLImage::getLastError();
			upload_error(error_message, "ProblemWithFile", filename, args);
			return;
		}
	}
 	else if( exten == "png")
 	{
 		asset_type = LLAssetType::AT_TEXTURE;
 		if (!LLViewerImageList::createUploadFile(src_filename,
 												 filename,
 												 IMG_CODEC_PNG ))
 		{
 			error_message = llformat("Problem with file %s:\n\n%s\n",
 					src_filename.c_str(), LLImage::getLastError().c_str());
 			args["FILE"] = src_filename;
 			args["ERROR"] = LLImage::getLastError();
 			upload_error(error_message, "ProblemWithFile", filename, args);
 			return;
 		}
 	}
	else if(exten == "wav")
	{
		asset_type = LLAssetType::AT_SOUND;  // tag it as audio
		S32 encode_result = 0;

		llinfos << "Attempting to encode wav as an ogg file" << llendl;

		encode_result = encode_vorbis_file(src_filename, filename);
		
		if (LLVORBISENC_NOERR != encode_result)
		{
			switch(encode_result)
			{
				case LLVORBISENC_DEST_OPEN_ERR:
				    error_message = llformat( "Couldn't open temporary compressed sound file for writing: %s\n", filename.c_str());
					args["FILE"] = filename;
					upload_error(error_message, "CannotOpenTemporarySoundFile", filename, args);
					break;

				default:	
				  error_message = llformat("Unknown vorbis encode failure on: %s\n", src_filename.c_str());
					args["FILE"] = src_filename;
					upload_error(error_message, "UnknownVorbisEncodeFailure", filename, args);
					break;	
			}	
			return;
		}
	}
	else if(exten == "tmp")	 	
	{	 	
		// This is a generic .lin resource file	 	
         asset_type = LLAssetType::AT_OBJECT;	 	
         LLFILE* in = LLFile::fopen(src_filename, "rb");		/* Flawfinder: ignore */	 	
         if (in)	 	
         {	 	
                 // read in the file header	 	
                 char buf[16384];		/* Flawfinder: ignore */ 	
                 S32 read;		/* Flawfinder: ignore */	 	
                 S32  version;	 	
                 if (fscanf(in, "LindenResource\nversion %d\n", &version))	 	
                 {	 	
                         if (2 == version)	 	
                         {
								// *NOTE: This buffer size is hard coded into scanf() below.
                                 char label[MAX_STRING];		/* Flawfinder: ignore */	 	
                                 char value[MAX_STRING];		/* Flawfinder: ignore */	 	
                                 S32  tokens_read;	 	
                                 while (fgets(buf, 1024, in))	 	
                                 {	 	
                                         label[0] = '\0';	 	
                                         value[0] = '\0';	 	
                                         tokens_read = sscanf(	/* Flawfinder: ignore */
											 buf,
											 "%254s %254s\n",
											 label, value);	 	

                                         llinfos << "got: " << label << " = " << value	 	
                                                         << llendl;	 	

                                         if (EOF == tokens_read)	 	
                                         {	 	
                                                 fclose(in);	 	
                                                 error_message = llformat("corrupt resource file: %s", src_filename.c_str());
												 args["FILE"] = src_filename;
												 upload_error(error_message, "CorruptResourceFile", filename, args);
                                                 return;
                                         }	 	

                                         if (2 == tokens_read)	 	
                                         {	 	
                                                 if (! strcmp("type", label))	 	
                                                 {	 	
                                                         asset_type = (LLAssetType::EType)(atoi(value));	 	
                                                 }	 	
                                         }	 	
                                         else	 	
                                         {	 	
                                                 if (! strcmp("_DATA_", label))	 	
                                                 {	 	
                                                         // below is the data section	 	
                                                         break;	 	
                                                 }	 	
                                         }	 	
                                         // other values are currently discarded	 	
                                 }	 	

                         }	 	
                         else	 	
                         {	 	
                                 fclose(in);	 	
                                 error_message = llformat("unknown linden resource file version in file: %s", src_filename.c_str());
								 args["FILE"] = src_filename;
								 upload_error(error_message, "UnknownResourceFileVersion", filename, args);
                                 return;
                         }	 	
                 }	 	
                 else	 	
                 {	 	
                         // this is an original binary formatted .lin file	 	
                         // start over at the beginning of the file	 	
                         fseek(in, 0, SEEK_SET);	 	

                         const S32 MAX_ASSET_DESCRIPTION_LENGTH = 256;	 	
                         const S32 MAX_ASSET_NAME_LENGTH = 64;	 	
                         S32 header_size = 34 + MAX_ASSET_DESCRIPTION_LENGTH + MAX_ASSET_NAME_LENGTH;	 	
                         S16     type_num;	 	

                         // read in and throw out most of the header except for the type	 	
                         if (fread(buf, header_size, 1, in) != 1)
						 {
							 llwarns << "Short read" << llendl;
						 }
                         memcpy(&type_num, buf + 16, sizeof(S16));		/* Flawfinder: ignore */	 	
                         asset_type = (LLAssetType::EType)type_num;	 	
                 }	 	

                 // copy the file's data segment into another file for uploading	 	
                 LLFILE* out = LLFile::fopen(filename, "wb");		/* Flawfinder: ignore */	
                 if (out)	 	
                 {	 	
                         while((read = fread(buf, 1, 16384, in)))		/* Flawfinder: ignore */	 	
                         {	 	
							 if (fwrite(buf, 1, read, out) != read)
							 {
								 llwarns << "Short write" << llendl;
							 }
                         }	 	
                         fclose(out);	 	
                 }	 	
                 else	 	
                 {	 	
                         fclose(in);	 	
                         error_message = llformat( "Unable to create output file: %s", filename.c_str());
						 args["FILE"] = filename;
						 upload_error(error_message, "UnableToCreateOutputFile", filename, args);
                         return;
                 }	 	

                 fclose(in);	 	
         }	 	
         else	 	
         {	 	
                 llinfos << "Couldn't open .lin file " << src_filename << llendl;	 	
         }	 	
	}
	else if (exten == "bvh")
	{
		// <edit> THE FUCK WE DON'T
		//error_message = llformat("We do not currently support bulk upload of animation files\n");
		//upload_error(error_message, "DoNotSupportBulkAnimationUpload", filename, args);
		//return;
		asset_type = LLAssetType::AT_ANIMATION;
		S32 file_size;
		LLAPRFile fp;
		
		if(!fp.open(src_filename, LL_APR_RB, LLAPRFile::local, &file_size))
		{
			args["ERROR_MESSAGE"] = llformat("Couldn't read file %s\n", src_filename.c_str());
			LLNotifications::instance().add("ErrorMessage", args);
			return;
		}
		char* file_buffer = new char[file_size + 1];
		if(!fp.read(file_buffer, file_size))
		{
			fp.close();
			delete[] file_buffer;
			args["ERROR_MESSAGE"] = llformat("Couldn't read file %s\n", src_filename.c_str());
			LLNotifications::instance().add("ErrorMessage", args);
			return;
		}
		LLBVHLoader* loaderp = new LLBVHLoader(file_buffer);
		if(!loaderp->isInitialized())
		{
			fp.close();
			delete[] file_buffer;
			args["ERROR_MESSAGE"] = llformat("Couldn't convert file %s to internal animation format\n", src_filename.c_str());
			LLNotifications::instance().add("ErrorMessage", args);
			return;
		}
		S32 buffer_size = loaderp->getOutputSize();
		U8* buffer = new U8[buffer_size];
		LLDataPackerBinaryBuffer dp(buffer, buffer_size);
		loaderp->serialize(dp);
		LLAPRFile apr_file;
		apr_file.open(filename, LL_APR_WB, LLAPRFile::local);
		apr_file.write(buffer, buffer_size);
		delete[] file_buffer;
		delete[] buffer;
		fp.close();
		apr_file.close();
		// </edit>
	}
	// <edit>
	// <edit>
	else if(exten == "ogg")
	{
		asset_type = LLAssetType::AT_SOUND;  // tag it as audio
		filename = src_filename;
	}
	// </edit>
	else if (exten == "animatn")
	{
		asset_type = LLAssetType::AT_ANIMATION;
		filename = src_filename;
	}
	else if(exten == "jp2" || exten == "j2k" || exten == "j2c")
	{
		asset_type = LLAssetType::AT_TEXTURE;
		filename = src_filename;
	}
	else if(exten == "gesture")
	{
		asset_type = LLAssetType::AT_GESTURE;
		filename = src_filename;
	}
	else if(exten == "notecard")
	{
		asset_type = LLAssetType::AT_NOTECARD;
		filename = src_filename;
	}
	else if(exten == "lsl")
	{
		asset_type = LLAssetType::AT_LSL_TEXT;
		filename = src_filename;
	}
	else if(exten == "eyes" || exten == "gloves" || exten == "hair" || exten == "jacket" || exten == "pants" || exten == "shape" || exten == "shirt" || exten == "shoes" || exten == "skin" || exten == "skirt" || exten == "socks" || exten == "underpants" || exten == "undershirt" || exten == "bodypart" || exten == "clothing")
	{
		asset_type = LLAssetType::AT_CLOTHING;
		filename = src_filename;
	}
	// </edit>
	else
	{
		// Unknown extension
		// *TODO: Translate?
		error_message = llformat("Unknown file extension .%s\nExpected .wav, .tga, .bmp, .jpg, .jpeg, or .bvh", exten.c_str());
		error = TRUE;;
	}

	// gen a new transaction ID for this asset
	tid.generate();

	if (!error)
	{
		uuid = tid.makeAssetID(gAgent.getSecureSessionID());
		// copy this file into the vfs for upload
		S32 file_size;
		LLAPRFile infile ;
		infile.open(filename, LL_APR_RB, LLAPRFile::local, &file_size);
		if (infile.getFileHandle())
		{
			LLVFile file(gVFS, uuid, asset_type, LLVFile::WRITE);

			file.setMaxSize(file_size);

			const S32 buf_size = 65536;
			U8 copy_buf[buf_size];
			while ((file_size = infile.read(copy_buf, buf_size)))
			{
				file.write(copy_buf, file_size);
			}
		}
		else
		{
			error_message = llformat( "Unable to access output file: %s", filename.c_str());
			error = TRUE;
		}
	}

	if (!error)
	{
		std::string t_disp_name = display_name;
		if (t_disp_name.empty())
		{
			t_disp_name = src_filename;
		}
		// <edit> hack to create scripts and gestures
		if(exten == "lsl" || exten == "gesture" || exten == "notecard") // added notecard Oct 15 2009
		{
			LLInventoryType::EType inv_type = LLInventoryType::IT_GESTURE;
			if(exten == "lsl") inv_type = LLInventoryType::IT_LSL;
			else if(exten == "gesture") inv_type = LLInventoryType::IT_GESTURE;
			else if(exten == "notecard") inv_type = LLInventoryType::IT_NOTECARD;
			create_inventory_item(	gAgent.getID(),
									gAgent.getSessionID(),
									gInventory.findCategoryUUIDForType(asset_type),
									LLTransactionID::tnull,
									name,
									uuid.asString(), // fake asset id, but in vfs
									asset_type,
									inv_type,
									NOT_WEARABLE,
									PERM_ITEM_UNRESTRICTED,
									new NewResourceItemCallback);
		}
		else
		// </edit>
		upload_new_resource(tid, asset_type, name, desc, compression_info, // tid
				    destination_folder_type, inv_type, next_owner_perms, group_perms, everyone_perms,
				    display_name, callback, expected_upload_cost, userdata);
	}
	else
	{
		llwarns << error_message << llendl;
		LLSD args;
		args["ERROR_MESSAGE"] = error_message;
		LLNotifications::instance().add("ErrorMessage", args);
		if(LLFile::remove(filename) == -1)
		{
			lldebugs << "unable to remove temp file" << llendl;
		}
		LLFilePicker::instance().reset();
	}
}
// <edit>
void temp_upload_callback(const LLUUID& uuid, void* user_data, S32 result, LLExtStat ext_status) // StoreAssetData callback (fixed)
{
	LLResourceData* data = (LLResourceData*)user_data;
	
	if(result >= 0)
	{
		LLUUID item_id;
		item_id.generate();
		LLPermissions* perms = new LLPermissions();
		perms->set(LLPermissions::DEFAULT);
		perms->setOwnerAndGroup(LLUUID::null, LLUUID::null, LLUUID::null, false);
		perms->setMaskBase(0);
		perms->setMaskEveryone(0);
		perms->setMaskGroup(0);
		perms->setMaskNext(0);
		perms->setMaskOwner(0);
		LLViewerInventoryItem* item = new LLViewerInventoryItem(
				item_id,
				gLocalInventoryRoot,
				*perms,
				uuid,
				(LLAssetType::EType)data->mAssetInfo.mType,
				(LLInventoryType::EType)data->mInventoryType,
				data->mAssetInfo.getName(),
				data->mAssetInfo.getDescription(),
				LLSaleInfo::DEFAULT,
				0,
				0);
		LLLocalInventory::addItem(item);
	}
	else 
	{
		LLSD args;
		args["FILE"] = LLInventoryType::lookupHumanReadable(data->mInventoryType);
		args["REASON"] = std::string(LLAssetStorage::getErrorString(result));
		LLNotifications::instance().add("CannotUploadReason", args);
	}

	LLUploadDialog::modalUploadFinished();
	delete data;
}
// <edit>
void upload_done_callback(const LLUUID& uuid, void* user_data, S32 result, LLExtStat ext_status) // StoreAssetData callback (fixed)
{
	LLResourceData* data = (LLResourceData*)user_data;
	S32 expected_upload_cost = data ? data->mExpectedUploadCost : 0;
	//LLAssetType::EType pref_loc = data->mPreferredLocation;
	BOOL is_balance_sufficient = TRUE;

	if(result >= 0)
	{
		LLAssetType::EType dest_loc = (data->mPreferredLocation == LLAssetType::AT_NONE) ? data->mAssetInfo.mType : data->mPreferredLocation;

		if (LLAssetType::AT_SOUND == data->mAssetInfo.mType ||
			LLAssetType::AT_TEXTURE == data->mAssetInfo.mType ||
			LLAssetType::AT_ANIMATION == data->mAssetInfo.mType)
		{
			// Charge the user for the upload.
			LLViewerRegion* region = gAgent.getRegion();

			if(!(can_afford_transaction(expected_upload_cost)))
			{
				LLFloaterBuyCurrency::buyCurrency(
					llformat("Uploading %s costs",
							 data->mAssetInfo.getName().c_str()), // *TODO: Translate
					expected_upload_cost);
				is_balance_sufficient = FALSE;
			}
			else if(region)
			{
				// Charge user for upload
				gStatusBar->debitBalance(expected_upload_cost);
				
				LLMessageSystem* msg = gMessageSystem;
				msg->newMessageFast(_PREHASH_MoneyTransferRequest);
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
				msg->nextBlockFast(_PREHASH_MoneyData);
				msg->addUUIDFast(_PREHASH_SourceID, gAgent.getID());
				msg->addUUIDFast(_PREHASH_DestID, LLUUID::null);
				msg->addU8("Flags", 0);
				// we tell the sim how much we were expecting to pay so it
				// can respond to any discrepancy
				msg->addS32Fast(_PREHASH_Amount, expected_upload_cost);
				msg->addU8Fast(_PREHASH_AggregatePermNextOwner, (U8)LLAggregatePermissions::AP_EMPTY);
				msg->addU8Fast(_PREHASH_AggregatePermInventory, (U8)LLAggregatePermissions::AP_EMPTY);
				msg->addS32Fast(_PREHASH_TransactionType, TRANS_UPLOAD_CHARGE);
				msg->addStringFast(_PREHASH_Description, NULL);
				msg->sendReliable(region->getHost());
			}
		}

		if(is_balance_sufficient)
		{
			// Actually add the upload to inventory
			llinfos << "Adding " << uuid << " to inventory." << llendl;
			LLUUID folder_id(gInventory.findCategoryUUIDForType(dest_loc));
			if(folder_id.notNull())
			{
				U32 next_owner_perms = data->mNextOwnerPerm;
				if(PERM_NONE == next_owner_perms)
				{
					next_owner_perms = PERM_MOVE | PERM_TRANSFER;
				}
				create_inventory_item(gAgent.getID(), gAgent.getSessionID(),
					folder_id, data->mAssetInfo.mTransactionID, data->mAssetInfo.getName(),
					data->mAssetInfo.getDescription(), data->mAssetInfo.mType,
					data->mInventoryType, NOT_WEARABLE, next_owner_perms,
					LLPointer<LLInventoryCallback>(NULL));
			}
			else
			{
				llwarns << "Can't find a folder to put it in" << llendl;
			}
		}
	}
	else // 	if(result >= 0)
	{
		LLSD args;
		args["FILE"] = LLInventoryType::lookupHumanReadable(data->mInventoryType);
		args["REASON"] = std::string(LLAssetStorage::getErrorString(result));
		LLNotifications::instance().add("CannotUploadReason", args);
	}

	LLUploadDialog::modalUploadFinished();
	delete data;

	// *NOTE: This is a pretty big hack. What this does is check the
	// file picker if there are any more pending uploads. If so,
	// upload that file.
	/* Agreed, let's not use it. -HgB
	const std::string& next_file = LLFilePicker::instance().getNextFile();
	if(is_balance_sufficient && !next_file.empty())
	{
		std::string asset_name = gDirUtilp->getBaseFileName(next_file, true);
		LLStringUtil::replaceNonstandardASCII( asset_name, '?' );
		LLStringUtil::replaceChar(asset_name, '|', '?');
		LLStringUtil::stripNonprintable(asset_name);
		LLStringUtil::trim(asset_name);

		std::string display_name = LLStringUtil::null;
		LLAssetStorage::LLStoreAssetCallback callback = NULL;
		void *userdata = NULL;
		upload_new_resource(next_file, asset_name, asset_name,	// file
				    0, LLAssetType::AT_NONE, LLInventoryType::IT_NONE,
				    PERM_NONE, PERM_NONE, PERM_NONE,
				    display_name,
				    callback,
				    expected_upload_cost, // assuming next in a group of uploads is of roughly the same type, i.e. same upload cost
				    userdata);
	}
	*/
}

void upload_new_resource(const LLTransactionID &tid, LLAssetType::EType asset_type,
			 std::string name,
			 std::string desc, S32 compression_info,
			 LLAssetType::EType destination_folder_type,
			 LLInventoryType::EType inv_type,
			 U32 next_owner_perms,
			 U32 group_perms,
			 U32 everyone_perms,
			 const std::string& display_name,
			 LLAssetStorage::LLStoreAssetCallback callback,
			 S32 expected_upload_cost,
			 void *userdata)
{
	if(gDisconnected)
	{
		return ;
	}

	LLAssetID uuid = tid.makeAssetID(gAgent.getSecureSessionID());
	
	if( LLAssetType::AT_SOUND == asset_type )
	{
		LLViewerStats::getInstance()->incStat(LLViewerStats::ST_UPLOAD_SOUND_COUNT );
	}
	else
	if( LLAssetType::AT_TEXTURE == asset_type )
	{
		LLViewerStats::getInstance()->incStat(LLViewerStats::ST_UPLOAD_TEXTURE_COUNT );
	}
	else
	if( LLAssetType::AT_ANIMATION == asset_type)
	{
		LLViewerStats::getInstance()->incStat(LLViewerStats::ST_UPLOAD_ANIM_COUNT );
	}

	if(LLInventoryType::IT_NONE == inv_type)
	{
		inv_type = LLInventoryType::defaultForAssetType(asset_type);
	}
	LLStringUtil::stripNonprintable(name);
	LLStringUtil::stripNonprintable(desc);
	if(name.empty())
	{
		name = "(No Name)";
	}
	if(desc.empty())
	{
		desc = "(No Description)";
	}
	
	// At this point, we're ready for the upload.
	std::string upload_message = "Uploading...\n\n";
	upload_message.append(display_name);
	LLUploadDialog::modalUploadDialog(upload_message);

	llinfos << "*** Uploading: " << llendl;
	llinfos << "Type: " << LLAssetType::lookup(asset_type) << llendl;
	llinfos << "UUID: " << uuid << llendl;
	llinfos << "Name: " << name << llendl;
	llinfos << "Desc: " << desc << llendl;
	llinfos << "Expected Upload Cost: " << expected_upload_cost << llendl;
	lldebugs << "Folder: " << gInventory.findCategoryUUIDForType((destination_folder_type == LLAssetType::AT_NONE) ? asset_type : destination_folder_type) << llendl;
	lldebugs << "Asset Type: " << LLAssetType::lookup(asset_type) << llendl;
	std::string url = gAgent.getRegion()->getCapability("NewFileAgentInventory");
	// <edit>
	BOOL temporary = gSavedSettings.getBOOL("TemporaryUpload");
	gSavedSettings.setBOOL("TemporaryUpload",FALSE);
	if (!url.empty() && !temporary)
	// </edit>
	{
		llinfos << "New Agent Inventory via capability" << llendl;
		LLSD body;
		body["folder_id"] = gInventory.findCategoryUUIDForType((destination_folder_type == LLAssetType::AT_NONE) ? asset_type : destination_folder_type);
		body["asset_type"] = LLAssetType::lookup(asset_type);
		body["inventory_type"] = LLInventoryType::lookup(inv_type);
		body["name"] = name;
		body["description"] = desc;
		body["next_owner_mask"] = LLSD::Integer(next_owner_perms);
		body["group_mask"] = LLSD::Integer(group_perms);
		body["everyone_mask"] = LLSD::Integer(everyone_perms);
		body["expected_upload_cost"] = LLSD::Integer(expected_upload_cost);
		
		//std::ostringstream llsdxml;
		//LLSDSerialize::toPrettyXML(body, llsdxml);
		//llinfos << "posting body to capability: " << llsdxml.str() << llendl;

		LLHTTPClient::post(url, body, new LLNewAgentInventoryResponder(body, uuid, asset_type));
	}
	else
	{
		// <edit>
		if(!temporary)
		{
			llinfos << "NewAgentInventory capability not found, new agent inventory via asset system." << llendl;
			// check for adequate funds
			// TODO: do this check on the sim
			if (LLAssetType::AT_SOUND == asset_type ||
				LLAssetType::AT_TEXTURE == asset_type ||
				LLAssetType::AT_ANIMATION == asset_type)
			{
				S32 balance = gStatusBar->getBalance();
				if (balance < expected_upload_cost)
				{
					// insufficient funds, bail on this upload
					LLFloaterBuyCurrency::buyCurrency("Uploading costs", expected_upload_cost);
					return;
				}
			}
		}

		LLResourceData* data = new LLResourceData;
		data->mAssetInfo.mTransactionID = tid;
		data->mAssetInfo.mUuid = uuid;
		data->mAssetInfo.mType = asset_type;
		data->mAssetInfo.mCreatorID = gAgentID;
		data->mInventoryType = inv_type;
		data->mNextOwnerPerm = next_owner_perms;
		data->mExpectedUploadCost = expected_upload_cost;
		data->mUserData = userdata;
		data->mAssetInfo.setName(name);
		data->mAssetInfo.setDescription(desc);
		data->mPreferredLocation = destination_folder_type;

		LLAssetStorage::LLStoreAssetCallback asset_callback = temporary ? &temp_upload_callback : &upload_done_callback;
		if (callback)
		{
			asset_callback = callback;
		}
		gAssetStorage->storeAssetData(data->mAssetInfo.mTransactionID, data->mAssetInfo.mType,
										asset_callback,
										(void*)data,
										temporary,
										TRUE,
										temporary);
	}
}


void init_menu_file()
{
	(new LLFileUploadImage())->registerListener(gMenuHolder, "File.UploadImage");
	(new LLFileUploadSound())->registerListener(gMenuHolder, "File.UploadSound");
	(new LLFileUploadAnim())->registerListener(gMenuHolder, "File.UploadAnim");
	(new LLFileUploadBulk())->registerListener(gMenuHolder, "File.UploadBulk");
	// <edit>
	(new LLFileImportXML())->registerListener(gMenuHolder, "File.ImportXML");
	(new LLFileEnableImportXML())->registerListener(gMenuHolder, "File.EnableImportXML");
	// </edit>
	(new LLFileCloseWindow())->registerListener(gMenuHolder, "File.CloseWindow");
	(new LLFileCloseAllWindows())->registerListener(gMenuHolder, "File.CloseAllWindows");
	(new LLFileEnableCloseWindow())->registerListener(gMenuHolder, "File.EnableCloseWindow");
	(new LLFileEnableCloseAllWindows())->registerListener(gMenuHolder, "File.EnableCloseAllWindows");
	// <edit>
	(new LLFileMinimizeAllWindows())->registerListener(gMenuHolder, "File.MinimizeAllWindows");
	// </edit>
	(new LLFileSaveTexture())->registerListener(gMenuHolder, "File.SaveTexture");
	(new LLFileTakeSnapshot())->registerListener(gMenuHolder, "File.TakeSnapshot");
	(new LLFileTakeSnapshotToDisk())->registerListener(gMenuHolder, "File.TakeSnapshotToDisk");
	(new LLFileQuit())->registerListener(gMenuHolder, "File.Quit");
	(new LLFileLogOut())->registerListener(gMenuHolder, "File.LogOut");
	//Emerald has a second llFileSaveTexture here... Same as the original. Odd. -HgB
	(new LLFileEnableUpload())->registerListener(gMenuHolder, "File.EnableUpload");
	(new LLFileEnableSaveAs())->registerListener(gMenuHolder, "File.EnableSaveAs");
}

// <edit>
void NewResourceItemCallback::fire(const LLUUID& new_item_id)
{
	LLViewerInventoryItem* new_item = (LLViewerInventoryItem*)gInventory.getItem(new_item_id);
	if(!new_item) return;
	LLUUID vfile_id = LLUUID(new_item->getDescription());
	if(vfile_id.isNull()) return;
	new_item->setDescription("(No Description)");
	new_item->updateServer(FALSE);
	gInventory.updateItem(new_item);
	gInventory.notifyObservers();

	std::string agent_url;
	LLSD body;
	body["item_id"] = new_item_id;
	
	if(new_item->getType() == LLAssetType::AT_GESTURE)
	{
		agent_url = gAgent.getRegion()->getCapability("UpdateGestureAgentInventory");
	}
	else if(new_item->getType() == LLAssetType::AT_LSL_TEXT)
	{
		agent_url = gAgent.getRegion()->getCapability("UpdateScriptAgent");
		body["target"] = "lsl2";
	}
	else if(new_item->getType() == LLAssetType::AT_NOTECARD)
	{
		agent_url = gAgent.getRegion()->getCapability("UpdateNotecardAgentInventory");
	}
	else
	{
		return;
	}
	
	if(agent_url.empty()) return;
	LLHTTPClient::post(agent_url, body,
	new LLUpdateAgentInventoryResponder(body, vfile_id, new_item->getType()));
}
// </edit>
