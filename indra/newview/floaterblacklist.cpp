// <edit>
#include "llviewerprecompiledheaders.h"
#include "floaterblacklist.h"
#include "llaudioengine.h"
#include "llvfs.h"
#include "lluictrlfactory.h"
#include "llsdserialize.h"
#include "llscrolllistctrl.h"
#include "llcheckboxctrl.h"
#include "llfilepicker.h"
#include "llviewerwindow.h"
#include "llviewercontrol.h"
#include "lldate.h"
#include "llagent.h"

LLFloaterBlacklist* LLFloaterBlacklist::sInstance;

std::vector<LLUUID> LLFloaterBlacklist::blacklist_textures;

std::map<LLUUID,LLSD> LLFloaterBlacklist::blacklist_entries;

LLFloaterBlacklist::LLFloaterBlacklist()
:	LLFloater()
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_blacklist.xml");
}
LLFloaterBlacklist::~LLFloaterBlacklist()
{
	sInstance = NULL;
}
// static
void LLFloaterBlacklist::show()
{
	if(sInstance)
		sInstance->open();
	else
	{
		sInstance = new LLFloaterBlacklist();
		sInstance->open();
	}
}
BOOL LLFloaterBlacklist::postBuild()
{
	childSetAction("add_btn", onClickAdd, this);
	childSetAction("clear_btn", onClickClear, this);
	childSetAction("copy_uuid_btn", onClickCopyUUID, this);
	childSetAction("remove_btn", onClickRemove, this);
	childSetAction("save_btn", onClickSave, this);
	childSetAction("load_btn", onClickLoad, this);
	childSetVisible("copy_uuid_btn",false);
	LLComboBox* box = getChild<LLComboBox>("asset_combo");
	box->add("Texture",LLSD(0));
	box->add("Sound",LLSD(1));
	box->add("Animation",LLSD(20));
	refresh();
	return TRUE;
}
void LLFloaterBlacklist::refresh()
{
	
	LLScrollListCtrl* list = getChild<LLScrollListCtrl>("file_list");
	list->clearRows();
	LLSD settings;
	for(std::map<LLUUID,LLSD>::iterator itr = blacklist_entries.begin(); itr != blacklist_entries.end(); ++itr)
	{
		LLSD element;
		std::string agent;
		gCacheName->getFullName(LLUUID(itr->second["entry_agent"].asString()), agent);

		element["id"] = itr->first.asString();
		LLSD& uuid_column = element["columns"][0];
		uuid_column["column"] = "asset_id";
		uuid_column["value"] = itr->first.asString();

		LLSD& name_column = element["columns"][1];
		name_column["column"] = "entry_name";
		name_column["value"] = itr->second["entry_name"].asString();

		LLSD& type_column = element["columns"][2];
		type_column["column"] = "entry_type";
		type_column["value"] = std::string(LLAssetType::lookupHumanReadable( (LLAssetType::EType)itr->second["entry_type"].asInteger() ));

		LLSD& agent_column = element["columns"][3];
		agent_column["column"] = "entry_agent";
		agent_column["value"] = agent;

		LLSD& date_column = element["columns"][4];
		date_column["column"] = "entry_date";
		date_column["value"] = itr->second["entry_date"].asString();
		list->addElement(element, ADD_BOTTOM);
	}
}

// static
void LLFloaterBlacklist::onClickAdd(void* user_data)
{
	LLFloaterBlacklist* floaterp = (LLFloaterBlacklist*)user_data;
	if(!floaterp) return;
	LLUUID add_id = LLUUID(floaterp->childGetValue("id_edit").asString());
	if(add_id.isNull()) return;
	std::string name = floaterp->childGetValue("name_edit").asString();
	if(name.size() == 0) return;
	LLSD indata;
	LLComboBox* mTypeComboBox = floaterp->getChild<LLComboBox>("asset_combo");
	indata["entry_type"] = (LLAssetType::EType)mTypeComboBox->getValue().asInteger();
	indata["entry_name"] = name;
	indata["entry_agent"] = gAgent.getID().asString();

	addEntry(add_id,indata);
}

//static
void LLFloaterBlacklist::addEntry(LLUUID key, LLSD data)
{
	if(key.notNull())
	{
		if(!data.has("entry_type")) 
		{
			LL_WARNS("FloaterBlacklistAdd") << "addEntry called with no entry type, specify LLAssetType::Etype" << LL_ENDL;
			return;
		}
		if(!data.has("entry_name"))
		{
			LL_WARNS("FloaterBlacklistAdd") << "addEntry called with no entry name, specify the name that should appear in the listing for this entry." << LL_ENDL;
			return;
		}

		if(!data.has("entry_date"))
		{
			LLDate* curdate = new LLDate(time_corrected());
			std::string input_date = curdate->asString();
			input_date.replace(input_date.find("T"),1," ");
			input_date.resize(input_date.size() - 1);
			data["entry_date"] = input_date;
		}
		std::string test=data["entry_type"].asString();
		if(data["entry_type"].asString() == "1")
		{
		  //remove sounds
		  LLUUID sound_id=LLUUID(key);
		  gVFS->removeFile(sound_id,LLAssetType::AT_SOUND);
		  std::string wav_path;
//		  if(gDirUtilp->mm_usesnd())
//			      wav_path= gDirUtilp->getExpandedFilename(MM_SNDLOC,sound_id.asString()) + ".dsf";
//		  else
			      wav_path= gDirUtilp->getExpandedFilename(LL_PATH_CACHE,sound_id.asString()) + ".dsf";
		  if(LLAPRFile::isExist(wav_path, LL_APR_RPB))
		    LLAPRFile::remove(wav_path);
		  gAudiop->removeAudioData(sound_id);
		}
		blacklist_entries.insert(std::pair<LLUUID,LLSD>(key,data));
		updateBlacklists();
	}
	else
		LL_WARNS("FloaterBlacklistAdd") << "addEntry called with a null entry key, please specify LLUUID of asset." << LL_ENDL;
}

