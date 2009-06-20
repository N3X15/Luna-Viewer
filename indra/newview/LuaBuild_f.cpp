#include "llviewerprecompiledheaders.h"
#include "LuaBuild_f.h"

#include "llprimitive.h"
#include "llvolume.h" // PCodes.
#include "llworld.h"
#include "llviewerregion.h"
#include "llworldmap.h"
#include "llagent.h"
#include "object_flags.h"
#include "llvolumemessage.h"

extern LLAgent gAgent;

LuaPrimBuilder::LuaPrimBuilder(void)
: mPrim()
{
	mPrim.init_primitive(PCODE_CUBE);
	mPhysx=false;
}
LuaPrimBuilder::~LuaPrimBuilder(void)
{}

void LuaPrimBuilder::SetSize(const double x, const double y, const double z)
{
	mPrim.setScale(x,y,z);
}
	
void LuaPrimBuilder::SetLocation(const double x, const double y, const double z)
{
	mPrim.setPosition(x,y,z);
}

void LuaPrimBuilder::SetTexture(const char* uuid,const U8 FaceID)
{
	LLUUID what(uuid);
	LLTextureEntry te(what);
	mPrim.setTE(FaceID,te);
}

bool LuaPrimBuilder::Commit()
{
	LLVector3 ray_start_region;
	LLVector3 ray_end_region;
	LLViewerRegion* regionp = gAgent.getRegion();
	
	ray_start_region	= regionp->getPosRegionFromAgent(LLVector3::zero);
	ray_end_region		= ray_start_region;

	LLQuaternion	rotation = mPrim.getRotation();
	LLVector3		scale = mPrim.getScale();
	U8				material = mPrim.getMaterial();
	LLVolumeParams	volume_params;

	gMessageSystem->newMessageFast(_PREHASH_ObjectAdd);
	gMessageSystem->nextBlockFast(_PREHASH_AgentData);
	gMessageSystem->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	gMessageSystem->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID()); 
	gMessageSystem->addUUIDFast(_PREHASH_GroupID, gAgent.getGroupID());
	gMessageSystem->nextBlockFast(_PREHASH_ObjectData);
	gMessageSystem->addU8Fast(_PREHASH_Material,	material);

	volume_params=VolumeParams;
	U32 flags = 0;		// not selected

	if (mPhysx)
	{
		flags |= FLAGS_USE_PHYSICS;
	}
	
	flags |= FLAGS_CREATE_SELECTED;

	gMessageSystem->addU32Fast(_PREHASH_AddFlags, flags );

	LLPCode volume_pcode;	// ...PCODE_VOLUME, or the original on error
	switch (mPrim.getPCode())
	{
		case LL_PCODE_SPHERE:
			rotation.setQuat(90.f * DEG_TO_RAD, LLVector3::y_axis);

			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE_HALF, LL_PCODE_PATH_CIRCLE );
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_TORUS:
			rotation.setQuat(90.f * DEG_TO_RAD, LLVector3::y_axis);

			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_CIRCLE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LLViewerObject::LL_VO_SQUARE_TORUS:
			rotation.setQuat(90.f * DEG_TO_RAD, LLVector3::y_axis);

			VolumeParams.setType( LL_PCODE_PROFILE_SQUARE, LL_PCODE_PATH_CIRCLE );

			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LLViewerObject::LL_VO_TRIANGLE_TORUS:
			rotation.setQuat(90.f * DEG_TO_RAD, LLVector3::y_axis);

			VolumeParams.setType( LL_PCODE_PROFILE_EQUALTRI, LL_PCODE_PATH_CIRCLE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_SPHERE_HEMI:
			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE_HALF, LL_PCODE_PATH_CIRCLE );

			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);

			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_CUBE:
			VolumeParams.setType( LL_PCODE_PROFILE_SQUARE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_PRISM:
			VolumeParams.setType( LL_PCODE_PROFILE_SQUARE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_PYRAMID:
			VolumeParams.setType( LL_PCODE_PROFILE_SQUARE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);

			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_TETRAHEDRON:
			VolumeParams.setType( LL_PCODE_PROFILE_EQUALTRI, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);

			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_CYLINDER:
			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_CYLINDER_HEMI:
			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_CONE:
			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_LINE );
			
			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		case LL_PCODE_CONE_HEMI:
			VolumeParams.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_LINE );

			LLVolumeMessage::packVolumeParams(&VolumeParams, gMessageSystem);
			volume_pcode = LL_PCODE_VOLUME;
			break;

		default:
			LLVolumeMessage::packVolumeParams(0, gMessageSystem);
			volume_pcode = mPrim.getPCode();
			break;
	}
	gMessageSystem->addU8Fast(_PREHASH_PCode, volume_pcode);

	gMessageSystem->addVector3Fast(_PREHASH_Scale,				mPrim.getScale() );
	gMessageSystem->addQuatFast(_PREHASH_Rotation,				rotation );
	gMessageSystem->addVector3Fast(_PREHASH_RayStart,			ray_start_region );
	gMessageSystem->addVector3Fast(_PREHASH_RayEnd,				ray_end_region );
	gMessageSystem->addU8Fast(_PREHASH_BypassRaycast,			(U8)TRUE );
	gMessageSystem->addU8Fast(_PREHASH_RayEndIsIntersection,	(U8)FALSE );
	gMessageSystem->addU8Fast(_PREHASH_State,					(U8)0);

	// Limit raycast to a single object.  
	// Speeds up server raycast + avoid problems with server ray hitting objects
	// that were clipped by the near plane or culled on the viewer.
	LLUUID ray_target_id;
	ray_target_id.setNull();
	gMessageSystem->addUUIDFast(_PREHASH_RayTargetID,			ray_target_id );
	
	// Pack in name value pairs
	gMessageSystem->sendReliable(regionp->getHost());

	// Spawns a message, so must be after above send
//	if (create_selected)
//	{
//		LLSelectMgr::getInstance()->deselectAll();
//		gViewerWindow->getWindow()->incBusyCount();
//	}

	// VEFFECT: AddObject
	/*
	LLHUDEffectSpiral *effectp = (LLHUDEffectSpiral *)LLHUDManager::getInstance()->createViewerEffect(LLHUDObject::LL_HUD_EFFECT_BEAM, TRUE);
	effectp->setSourceObject((LLViewerObject*)gAgent.getAvatarObject());
	effectp->setPositionGlobal(regionp->getPosGlobalFromRegion(ray_end_region));
	effectp->setDuration(LL_HUD_DUR_SHORT);
	effectp->setColor(LLColor4U(gAgent.getEffectColor()));*/

//	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CREATE_COUNT);
	return true;
}
