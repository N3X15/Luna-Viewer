/**
 * @file LuaAvatar_f.cpp
 * @brief Luna Viewer Lua Framework:  Appearance bindings
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 Luna Contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "llviewerprecompiledheaders.h"

#include "LuaBase_f.h"
#include "LuaAvatar_f.h"

#include "llagent.h"
#include "llvoavatar.h"
#include "llviewerobjectlist.h"
#include "llwearable.h"
#include "llwearablelist.h"
#include "llcharacter.h"
#include "llfloatercustomize.h"


bool HasPermissions(LLViewerInventoryItem* item);

std::map<std::string,double> LuaDumpVisualParams()
{
	std::map<std::string,double> VisParams;
	VisParams.clear();
	LLViewerInventoryItem* item;
	item = (LLViewerInventoryItem*)gAgent.getWearableInventoryItem(WT_SHAPE);
	if(!HasPermissions(item))
	{
		LuaError("LuaDumpVisualParams():  You do not have permission to edit this shape.");
		return VisParams;
	}

	LLVOAvatar *av=gAgent.getAvatarObject();
	for (LLVisualParam *param = av->getFirstVisualParam(); 
		param;
		param = av->getNextVisualParam())
	{
		VisParams.insert(std::pair<std::string,double>(param->getName(),(double)param->getCurrentWeight()));
	}
	return VisParams;
}

double getParamDefaultWeight(const LLUUID &avid,const char* paramname)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return 0;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	LLVisualParam *p=av->getVisualParam(paramname);
	if(!p)
	{
		std::string out("Invalid visual parameter: ");
		out.append(paramname);
		LuaError(out.c_str());
		return 0;
	}
	return (double)p->getDefaultWeight();
}

double getParamCurrentWeight(const LLUUID &avid,const char* paramname)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return 0;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	LLVisualParam *p=av->getVisualParam(paramname);
	if(!p)
	{
		std::string out("Invalid visual parameter: ");
		out.append(paramname);
		LuaError(out.c_str());
		return 0;
	}
	return (double)p->getCurrentWeight();
}

double getParamMax(const LLUUID &avid,const char* paramname)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return 0;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	LLVisualParam *p=av->getVisualParam(paramname);
	if(!p)
	{	gAgent.saveWearable(WT_SHAPE,TRUE);
		std::string out("Invalid visual parameter: ");
		out.append(paramname);
		LuaError(out.c_str());
		return 0;
	}
	return (double)p->getMaxWeight();
}

double getParamMin(const LLUUID &avid,const char* paramname)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return 0;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	LLVisualParam *p=av->getVisualParam(paramname);
	if(!p)
	{
		std::string out("Invalid visual parameter: ");
		out.append(paramname);
		LuaError(out.c_str());
		return 0;
	}
	return (double)p->getMinWeight();
}

//------------------------------------------------------------------------
// Set
//------------------------------------------------------------------------

void setParams_Event(LLUUID &avid, std::string &name, double &weight)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o)
	{
		LuaError("Can't find avatar.");
		return;
	}
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;

	LLVisualParam *p=av->getVisualParam(name.c_str());
	if(!p)
	{
		std::string out("Invalid visual parameter: ");
		out.append(name);
		LuaError(out.c_str());
		return;
	}
	p->setWeight((F32)weight,FALSE);
}

void setParamOnTarget(LLUUID target,std::string paramname,double weight)
{
	CB_Args3<LLUUID,std::string,double>(setParams_Event,target,paramname,weight); //add to client event queue
}

void setParamOnSelf(std::string paramname,double weight)
{
	LLVOAvatar *me=gAgent.getAvatarObject();
	if(!me)
		LuaError("No Agent Avatar");
	else
		CB_Args3<LLUUID,std::string,double>(setParams_Event,me->getID(),paramname,weight); //add to client event queue
	llinfos << "setParamOnSelf: Queued." << llendl;
}

void LuaWear_Event(const LLUUID& assetid)
{
	LLWearable *wear=LuaLoadWearable(assetid);
	if(!wear)
		LuaError("No Wearable found");
	else if((wear=gWearableList.createCopy(wear))==NULL)
		LuaError("Failed creation of new wearable");
	else
	{
		wear->saveNewAsset();
		wear->writeToAvatar(false);
	}
}

void LuaWear(const LLUUID& assetid)
{
	CB_Args1<const LLUUID>(&LuaWear_Event,assetid);
}

void LuaRemoveAllWearables_Event()
{
	LLAgent::userRemoveAllClothesStep2(TRUE,NULL);
}
void LuaRemoveAllWearables() // calls glGenTextures
{
	CB_Args0(LuaRemoveAllWearables_Event);
}

bool LuaSaveWearable(LLWearable *w)
{
	// TODO:  Add check for permissions here.

	if(!w)
		return false;
	//Buffer to hold the char[] version of the LLUUID
	char new_asset_id_string[UUID_STR_LENGTH];

	// Filename itself.
	char filename[LL_MAX_PATH];

	// Get ID and convert to char[]
	w->getID().toString(new_asset_id_string);
	
	// Turn ID into useful filename + path
	snprintf(filename, LL_MAX_PATH, "%s.wbl", gDirUtilp->getExpandedFilename(FL_PATH_LUA,"data",new_asset_id_string).c_str());

	// Open file handle for writing/creation
	LLFILE* fp = LLFile::fopen(filename, "wb");

	// Successful save flag
	BOOL successful_save = FALSE;

	// Did nothing fucking explode?
	if(fp && w->exportFile(fp))
	{
		//Then GREAT SUCCESS
		successful_save = TRUE;
	}

	// File handle still there?
	if(fp)
	{
		// Close it
		fclose(fp);
		fp = NULL;
	}

	// Did shit blow up?
	if(!successful_save)
	{
		// Make a char[] 2 times larger than max std::string  (?)
		char buffer[2*MAX_STRING];

		// Format error message
		snprintf(buffer,		/* Flawfinder: ignore */
				sizeof(buffer),
				"Unable to save '%s' to wearable file.",
				new_asset_id_string);

		// Stuff in warning log
		llwarns << buffer << llendl;
		
		std::stringstream s;

		// Red errors
		s << "Could not save wearable \"" << w->getName() << "\", possibly out of space.";
		LuaError(s.str().c_str());

		// Report back that shit did, indeed, blow up
		return false;
	}

	//Everything went fine, so report that
	return true;
}

