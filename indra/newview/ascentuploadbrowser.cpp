/** 
 * @file ascentuploadbrowser.h
 * @Author Duncan Garrett (Hg Beeks)
 * Meant as a replacement to using a system file browser for uploads.
 *
 * Created August 27 2010
 * 
 * ALL SOURCE CODE IS PROVIDED "AS IS." THE CREATOR MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * k ilu bye
 */

#include "llviewerprecompiledheaders.h"

#include "ascentuploadbrowser.h"

//UI Elements
#include "llbutton.h" //Buttons
#include "llcombobox.h" //Combo dropdowns
#include "llscrolllistctrl.h" //List box for filenames
#include "lluictrlfactory.h" //Loads the XUI

// project includes
#include "llresmgr.h"
#include "llsdserialize.h" //XML Parsing - Probably not needed
#include "llviewercontrol.h"
#include "llviewerwindow.h"

///----------------------------------------------------------------------------
/// Local function declarations, constants, enums, and typedefs
///----------------------------------------------------------------------------

ASFloaterUploadBrowser* ASFloaterUploadBrowser::sInstance = NULL;


///----------------------------------------------------------------------------
/// Class LLFloaterAbout
///----------------------------------------------------------------------------

// Default constructor
ASFloaterUploadBrowser::ASFloaterUploadBrowser() 
:	LLFloater(std::string("floater_upload_browser"), std::string("FloaterUploadRect"), LLStringUtil::null)
{	
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_upload_browser.xml");
	mPathName = gDirUtilp->getSkinBaseDir();
	mFilterType = "None";
	mFileList = getChild<LLScrollListCtrl>("file_list");
	mDriveCombo = getChild<LLComboBox>("drive_combo");
	mBookmarkCombo = getChild<LLComboBox>("bookmark_combo");
	
	childSetCommitCallback("file_list", onClickFile, this);
	childSetDoubleClickCallback("file_list", onDoubleClick);
	childSetCommitCallback("file_filter_combo", onUpdateFilter, this);

	//This is so unbelievably shitty I can't believe it -HgB
	std::string drive_letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	S32 index;
	mDriveCombo->removeall();
	for (index = 0; index < 26; index++)
	{
		
		std::string dir = drive_letters.substr(index, index) + ":" + gDirUtilp->getDirDelimiter();
		S32 file_count = gDirUtilp->countFilesInDir(dir, "*.*");
		llinfos << dir << " - " << file_count << llendl;
		if(file_count)
		{
			mDriveCombo->add(dir, ADD_BOTTOM);
		}
	}

	refresh();
	mFileList->sortByColumn(std::string("file_name"), TRUE);
	mFileList->sortByColumn(std::string("file_type"), TRUE);

	
}

// Destroys the object
ASFloaterUploadBrowser::~ASFloaterUploadBrowser()
{
	sInstance = NULL;
}

void ASFloaterUploadBrowser::onClickFile(LLUICtrl* ctrl, void* user_data)
{
	ASFloaterUploadBrowser* panelp = (ASFloaterUploadBrowser*)user_data;
	panelp->refreshUploadOptions();
}

void ASFloaterUploadBrowser::onUpdateFilter(LLUICtrl* ctrl, void* user_data)
{
	ASFloaterUploadBrowser* panelp = (ASFloaterUploadBrowser*)user_data;
	LLComboBox* combo = panelp->getChild<LLComboBox>("file_filter_combo");
	if (combo->getSelectedValue().asString() != panelp->mFilterType)
	{
		panelp->mFilterType = "";
		panelp->mFilterType = combo->getSelectedValue().asString();
		panelp->refresh();
		panelp->mFileList->selectFirstItem();
		panelp->childSetValue("asset_name", "");
	}
}

void ASFloaterUploadBrowser::refreshUploadOptions()
{
	if (!mFileList->isEmpty())
	{
		if(!mFileList->getSelectedIDs().count())
		{
			llinfos << "No selection, clearing field." << llendl;
			childSetValue("asset_name", "");
		}
		else 
		{
			if (mFileList->getFirstSelected()->getColumn(LIST_FILE_TYPE)->getValue().asInteger() == LIST_TYPE_FILE)
			{
				if (mFileList->getAllSelected().size() > 1)
				{
					llinfos << "Selected multiple files." << llendl;
					childSetValue("asset_name", "(Multiple)");
					childSetLabelArg("upload_button", "[COST]", std::string("$L" + (10 * mFileList->getAllSelected().size())));
				}
				else
				{
					llinfos << "Selected a file." << llendl;
					std::string name = mFileList->getFirstSelected()->getColumn(LIST_FILE_NAME)->getValue().asString();
					childSetValue("asset_name", name);
					childSetLabelArg("upload_button", "[COST]", std::string("$L10"));
				}
			}
		}
	}
}

