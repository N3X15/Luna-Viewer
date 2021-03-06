#include "llviewerprecompiledheaders.h"

#include "hippogridmanager.h"

#include <cctype>
#include <climits>
#include <cstring>
#include <algorithm>

#include <stdtypes.h>
#include "lldir.h"
#include "lleconomy.h"
#include "llerror.h"
#include "llfile.h"
#include "llhttpclient.h"
#include "llsdserialize.h"
#include "llviewercontrol.h"
#include "llweb.h"

#include "hipporestrequest.h"


// ********************************************************************
// Global Variables

HippoGridManager *gHippoGridManager = 0;

HippoGridInfo HippoGridInfo::FALLBACK_GRIDINFO("");



// ********************************************************************
// ********************************************************************
// HippoGridInfo
// ********************************************************************
// ********************************************************************


// ********************************************************************
// Initialize

HippoGridInfo::HippoGridInfo(const std::string &gridNick) :
	mPlatform(PLATFORM_OTHER),
	mGridNick(gridNick),
	mVoiceConnector("SLVoice"),
	mRenderCompat(true),
	mMaxAgentGroups(-1),
	mVersion(0),
	mCurrencySymbol("OS$"),
	mRealCurrencySymbol("US$"),
	mDirectoryFee(30)
{
	cleanUpGridNick(mGridNick);
}


void HippoGridInfo::setPlatform(Platform platform)
{
	mPlatform = platform;
	if (mPlatform == PLATFORM_SECONDLIFE) mCurrencySymbol = "L$";
}


void HippoGridInfo::setPlatform(const std::string &platform)
{
	std::string tmp = platform;
	for (unsigned i=0; i<platform.size(); i++)
		tmp[i] = tolower(tmp[i]);

	if (tmp == "opensim") {
		setPlatform(PLATFORM_OPENSIM);
	} else if (tmp == "secondlife") {
		setPlatform(PLATFORM_SECONDLIFE);
	} else {
		setPlatform(PLATFORM_OTHER);
		llwarns << "Unknown platform '" << platform << "'." << llendl;
	}
}


// ********************************************************************
// Grid Info

std::string HippoGridInfo::getSearchUrl(SearchType ty) const
{
	if ((mPlatform == PLATFORM_SECONDLIFE) || mSearchUrl.empty()) {
		// Second Life defaults
		if (ty == SEARCH_ALL_EMPTY) {
			return gSavedSettings.getString("SearchURLDefault");
		} else if (ty == SEARCH_ALL_QUERY) {
			return gSavedSettings.getString("SearchURLQuery");
		} else if (ty == SEARCH_ALL_TEMPLATE) {
			return gSavedSettings.getString("SearchURLSuffix2");
		} else {
			llinfos << "Illegal search URL type " << ty << llendl;
			return "";
		}
	} else {
		// OpenSim and other
		if (ty == SEARCH_ALL_EMPTY) {
			return (mSearchUrl + "panel=All&");
		} else if (ty == SEARCH_ALL_QUERY) {
			return (mSearchUrl + "q=[QUERY]&s=[COLLECTION]&");
		} else if (ty == SEARCH_ALL_TEMPLATE) {
			return "lang=[LANG]&m=[MATURE]&t=[TEEN]&region=[REGION]&x=[X]&y=[Y]&z=[Z]&session=[SESSION]";
		} else {
			llinfos << "Illegal search URL type " << ty << llendl;
			return "";
		}
	}
}