// static
void LLFloaterBlacklist::onClickClear(void* user_data)
{
	blacklist_entries.clear();
	updateBlacklists();
}
// static
void LLFloaterBlacklist::onClickCopyUUID(void* user_data)
{
	LLFloaterBlacklist* floaterp = (LLFloaterBlacklist*)user_data;
	LLScrollListCtrl* list = floaterp->getChild<LLScrollListCtrl>("file_list");
	gViewerWindow->mWindow->copyTextToClipboard(utf8str_to_wstring(list->getFirstSelected()->getColumn(0)->getValue().asString()));
}
// static
void LLFloaterBlacklist::onClickRemove(void* user_data)
{
	LLFloaterBlacklist* floaterp = (LLFloaterBlacklist*)user_data;
	LLScrollListCtrl* list = floaterp->getChild<LLScrollListCtrl>("file_list");
	LLUUID selected_id;
	if(list->getFirstSelected())
	{
	  LLScrollListItem* item = list->getFirstSelected();
	  selected_id = item->getColumn(0)->getValue().asUUID();
	  if(selected_id.isNull()) return;
	  list->deleteSingleItem(list->getFirstSelectedIndex());
	  blacklist_entries.erase(selected_id);
	  updateBlacklists();

	}
}
// static
void LLFloaterBlacklist::loadFromSave()
{
	std::string file_name = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "floater_blist_settings.xml");
	llifstream xml_file(file_name);
	if(!xml_file.is_open()) return;
	LLSD data;
	if(LLSDSerialize::fromXML(data, xml_file) >= 1)
	{
		for(LLSD::map_iterator itr = data.beginMap(); itr != data.endMap(); ++itr)
		{
			blacklist_entries.insert(std::pair<LLUUID,LLSD>(LLUUID(itr->first),itr->second));
		}
		updateBlacklists();
	}
	xml_file.close();
}

//static
void LLFloaterBlacklist::updateBlacklists()
{
	if(gAssetStorage)
	{
		blacklist_textures.clear();
		gAssetStorage->mBlackListedAsset.clear();
		for(std::map<LLUUID,LLSD>::iterator itr = blacklist_entries.begin(); itr != blacklist_entries.end(); ++itr)
		{
			if(blacklist_entries[itr->first]["entry_type"].asString() == "0")
			{
				blacklist_textures.push_back(LLUUID(itr->first));
			}
			else
			{
				gAssetStorage->mBlackListedAsset.push_back(LLUUID(itr->first));
			}
		}
		saveToDisk();
		LLFloaterBlacklist* instance = LLFloaterBlacklist::getInstance();
		if(instance)
			instance->refresh();
	}
}

//static
void LLFloaterBlacklist::saveToDisk()
{
	std::string file_name = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "floater_blist_settings.xml");
	llofstream export_file(file_name);
	LLSD data;
	for(std::map<LLUUID,LLSD>::iterator itr = blacklist_entries.begin(); itr != blacklist_entries.end(); ++itr)
	{
		data[itr->first.asString()] = itr->second;
	}
	LLSDSerialize::toPrettyXML(data, export_file);
	export_file.close();
}

//static
void LLFloaterBlacklist::onClickSave(void* user_data)
{
	LLFilePicker& file_picker = LLFilePicker::instance();
	if(file_picker.getSaveFile( LLFilePicker::FFSAVE_BLACKLIST, LLDir::getScrubbedFileName("untitled.blacklist")))
	{
		std::string file_name = file_picker.getFirstFile();
		llofstream export_file(file_name);
		LLSD data;
		for(std::map<LLUUID,LLSD>::iterator itr = blacklist_entries.begin(); itr != blacklist_entries.end(); ++itr)
		{
			data[itr->first.asString()] = itr->second;
		}
		LLSDSerialize::toPrettyXML(data, export_file);
		export_file.close();
	}
}

//static
void LLFloaterBlacklist::onClickLoad(void* user_data)
{
	LLFilePicker& file_picker = LLFilePicker::instance();
	if(file_picker.getOpenFile(LLFilePicker::FFLOAD_BLACKLIST))
	{
		std::string file_name = file_picker.getFirstFile();
		llifstream xml_file(file_name);
		if(!xml_file.is_open()) return;
		LLSD data;
		if(LLSDSerialize::fromXML(data, xml_file) >= 1)
		{
			for(LLSD::map_iterator itr = data.beginMap(); itr != data.endMap(); ++itr)
			{
				blacklist_entries.insert(std::pair<LLUUID,LLSD>(LLUUID(itr->first),itr->second));
			}
			updateBlacklists();
		}
		xml_file.close();
	}
}
// </edit>