void ASFloaterUploadBrowser::onDoubleClick(void* user_data)
{
	ASFloaterUploadBrowser* panelp = (ASFloaterUploadBrowser*)user_data;
	panelp->handleDoubleClick();
}

void ASFloaterUploadBrowser::handleDoubleClick()
{
	if (mFileList->getFirstSelected()->getColumn(LIST_FILE_TYPE)->getValue().asInteger() == LIST_TYPE_PARENT)
	{
		S32 dirLimiterIndex = mPathName.find_last_of(gDirUtilp->getDirDelimiter());
		mPathName = mPathName.substr(0, dirLimiterIndex);
		refresh();
		mFileList->selectFirstItem();
	}
	else if (mFileList->getFirstSelected()->getColumn(LIST_FILE_TYPE)->getValue().asInteger() == LIST_TYPE_FOLDER)
	{
		mPathName = mPathName + gDirUtilp->getDirDelimiter() + mFileList->getFirstSelected()->getColumn(LIST_FILE_NAME)->getValue().asString();
		refresh();
		mFileList->selectFirstItem();

	}
	childSetValue("asset_name", "");
}

void ASFloaterUploadBrowser::refresh()
{
	std::string filename;
	std::string fullPath = mPathName + gDirUtilp->getDirDelimiter();
	mFileList->deselectAllItems();
	mFileList->deleteAllItems();
	llinfos << "Getting file listing at " << gDirUtilp->getDirName(fullPath) << " side note " << gDirUtilp->getDirName("C:\\") << llendl;
	gDirUtilp->getNextFileInDir(gDirUtilp->getChatLogsDir(),"*", filename, false); //Clears the last file
	bool found = true;
	S32 file_count = 0;
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(fullPath, "*.*", filename, false);
		if(found)
		{
			S32 periodIndex = filename.find_last_of(".");
			std::string extension = filename.substr(periodIndex + 1, filename.length() - 1);
			std::string extensionL = utf8str_tolower(extension);
			LLSD element;
			element["path"] = mPathName + filename;

			LLSD& filename_column = element["columns"][LIST_FILE_NAME];
			filename_column["column"] = "file_name";
			filename_column["font"] = "SANSSERIF";
			filename_column["font-style"] = "NORMAL";	
			
			LLSD& filetype_column = element["columns"][LIST_FILE_TYPE];
			filetype_column["column"] = "file_type";
			filetype_column["type"] = "number";

			LLSD& invtype_column = element["columns"][LIST_INVENTORY_TYPE];
			invtype_column["column"] = "icon_inventory_type";
			invtype_column["type"] = "icon";
			invtype_column["value"] = "inv_folder_trash.tga";

			if (((extensionL == "jpeg")||(extensionL == "jpg")||(extensionL == "tga")
				||(extensionL == "png")||(extensionL == "bmp"))&&((mFilterType == "None")||(mFilterType == "Texture")))
			{
				invtype_column["value"] = "inv_item_texture.tga";
				filename_column["value"] = filename.substr(0, periodIndex);
				filetype_column["value"] = LIST_TYPE_FILE;
			}
			else if ((extensionL == "wav")&&((mFilterType == "None")||(mFilterType == "Sound")))
			{	
				invtype_column["value"] = "inv_item_sound.tga";
				filename_column["value"] = filename.substr(0, periodIndex);
				filetype_column["value"] = LIST_TYPE_FILE;
			}
			else if (((extensionL == "bvh")||(extensionL == "anim"))&&((mFilterType == "None")||(mFilterType == "Animation")))
			{	
				invtype_column["value"] = "inv_item_animation.tga";
				filename_column["value"] = filename.substr(0, periodIndex);
				filetype_column["value"] = LIST_TYPE_FILE;
			}
			else if ((extension == filename.substr(0, filename.length() - 1))&&(filename != "."))
			{
				invtype_column["value"] = "inv_folder_plain_closed.tga";
				filename_column["value"] = filename;
				filetype_column["value"] = LIST_TYPE_FOLDER;
			}
			else if (filename == "..")
			{
				invtype_column["value"] = "inv_folder_plain_open.tga";
				filename_column["value"] = filename;
				filetype_column["value"] = LIST_TYPE_PARENT;
			}
			if (invtype_column["value"].asString() != "inv_folder_trash.tga")
			{
				mFileList->addElement(element, ADD_BOTTOM);
				if (filetype_column["value"].asInteger() == LIST_TYPE_FILE)
				{
					file_count++;
				}
			}
		}
	}
	
	std::string result;
	LLResMgr::getInstance()->getIntegerString(result, file_count);
	childSetTextArg("result_label",  "[COUNT]", result);

	mFileList->sortItems();
	llinfos << "Total files loaded: " << result << "." << llendl;
}

// static
void ASFloaterUploadBrowser::show(void*)
{
	if (!sInstance)
	{
		sInstance = new ASFloaterUploadBrowser();
	}

	sInstance->open();	 /*Flawfinder: ignore*/
}