//static
void HippoGridInfo::onXmlElementStart(void *userData, const XML_Char *name, const XML_Char **atts)
{
	HippoGridInfo *self = (HippoGridInfo*)userData;
	if (strcasecmp(name, "gridnick") == 0)
		self->mXmlState = XML_GRIDNICK;
	else if (strcasecmp(name, "gridname") == 0)
		self->mXmlState = XML_GRIDNAME;
	else if (strcasecmp(name, "platform") == 0)
		self->mXmlState = XML_PLATFORM;
	else if ((strcasecmp(name, "login") == 0) || (strcasecmp(name, "loginuri") == 0))
		self->mXmlState = XML_LOGINURI;
	else if ((strcasecmp(name, "welcome") == 0) || (strcasecmp(name, "loginpage") == 0))
		self->mXmlState = XML_LOGINPAGE;
	else if ((strcasecmp(name, "economy") == 0) || (strcasecmp(name, "helperuri") == 0))
		self->mXmlState = XML_HELPERURI;
	else if ((strcasecmp(name, "about") == 0) || (strcasecmp(name, "website") == 0))
		self->mXmlState = XML_WEBSITE;
	else if ((strcasecmp(name, "help") == 0) || (strcasecmp(name, "support") == 0))
		self->mXmlState = XML_SUPPORT;
	else if ((strcasecmp(name, "register") == 0) || (strcasecmp(name, "account") == 0))
		self->mXmlState = XML_REGISTER;
	else if (strcasecmp(name, "password") == 0)
		self->mXmlState = XML_PASSWORD;
	//else if (strcasecmp(name, "search") == 0)
		//self->mXmlState = XML_SEARCH;
}

//static
void HippoGridInfo::onXmlElementEnd(void *userData, const XML_Char *name)
{
	HippoGridInfo *self = (HippoGridInfo*)userData;
	self->mXmlState = XML_VOID;
}

//static
void HippoGridInfo::onXmlCharacterData(void *userData, const XML_Char *s, int len)
{
	HippoGridInfo *self = (HippoGridInfo*)userData;
	switch (self->mXmlState) {

		case XML_GRIDNICK:
			if (self->mGridNick == "") self->mGridNick.assign(s, len);
			cleanUpGridNick(self->mGridNick);
			break;

		case XML_PLATFORM: {
			std::string platform(s, len);
			self->setPlatform(platform);
			break;
		}

		case XML_LOGINURI:
			self->mLoginUri.assign(s, len);
			cleanUpUri(self->mLoginUri);
			break;

		case XML_HELPERURI:
			self->mHelperUri.assign(s, len);
			cleanUpUri(self->mHelperUri);
			break;

		case XML_SEARCH:
			//self->mSearchUrl.assign(s, len);
			//cleanUpQueryUrl(mSearchUrl);
			break;

		case XML_GRIDNAME: self->mGridName.assign(s, len); break;
		case XML_LOGINPAGE: self->mLoginPage.assign(s, len); break;
		case XML_WEBSITE: self->mWebSite.assign(s, len); break;
		case XML_SUPPORT: self->mSupportUrl.assign(s, len); break;
		case XML_REGISTER: self->mRegisterUrl.assign(s, len); break;
		case XML_PASSWORD: self->mPasswordUrl.assign(s, len); break;

		case XML_VOID: break;
	}
}


bool HippoGridInfo::retrieveGridInfo()
{
	if (mLoginUri == "") return false;

	// If last character in uri is not "/"
	std::string uri = mLoginUri;
	if (uri.compare(uri.length()-1, 1, "/") != 0) 
	{
	 	uri += '/';
	}
	std::string reply;
	int result = HippoRestRequest::getBlocking(uri + "get_grid_info", &reply);
	if (result != 200) return false;

	llinfos << "Received: " << reply << llendl;

	bool success = true;
	XML_Parser parser = XML_ParserCreate(0);
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, onXmlElementStart, onXmlElementEnd);
	XML_SetCharacterDataHandler(parser, onXmlCharacterData);
	mXmlState = XML_VOID;
	if (!XML_Parse(parser, reply.data(), reply.size(), TRUE)) {
		llwarns << "XML Parse Error: " << XML_ErrorString(XML_GetErrorCode(parser)) << llendl;
		success = false;
	}
	XML_ParserFree(parser);

	return success;
}


std::string HippoGridInfo::getUploadFee() const
{
	std::string fee;
	formatFee(fee, LLGlobalEconomy::Singleton::getInstance()->getPriceUpload(), true);
	return fee;
}

std::string HippoGridInfo::getGroupCreationFee() const
{
	std::string fee;
	formatFee(fee, LLGlobalEconomy::Singleton::getInstance()->getPriceGroupCreate(), false);
	return fee;
}

std::string HippoGridInfo::getDirectoryFee() const
{
	std::string fee;
	formatFee(fee, mDirectoryFee, true);
	if (fee != "free") fee += "/week";
	return fee;
}