LLWearable * LuaLoadWearable(const LLUUID& uuid)
{
	char filename[LL_MAX_PATH];		/* Flawfinder: ignore */
	snprintf(filename, LL_MAX_PATH, "%s.wbl", gDirUtilp->getExpandedFilename(FL_PATH_LUA,"data",uuid.asString()).c_str());		/* Flawfinder: ignore */
	LLFILE* fp = LLFile::fopen(filename, "r");		/* Flawfinder: ignore */
	
	if(!fp)
	{
		LuaError("Cannot open wearable asset file.  Make sure lua/data exists and that it is readable (CHMOD).");
		return NULL;
	}

	LLWearable *w=new LLWearable(uuid);
	if(!w->importFile(fp))
	{
		LuaError("Cannot open wearable asset file.  Make sure lua/data exists and that it is readable (CHMOD).");
		return NULL;
	}
	fclose(fp);

	return w;
}

// TE 0 = lol viewer tag.
void LuaSetTEImage(int index,const LLUUID& id)
{
	LLVOAvatar *a=gAgent.getAvatarObject();
	if(!a)
	{
		LuaError("No Agent Avatar");
		return;
	}
	a->setTEImage((U8)index,new LLViewerImage(id));
}
void LuaUpdateAppearance_Event()
{
	gAgent.saveAllWearables();

	// May fix llFloaterCustomize crash due to WT_SHAPE becoming dirty.
	LLVOAvatar* avatar = gAgent.getAvatarObject();
	if ( avatar )
	{
		avatar->invalidateAll();
		avatar->requestLayerSetUploads();
		gAgent.sendAgentSetAppearance();
		// Thanks to Zwagoth
		gAgent.saveWearable(WT_SHAPE,	TRUE);
		gAgent.saveWearable(WT_SKIN,	TRUE);
	}
}
void LuaUpdateAppearance()
{
	CB_Args0(LuaUpdateAppearance_Event);
}

bool HasPermissions(LLViewerInventoryItem* item)
{
	U32 perm_mask = 0x0;
	BOOL is_complete = FALSE;
	if(item)
	{
		perm_mask = item->getPermissions().getMaskOwner();
		is_complete = item->isComplete();
	}
	return((perm_mask & PERM_MODIFY) && is_complete);
}

void LuaStopAnimation(LLUUID avid, LLUUID movement)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	av->stopMotion(movement);
}

void LuaStartAnimation(LLUUID avid, LLUUID movement)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		return;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	av->startMotion(movement);
}

std::map<LLUUID,S32> LuaGetPlayingAnimations(LLUUID avid)
{
	LLViewerObject *o=gObjectList.findObject(avid);
	if(!o->isAvatar())
	{
		LuaError("Object is not an avatar.");
		std::map<LLUUID,S32> out;
		return out;
	}
	LLVOAvatar *av=(LLVOAvatar *)o;
	return av->mPlayingAnimations;
}