void HippoGridInfo::formatFee(std::string &fee, int cost, bool showFree) const
{
	if (showFree && (cost == 0)) {
		fee = "free";
	} else {
		fee = llformat("%s%d", getCurrencySymbol().c_str(), cost);
	}
}


// ********************************************************************
// Static Helpers

// static
const char *HippoGridInfo::getPlatformString(Platform platform)
{
	static const char *platformStrings[PLATFORM_LAST] = {
		"Other", "OpenSim", "SecondLife"
	};

	if ((platform < PLATFORM_OTHER) || (platform >= PLATFORM_LAST))
		platform = PLATFORM_OTHER;
	return platformStrings[platform];
}


// static
void HippoGridInfo::cleanUpGridNick(std::string &gridnick)
{
	std::string tmp;
	int size = gridnick.size();
	for (int i=0; i<size; i++) {
		char c = gridnick[i];
		if ((c == '_') || isalnum(c)) {
			tmp += tolower(c);
		} else if (isspace(c)) {
			tmp += "_";
		}
	}
	gridnick = tmp;
}

// static
void HippoGridInfo::cleanUpUri(std::string &uri)
{
	std::string::size_type n = uri.rfind('/');
	if ((n == std::string::npos) || (n < 10))
		uri += '/';
}


void HippoGridInfo::initFallback()
{
#ifdef TPV_COMPLIANT
	FALLBACK_GRIDINFO.mGridNick = "secondlife";
	FALLBACK_GRIDINFO.setPlatform(PLATFORM_SECONDLIFE);
	FALLBACK_GRIDINFO.setGridName("secondlife");
	FALLBACK_GRIDINFO.setLoginUri("https://login.agni.lindenlab.com/cgi-bin/login.cgi");
	FALLBACK_GRIDINFO.setLoginPage("http://phoenixviewer.com/app/login/");
	FALLBACK_GRIDINFO.setHelperUri("https://secondlife.com/helpers/");
	FALLBACK_GRIDINFO.setWebSite("http://secondlife.com/");
#else
	FALLBACK_GRIDINFO.mGridNick = "inworldz";
	FALLBACK_GRIDINFO.setPlatform(PLATFORM_OPENSIM);
	FALLBACK_GRIDINFO.setGridName("inworldz");
	FALLBACK_GRIDINFO.setLoginUri("http://inworldz.com:8002/");
	FALLBACK_GRIDINFO.setLoginPage("http://luna.nexisonline.net/login/");
	FALLBACK_GRIDINFO.setHelperUri("https://inworldz.com/helpers/");
	FALLBACK_GRIDINFO.setWebSite("http://inworldz.com/");
#endif
}



// ********************************************************************
// ********************************************************************
// HippoGridManager
// ********************************************************************
// ********************************************************************


// ********************************************************************
// Initialize

HippoGridManager::HippoGridManager() :
	mConnectedGrid(0)
{
}

HippoGridManager::~HippoGridManager()
{
	cleanup();
}


void HippoGridManager::cleanup()
{
	std::map<std::string, HippoGridInfo*>::iterator it, end = mGridInfo.end();
	for (it=mGridInfo.begin(); it != end; ++it) {
		delete it->second;
	}
	mGridInfo.clear();
}


void HippoGridManager::init()
{
	HippoGridInfo::initFallback();
	loadFromFile();

	// !!!### gSavedSettings.getControl("CmdLineLoginURI");
	// !!!### gSavedSettings.getString("CmdLineLoginPage");
	// !!!### gSavedSettings.getString("CmdLineHelperURI");
	// !!!### LLString::compareInsensitive(gGridInfo[grid_index].mLabel, grid_name.c_str()))
}


// ********************************************************************
// Public Access

HippoGridInfo *HippoGridManager::getGrid(const std::string &grid) const
{
	std::map<std::string, HippoGridInfo*>::const_iterator it;
	it = mGridInfo.find(grid);
	if (it != mGridInfo.end()) {
		return it->second;
	} else {
		return 0;
	}
}


HippoGridInfo *HippoGridManager::getCurrentGrid() const
{
	HippoGridInfo *grid = getGrid(mCurrentGrid);
	if (grid) {
		return grid;
	} else {
		return &HippoGridInfo::FALLBACK_GRIDINFO;
	}
}


void HippoGridManager::addGrid(HippoGridInfo *grid)
{
	if (!grid) return;
	const std::string &nick = grid->getGridNick();
	if (nick == "") {
		llwarns << "Ignoring to try adding grid with empty nick." << llendl;
		delete grid;
		return;
	}
	if (mGridInfo.find(nick) != mGridInfo.end()) {
		llwarns << "Ignoring to try adding existing grid " << nick << '.' << llendl;
		delete grid;
		return;
	}
	mGridInfo[nick] = grid;
}


void HippoGridManager::deleteGrid(const std::string &grid)
{
	GridIterator it = mGridInfo.find(grid);
	if (it == mGridInfo.end()) {
		llwarns << "Trying to delete non-existing grid " << grid << '.' << llendl;
		return;
	}
	mGridInfo.erase(it);
	llinfos << "Number of grids now: " << mGridInfo.size() << llendl;
	if (mGridInfo.empty()) llinfos << "Grid info map is empty." << llendl;
	if (grid == mDefaultGrid)
		setDefaultGrid("");	 // sets first grid, if map not empty
	if (grid == mCurrentGrid)
		mCurrentGrid = mDefaultGrid;
}


void HippoGridManager::setDefaultGrid(const std::string &grid)
{
	GridIterator it = mGridInfo.find(grid);
	if (it != mGridInfo.end()) {
		mDefaultGrid = grid;
#ifdef TPV_COMPLIANT
	} else if (mGridInfo.find("secondlife") != mGridInfo.end()) {
		mDefaultGrid = "secondlife";
#endif
	} else if (!mGridInfo.empty()) {
		mDefaultGrid = mGridInfo.begin()->first;
	} else {
		mDefaultGrid = "";
	}
}


void HippoGridManager::setCurrentGrid(const std::string &grid)
{
	GridIterator it = mGridInfo.find(grid);
	if (it != mGridInfo.end()) {
		mCurrentGrid = grid;
	} else if (!mGridInfo.empty()) {
		llwarns << "Unknown grid '" << grid << "'. Setting to default grid." << llendl;
		mCurrentGrid = mDefaultGrid;
	}
}


// ********************************************************************
// Persistent Store

void HippoGridManager::loadFromFile()
{
	// load user grid info
	parseFile(gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "grid_info.xml"), false);
	// merge default grid info, if newer. Force load, if list of grids is empty.
	parseFile(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "default_grids.xml"), !mGridInfo.empty());
	// merge grid info from web site, if newer. Force load, if list of grids is empty.
	parseUrl("http://opensim-viewer.sourceforge.net/db/grids.php", !mGridInfo.empty());

	setDefaultGrid(gSavedSettings.getString("DefaultGrid"));
	setCurrentGrid(gSavedSettings.getString("CmdLineGridChoice"));
}


void HippoGridManager::parseUrl(const char *url, bool mergeIfNewer)
{
	llinfos << "Loading grid info from '" << url << "'." << llendl;

	// query update server
	std::string escaped_url = LLWeb::escapeURL(url);
	LLSD response = LLHTTPClient::blockingGet(url);

	// check response, return on error
	S32 status = response["status"].asInteger();
	if ((status != 200) || !response["body"].isArray()) {
		llinfos << "GridInfo Update failed (" << status << "): "
			<< (response["body"].isString()? response["body"].asString(): "<unknown error>")
			<< llendl;
		return;
	}

	LLSD gridInfo = response["body"];
	parseData(gridInfo, mergeIfNewer);
}

void HippoGridManager::parseFile(const std::string &fileName, bool mergeIfNewer)
{
	llifstream infile;
	infile.open(fileName.c_str());
	if(!infile.is_open()) {
		llwarns << "Cannot find grid info file " << fileName << " to load." << llendl;
		return;
	}

	LLSD gridInfo;
	if (LLSDSerialize::fromXML(gridInfo, infile) <= 0) {
		llwarns << "Unable to parse grid info file " << fileName << '.' << llendl;
		return;
	}

	llinfos << "Loading grid info file " << fileName << '.' << llendl;
	parseData(gridInfo, mergeIfNewer);
}


void HippoGridManager::parseData(LLSD &gridInfo, bool mergeIfNewer)
{
	LLSD::array_const_iterator it, end = gridInfo.endArray();
	for (it = gridInfo.beginArray(); it != end; ++it) {
		LLSD gridMap = *it;
		if (gridMap.has("gridnick") && gridMap.has("loginuri")) {
			std::string gridnick = gridMap["gridnick"];
#ifndef TPV_COMPLIANT
			if(gridnick=="secondlife")
				continue;
#endif
			HippoGridInfo *grid;
			GridIterator it = mGridInfo.find(gridnick);
			bool newGrid = (it == mGridInfo.end());
			if (gridMap.has("version")) {
				int version = gridMap["version"];
				if (version == -1) {
					// delete grid
					if (!newGrid) mGridInfo.erase(it);
					continue;
				} else if (mergeIfNewer && !newGrid && (version <= it->second->getVersion())) {
					// don't update if version is not newer
					continue;
				}
			}
			if (newGrid) {
				// create new grid info
				grid = new HippoGridInfo(gridnick);
			} else {
				// update existing grid info
				grid = it->second;
			}
			grid->setLoginUri(gridMap["loginuri"]);
			if (gridMap.has("platform")) grid->setPlatform(gridMap["platform"]);
			if (gridMap.has("gridname")) grid->setGridName(gridMap["gridname"]);
			if (gridMap.has("lastlogin_fname")) grid->setLastFName(gridMap["lastlogin_fname"]);
			if (gridMap.has("lastlogin_lname")) grid->setLastLName(gridMap["lastlogin_lname"]);
			if (gridMap.has("loginpage")) grid->setLoginPage(gridMap["loginpage"]);
			if (gridMap.has("helperuri")) grid->setHelperUri(gridMap["helperuri"]);
			if (gridMap.has("website")) grid->setWebSite(gridMap["website"]);
			if (gridMap.has("support")) grid->setSupportUrl(gridMap["support"]);
			if (gridMap.has("register")) grid->setRegisterUrl(gridMap["register"]);
			if (gridMap.has("password")) grid->setPasswordUrl(gridMap["password"]);
			//if (gridMap.has("search")) grid->setSearchUrl(gridMap["search"]);
			if (gridMap.has("render_compat")) grid->setRenderCompat(gridMap["render_compat"]);
			if (gridMap.has("version")) grid->setVersion(gridMap["version"]);
			if (newGrid) addGrid(grid);
		}
	}
}


void HippoGridManager::saveFile()
{
	// save default grid to client settings
	gSavedSettings.setString("DefaultGrid", mDefaultGrid);

	// build LLSD
	LLSD gridInfo;
	S32 i = 0;
	GridIterator it, end = mGridInfo.end();
	for (it = mGridInfo.begin(); it != end; ++it, i++) {
		HippoGridInfo *grid = it->second;
		gridInfo[i]["gridnick"] = grid->getGridNick();
		gridInfo[i]["platform"] = HippoGridInfo::getPlatformString(grid->getPlatform());
		gridInfo[i]["gridname"] = grid->getGridName();
		if (!grid->getLastFName().empty()) gridInfo[i]["lastlogin_fname"] = grid->getLastFName();
		if (!grid->getLastLName().empty()) gridInfo[i]["lastlogin_lname"] = grid->getLastLName();
		gridInfo[i]["loginuri"] = grid->getLoginUri();
		gridInfo[i]["loginpage"] = grid->getLoginPage();
		gridInfo[i]["helperuri"] = grid->getHelperUri();
		gridInfo[i]["website"] = grid->getWebSite();
		gridInfo[i]["support"] = grid->getSupportUrl();
		gridInfo[i]["register"] = grid->getRegisterUrl();
		gridInfo[i]["password"] = grid->getPasswordUrl();
		//gridInfo[i]["search"] = grid->getSearchUrl();
		gridInfo[i]["render_compat"] = grid->isRenderCompat();
		gridInfo[i]["version"] = grid->getVersion();
	}

	// write client grid info file
	std::string fileName = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "grid_info.xml");
	llofstream file;
	file.open(fileName.c_str());
	if (file.is_open()) {
		LLSDSerialize::toPrettyXML(gridInfo, file);
		file.close();
		llinfos << "Saved grids to " << fileName << llendl;
	} else {
		llerrs << "Unable to open grid info file: " << fileName << llendl;
	}
}
