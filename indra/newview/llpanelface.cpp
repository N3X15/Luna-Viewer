/** 
 * @file llpanelface.cpp
 * @brief Panel in the tools floater for editing face textures, colors, etc.
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
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

// file include
#include "llpanelface.h"
 
// library includes
#include "llcalc.h"
#include "llerror.h"
#include "llfocusmgr.h"
#include "llrect.h"
#include "llstring.h"
#include "llfontgl.h"

// project includes
#include "llagent.h"
#include "llbutton.h"
#include "llcheckboxctrl.h"
#include "llcolorswatch.h"
#include "llcombobox.h"
#include "lldrawpoolbump.h"
#include "llface.h"
#include "lllineeditor.h"
#include "llresmgr.h"
#include "llselectmgr.h"
#include "llspinctrl.h"
#include "lltextbox.h"
#include "lltexturectrl.h"
#include "lltextureentry.h"
#include "lltooldraganddrop.h"
#include "llui.h"
#include "llviewercontrol.h"
#include "llviewermedia.h"
#include "llviewerobject.h"
#include "llviewerregion.h"
#include "llviewerstats.h"
#include "lluictrlfactory.h"
#include "llpluginclassmedia.h"

//
// Methods
//

BOOL	LLPanelFace::postBuild()
{
	LLRect	rect = this->getRect();
	LLTextureCtrl*	mTextureCtrl;
	LLColorSwatchCtrl*	mColorSwatch;

	LLComboBox*		mComboTexGen;

	LLCheckBoxCtrl	*mCheckFullbright;
	
	LLTextBox*		mLabelColorTransp;
	LLSpinCtrl*		mCtrlColorTransp;		// transparency = 1 - alpha

	LLSpinCtrl*     mCtrlGlow;

	setMouseOpaque(FALSE);
	mTextureCtrl = getChild<LLTextureCtrl>("texture control");
	if(mTextureCtrl)
	{
		mTextureCtrl->setDefaultImageAssetID(LLUUID( gSavedSettings.getString( "DefaultObjectTexture" )));
		mTextureCtrl->setCommitCallback( LLPanelFace::onCommitTexture );
		mTextureCtrl->setOnCancelCallback( LLPanelFace::onCancelTexture );
		mTextureCtrl->setOnSelectCallback( LLPanelFace::onSelectTexture );
		mTextureCtrl->setDragCallback(LLPanelFace::onDragTexture);
		mTextureCtrl->setCallbackUserData( this );
		mTextureCtrl->setFollowsTop();
		mTextureCtrl->setFollowsLeft();
		// Don't allow (no copy) or (no transfer) textures to be selected during immediate mode
		mTextureCtrl->setImmediateFilterPermMask(PERM_COPY | PERM_TRANSFER);
		// Allow any texture to be used during non-immediate mode.
		mTextureCtrl->setNonImmediateFilterPermMask(PERM_NONE);
		LLAggregatePermissions texture_perms;
		if (LLSelectMgr::getInstance()->selectGetAggregateTexturePermissions(texture_perms))
		{
			BOOL can_copy = 
				texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_EMPTY || 
				texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_ALL;
			BOOL can_transfer = 
				texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_EMPTY || 
				texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_ALL;
			mTextureCtrl->setCanApplyImmediately(can_copy && can_transfer);
		}
		else
		{
			mTextureCtrl->setCanApplyImmediately(FALSE);
		}
	}

	mColorSwatch = getChild<LLColorSwatchCtrl>("colorswatch");
	if(mColorSwatch)
	{
		mColorSwatch->setCommitCallback(LLPanelFace::onCommitColor);
		mColorSwatch->setOnCancelCallback(LLPanelFace::onCancelColor);
		mColorSwatch->setOnSelectCallback(LLPanelFace::onSelectColor);
		mColorSwatch->setCallbackUserData( this );
		mColorSwatch->setFollowsTop();
		mColorSwatch->setFollowsLeft();
		mColorSwatch->setCanApplyImmediately(TRUE);
	}

	mLabelColorTransp = getChild<LLTextBox>("color trans");
	if(mLabelColorTransp)
	{
		mLabelColorTransp->setFollowsTop();
		mLabelColorTransp->setFollowsLeft();
	}

	mCtrlColorTransp = getChild<LLSpinCtrl>("ColorTrans");
	if(mCtrlColorTransp)
	{
		mCtrlColorTransp->setCommitCallback(LLPanelFace::onCommitAlpha);
		mCtrlColorTransp->setCallbackUserData(this);
		mCtrlColorTransp->setPrecision(0);
		mCtrlColorTransp->setFollowsTop();
		mCtrlColorTransp->setFollowsLeft();
	}

	mCheckFullbright = getChild<LLCheckBoxCtrl>("checkbox fullbright");
	if (mCheckFullbright)
	{
		mCheckFullbright->setCommitCallback(LLPanelFace::onCommitFullbright);
		mCheckFullbright->setCallbackUserData( this );
	}

	mComboTexGen = getChild<LLComboBox>("combobox texgen");
	if(mComboTexGen)
	{
		mComboTexGen->setCommitCallback(LLPanelFace::onCommitTexGen);
		mComboTexGen->setFollows(FOLLOWS_LEFT | FOLLOWS_TOP);	
		mComboTexGen->setCallbackUserData( this );
	}

	mCtrlGlow = getChild<LLSpinCtrl>("glow");
	if(mCtrlGlow)
	{
		mCtrlGlow->setCommitCallback(LLPanelFace::onCommitGlow);
		mCtrlGlow->setCallbackUserData(this);
	}
	
	childSetCommitCallback("combobox shininess",&LLPanelFace::onCommitShiny,this);
	childSetCommitCallback("combobox bumpiness",&LLPanelFace::onCommitBump,this);
	childSetCommitCallback("checkbox planar align",&LLPanelFace::onCommitPlanarAlign, this);
	childSetCommitCallback("TexScaleU",&LLPanelFace::onCommitTextureInfo, this);
	childSetCommitCallback("checkbox flip s",&LLPanelFace::onCommitTextureInfo, this);
	childSetCommitCallback("TexScaleV",&LLPanelFace::onCommitTextureInfo, this);
	childSetCommitCallback("checkbox flip t",&LLPanelFace::onCommitTextureInfo, this);
	childSetCommitCallback("TexRot",&LLPanelFace::onCommitTextureInfo, this);
	childSetAction("button apply",&onClickApply,this);
	childSetCommitCallback("TexOffsetU",LLPanelFace::onCommitTextureInfo, this);
	childSetCommitCallback("TexOffsetV",LLPanelFace::onCommitTextureInfo, this);
	childSetAction("button align",onClickAutoFix,this);
	childSetAction("copytextures",onClickCopy,this);
	childSetAction("pastetextures",onClickPaste,this);

	clearCtrls();

	return TRUE;
}

LLPanelFace::LLPanelFace(const std::string& name)
:	LLPanel(name)
{
}


LLPanelFace::~LLPanelFace()
{
	// Children all cleaned up by default view destructor.
}


void LLPanelFace::sendTexture()
{
	LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("texture control");
	if(!mTextureCtrl) return;
	if( !mTextureCtrl->getTentative() )
	{
		// we grab the item id first, because we want to do a
		// permissions check in the selection manager. ARGH!
		LLUUID id = mTextureCtrl->getImageItemID();
		if(id.isNull())
		{
			id = mTextureCtrl->getImageAssetID();
		}
		LLSelectMgr::getInstance()->selectionSetImage(id);
	}
}

void LLPanelFace::sendBump()
{	
	LLComboBox*	mComboBumpiness = getChild<LLComboBox>("combobox bumpiness");
	if(!mComboBumpiness)return;
	U8 bump = (U8) mComboBumpiness->getCurrentIndex() & TEM_BUMP_MASK;
	LLSelectMgr::getInstance()->selectionSetBumpmap( bump );
}

void LLPanelFace::sendTexGen()
{
	LLComboBox*	mComboTexGen = getChild<LLComboBox>("combobox texgen");
	if(!mComboTexGen)return;
	U8 tex_gen = (U8) mComboTexGen->getCurrentIndex() << TEM_TEX_GEN_SHIFT;
	LLSelectMgr::getInstance()->selectionSetTexGen( tex_gen );
}

void LLPanelFace::sendShiny()
{
	LLComboBox*	mComboShininess = getChild<LLComboBox>("combobox shininess");
	if(!mComboShininess)return;
	U8 shiny = (U8) mComboShininess->getCurrentIndex() & TEM_SHINY_MASK;
	LLSelectMgr::getInstance()->selectionSetShiny( shiny );
}

void LLPanelFace::sendFullbright()
{
	LLCheckBoxCtrl*	mCheckFullbright = getChild<LLCheckBoxCtrl>("checkbox fullbright");
	if(!mCheckFullbright)return;
	U8 fullbright = mCheckFullbright->get() ? TEM_FULLBRIGHT_MASK : 0;
	LLSelectMgr::getInstance()->selectionSetFullbright( fullbright );
}

void LLPanelFace::sendColor()
{
	
	LLColorSwatchCtrl*	mColorSwatch = getChild<LLColorSwatchCtrl>("colorswatch");
	if(!mColorSwatch)return;
	LLColor4 color = mColorSwatch->get();

	LLSelectMgr::getInstance()->selectionSetColorOnly( color );
}

void LLPanelFace::sendAlpha()
{	
	LLSpinCtrl*	mCtrlColorTransp = getChild<LLSpinCtrl>("ColorTrans");
	if(!mCtrlColorTransp)return;
	F32 alpha = (100.f - mCtrlColorTransp->get()) / 100.f;

	LLSelectMgr::getInstance()->selectionSetAlphaOnly( alpha );
}


void LLPanelFace::sendGlow()
{
	LLSpinCtrl*	mCtrlGlow = getChild<LLSpinCtrl>("glow");
	F32 glow = mCtrlGlow->get();

	LLSelectMgr::getInstance()->selectionSetGlow( glow );
}

struct LLPanelFaceSetTEFunctor : public LLSelectedTEFunctor
{
	LLPanelFaceSetTEFunctor(LLPanelFace* panel) : mPanel(panel) {}
	virtual bool apply(LLViewerObject* object, S32 te)
	{
		BOOL valid;
		F32 value;
		LLSpinCtrl*	ctrlTexScaleS = mPanel->getChild<LLSpinCtrl>("TexScaleU");
		LLSpinCtrl*	ctrlTexScaleT = mPanel->getChild<LLSpinCtrl>("TexScaleV");
		LLSpinCtrl*	ctrlTexOffsetS = mPanel->getChild<LLSpinCtrl>("TexOffsetU");
		LLSpinCtrl*	ctrlTexOffsetT = mPanel->getChild<LLSpinCtrl>("TexOffsetV");
		LLSpinCtrl*	ctrlTexRotation = mPanel->getChild<LLSpinCtrl>("TexRot");
		LLCheckBoxCtrl*	checkFlipScaleS = mPanel->getChild<LLCheckBoxCtrl>("checkbox flip s");
		LLCheckBoxCtrl*	checkFlipScaleT = mPanel->getChild<LLCheckBoxCtrl>("checkbox flip t");
		LLComboBox*		comboTexGen = mPanel->getChild<LLComboBox>("combobox texgen");
		if (ctrlTexScaleS)
		{
			valid = !ctrlTexScaleS->getTentative() || !checkFlipScaleS->getTentative();
			if (valid)
			{
				value = ctrlTexScaleS->get();
				if( checkFlipScaleS->get() )
				{
					value = -value;
				}
				if (comboTexGen->getCurrentIndex() == 1)
				{
					value *= 0.5f;
				}
				object->setTEScaleS( te, value );
			}
		}

		if (ctrlTexScaleT)
		{
			valid = !ctrlTexScaleT->getTentative() || !checkFlipScaleT->getTentative();
			if (valid)
			{
				value = ctrlTexScaleT->get();
				if( checkFlipScaleT->get() )
				{
					value = -value;
				}
				if (comboTexGen->getCurrentIndex() == 1)
				{
					value *= 0.5f;
				}
				object->setTEScaleT( te, value );
			}
		}

		if (ctrlTexOffsetS)
		{
			valid = !ctrlTexOffsetS->getTentative();
			if (valid)
			{
				value = ctrlTexOffsetS->get();
				object->setTEOffsetS( te, value );
			}
		}

		if (ctrlTexOffsetT)
		{
			valid = !ctrlTexOffsetT->getTentative();
			if (valid)
			{
				value = ctrlTexOffsetT->get();
				object->setTEOffsetT( te, value );
			}
		}

		if (ctrlTexRotation)
		{
			valid = !ctrlTexRotation->getTentative();
			if (valid)
			{
				value = ctrlTexRotation->get() * DEG_TO_RAD;
				object->setTERotation( te, value );
			}
		}
		return true;
	}
private:
	LLPanelFace* mPanel;
};

// Functor that aligns a face to mCenterFace
struct LLPanelFaceSetAlignedTEFunctor : public LLSelectedTEFunctor
{
	LLPanelFaceSetAlignedTEFunctor(LLPanelFace* panel, LLFace* center_face) :
		mPanel(panel),
		mCenterFace(center_face) {}

	virtual bool apply(LLViewerObject* object, S32 te)
	{
		LLFace* facep = object->mDrawable->getFace(te);
		if (!facep)
		{
			return true;
		}

		bool set_aligned = true;
		if (facep == mCenterFace)
		{
			set_aligned = false;
		}
		if (set_aligned)
		{
			LLVector2 uv_offset, uv_scale;
			F32 uv_rot;
			set_aligned = facep->calcAlignedPlanarTE(mCenterFace, &uv_offset, &uv_scale, &uv_rot);
			if (set_aligned)
			{
				object->setTEOffset(te, uv_offset.mV[VX], uv_offset.mV[VY]);
				object->setTEScale(te, uv_scale.mV[VX], uv_scale.mV[VY]);
				object->setTERotation(te, uv_rot);
			}
		}
		if (!set_aligned)
		{
			LLPanelFaceSetTEFunctor setfunc(mPanel);
			setfunc.apply(object, te);
		}
		return true;
	}
private:
	LLPanelFace* mPanel;
	LLFace* mCenterFace;
};

// Functor that tests if a face is aligned to mCenterFace
struct LLPanelFaceGetIsAlignedTEFunctor : public LLSelectedTEFunctor
{
	LLPanelFaceGetIsAlignedTEFunctor(LLFace* center_face) :
		mCenterFace(center_face) {}

	virtual bool apply(LLViewerObject* object, S32 te)
	{
		LLFace* facep = object->mDrawable->getFace(te);
		if (!facep)
		{
			return false;
		}
		if (facep == mCenterFace)
		{
			return true;
		}
		
		LLVector2 aligned_st_offset, aligned_st_scale;
		F32 aligned_st_rot;
		if ( facep->calcAlignedPlanarTE(mCenterFace, &aligned_st_offset, &aligned_st_scale, &aligned_st_rot) )
		{
			const LLTextureEntry* tep = facep->getTextureEntry();
			LLVector2 st_offset, st_scale;
			tep->getOffset(&st_offset.mV[VX], &st_offset.mV[VY]);
			tep->getScale(&st_scale.mV[VX], &st_scale.mV[VY]);
			F32 st_rot = tep->getRotation();
			// needs a fuzzy comparison, because of fp errors
			if (is_approx_equal_fraction(st_offset.mV[VX], aligned_st_offset.mV[VX], 16) && 
				is_approx_equal_fraction(st_offset.mV[VY], aligned_st_offset.mV[VY], 16) && 
				is_approx_equal_fraction(st_scale.mV[VX], aligned_st_scale.mV[VX], 16) &&
				is_approx_equal_fraction(st_scale.mV[VY], aligned_st_scale.mV[VY], 16) &&
				is_approx_equal_fraction(st_rot, aligned_st_rot, 14))
			{
				return true;
			}
		}
		return false;
	}
private:
	LLFace* mCenterFace;
};

struct LLPanelFaceSendFunctor : public LLSelectedObjectFunctor
{
	virtual bool apply(LLViewerObject* object)
	{
		object->sendTEUpdate();
		return true;
	}
};

void LLPanelFace::sendTextureInfo()
{
	if ((bool)childGetValue("checkbox planar align").asBoolean())
	{
		struct f1 : public LLSelectedTEGetFunctor<LLFace *>
		{
			LLFace* get(LLViewerObject* object, S32 te)
			{
				return (object->mDrawable) ? object->mDrawable->getFace(te): NULL;
			}
		} get_last_face_func;
		LLFace* last_face;
		LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue(&get_last_face_func, last_face);

		LLPanelFaceSetAlignedTEFunctor setfunc(this, last_face);
		LLSelectMgr::getInstance()->getSelection()->applyToTEs(&setfunc);
	}
	else
	{
		LLPanelFaceSetTEFunctor setfunc(this);
		LLSelectMgr::getInstance()->getSelection()->applyToTEs(&setfunc);
	}

	LLPanelFaceSendFunctor sendfunc;
	LLSelectMgr::getInstance()->getSelection()->applyToObjects(&sendfunc);
}

void LLPanelFace::getState()
{
	LLViewerObject* objectp = LLSelectMgr::getInstance()->getSelection()->getFirstObject();
	LLCalc* calcp = LLCalc::getInstance();
	if( objectp
		&& objectp->getPCode() == LL_PCODE_VOLUME
		&& objectp->permModify())
	{
		BOOL editable = objectp->permModify();


		// only turn on auto-adjust button if there is a media renderer and the media is loaded
		childSetEnabled("textbox autofix",FALSE);
		//mLabelTexAutoFix->setEnabled ( FALSE );
		childSetEnabled("button align",FALSE);
		//mBtnAutoFix->setEnabled ( FALSE );
		
		//if ( LLMediaEngine::getInstance()->getMediaRenderer () )
		//	if ( LLMediaEngine::getInstance()->getMediaRenderer ()->isLoaded () )
		//	{	
		//		
		//		//mLabelTexAutoFix->setEnabled ( editable );
		//		
		//		//mBtnAutoFix->setEnabled ( editable );
		//	}
		S32 selected_count = LLSelectMgr::getInstance()->getSelection()->getObjectCount();
		BOOL single_volume = (LLSelectMgr::getInstance()->selectionAllPCode( LL_PCODE_VOLUME ))
						 && (selected_count == 1);
		
		childSetEnabled("copytextures", single_volume && editable);
		childSetEnabled("pastetextures", single_volume && editable);
		childSetEnabled("textbox params", single_volume && editable);
		childSetEnabled("button apply",editable);

		bool identical;
		LLTextureCtrl*	texture_ctrl = getChild<LLTextureCtrl>("texture control");
		
		// Texture
		{
			LLUUID id;
			struct f1 : public LLSelectedTEGetFunctor<LLUUID>
			{
				LLUUID get(LLViewerObject* object, S32 te)
				{
					LLViewerImage* image = object->getTEImage(te);
					return image ? image->getID() : LLUUID::null;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, id );

			if (identical)
			{
				// All selected have the same texture
				if(texture_ctrl)
				{
					texture_ctrl->setTentative( FALSE );
					texture_ctrl->setEnabled( editable );
					texture_ctrl->setImageAssetID( id );
				}
			}
			else
			{
				if(texture_ctrl)
				{
					if( id.isNull() )
					{
						// None selected
						texture_ctrl->setTentative( FALSE );
						texture_ctrl->setEnabled( FALSE );
						texture_ctrl->setImageAssetID( LLUUID::null );
					}
					else
					{
						// Tentative: multiple selected with different textures
						texture_ctrl->setTentative( TRUE );
						texture_ctrl->setEnabled( editable );
						texture_ctrl->setImageAssetID( id );
					}
				}
			}

			if(LLViewerMedia::textureHasMedia(id))
			{
				childSetEnabled("textbox autofix",editable);
				childSetEnabled("button align",editable);
			}

		}

		
		LLAggregatePermissions texture_perms;
		if(texture_ctrl)
		{
// 			texture_ctrl->setValid( editable );
		
			if (LLSelectMgr::getInstance()->selectGetAggregateTexturePermissions(texture_perms))
			{
				BOOL can_copy = 
					texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_EMPTY || 
					texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_ALL;
				BOOL can_transfer = 
					texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_EMPTY || 
					texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_ALL;
				texture_ctrl->setCanApplyImmediately(can_copy && can_transfer);
			}
			else
			{
				texture_ctrl->setCanApplyImmediately(FALSE);
			}
		}

		// planar align
		bool align_planar = false;
		bool identical_planar_aligned = false;
		{
			LLCheckBoxCtrl*	cb_planar_align = getChild<LLCheckBoxCtrl>("checkbox planar align");
			align_planar = (cb_planar_align && cb_planar_align->get());
			struct f1 : public LLSelectedTEGetFunctor<bool>
			{
				bool get(LLViewerObject* object, S32 face)
				{
					return (object->getTE(face)->getTexGen() == LLTextureEntry::TEX_GEN_PLANAR);
				}
			} func;
			
			bool is_planar;
			bool texgens_identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, is_planar );
			bool enabled = (editable && texgens_identical && is_planar);
			childSetValue("checkbox planar align", align_planar && enabled);
			childSetEnabled("checkbox planar align", enabled);
		
			if (align_planar && enabled)
			{
				struct f2 : public LLSelectedTEGetFunctor<LLFace *>
				{
					LLFace* get(LLViewerObject* object, S32 te)
					{
						return (object->mDrawable) ? object->mDrawable->getFace(te): NULL;
					}
				} get_te_face_func;
				LLFace* last_face;
				LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue(&get_te_face_func, last_face);
				LLPanelFaceGetIsAlignedTEFunctor get_is_aligend_func(last_face);
				// this will determine if the texture param controls are tentative:
				identical_planar_aligned = LLSelectMgr::getInstance()->getSelection()->applyToTEs(&get_is_aligend_func);
			}
		}
		
		// Texture scale
		{
			childSetEnabled("tex scale",editable);
			//mLabelTexScale->setEnabled( editable );
			F32 scale_s = 1.f;
			struct f2 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->mScaleS;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, scale_s );
			identical = align_planar ? identical_planar_aligned : identical;
			childSetValue("TexScaleU",editable ? llabs(scale_s) : 0);
			childSetTentative("TexScaleU",LLSD((BOOL)(!identical)));
			childSetEnabled("TexScaleU",editable);
			childSetValue("checkbox flip s",LLSD((BOOL)(scale_s < 0 ? TRUE : FALSE )));
			childSetTentative("checkbox flip s",LLSD((BOOL)((!identical) ? TRUE : FALSE )));
			childSetEnabled("checkbox flip s",editable);
		}

		{
			F32 scale_t = 1.f;
			struct f3 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->mScaleT;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, scale_t );
			identical = align_planar ? identical_planar_aligned : identical;

			childSetValue("TexScaleV",llabs(editable ? llabs(scale_t) : 0));
			childSetTentative("TexScaleV",LLSD((BOOL)(!identical)));
			childSetEnabled("TexScaleV",editable);
			childSetValue("checkbox flip t",LLSD((BOOL)(scale_t< 0 ? TRUE : FALSE )));
			childSetTentative("checkbox flip t",LLSD((BOOL)((!identical) ? TRUE : FALSE )));
			childSetEnabled("checkbox flip t",editable);
		}

		// Texture offset
		{
			childSetEnabled("tex offset",editable);
			F32 offset_s = 0.f;
			struct f4 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->mOffsetS;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, offset_s );
			identical = align_planar ? identical_planar_aligned : identical;
			childSetValue("TexOffsetU", editable ? offset_s : 0);
			childSetTentative("TexOffsetU",!identical);
			childSetEnabled("TexOffsetU",editable);
		}

		{
			F32 offset_t = 0.f;
			struct f5 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->mOffsetT;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, offset_t );
			identical = align_planar ? identical_planar_aligned : identical;
			childSetValue("TexOffsetV", editable ? offset_t : 0);
			childSetTentative("TexOffsetV",!identical);
			childSetEnabled("TexOffsetV",editable);
		}

		// Texture rotation
		{
			childSetEnabled("tex rotate",editable);
			F32 rotation = 0.f;
			struct f6 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->mRotation;
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, rotation );
			identical = align_planar ? identical_planar_aligned : identical;
			childSetValue("TexRot", editable ? rotation * RAD_TO_DEG : 0);
			childSetTentative("TexRot",!identical);
			childSetEnabled("TexRot",editable);
		}

		// Color swatch
		LLColorSwatchCtrl*	mColorSwatch = getChild<LLColorSwatchCtrl>("colorswatch");
		LLColor4 color = LLColor4::white;
		if(mColorSwatch)
		{
			struct f7 : public LLSelectedTEGetFunctor<LLColor4>
			{
				LLColor4 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->getColor();
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, color );
			
			mColorSwatch->setOriginal(color);
			mColorSwatch->set(color, TRUE);

			mColorSwatch->setValid(editable);
			mColorSwatch->setEnabled( editable );
			mColorSwatch->setCanApplyImmediately( editable );
		}
		// Color transparency
		{
			childSetEnabled("color trans",editable);
		}

		F32 transparency = (1.f - color.mV[VALPHA]) * 100.f;
		{
			childSetValue("ColorTrans", editable ? transparency : 0);
			childSetEnabled("ColorTrans",editable);
		}

		{
			F32 glow = 0.f;
			struct f8 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return object->getTE(face)->getGlow();
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, glow );

			childSetValue("glow",glow);
			childSetEnabled("glow",editable);
			childSetTentative("glow",!identical);
			childSetEnabled("glow label",editable);

		}

		// Bump
		{
			F32 shinyf = 0.f;
			struct f9 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return (F32)(object->getTE(face)->getShiny());
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, shinyf );
			LLCtrlSelectionInterface* combobox_shininess =
			      childGetSelectionInterface("combobox shininess");
			if (combobox_shininess)
			{
				combobox_shininess->selectNthItem((S32)shinyf);
			}
			else
			{
				llwarns << "failed childGetSelectionInterface for 'combobox shininess'" << llendl;
			}
			childSetEnabled("combobox shininess",editable);
			childSetTentative("combobox shininess",!identical);
			childSetEnabled("label shininess",editable);
		}

		{
			F32 bumpf = 0.f;
			struct f10 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return (F32)(object->getTE(face)->getBumpmap());
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, bumpf );
			LLCtrlSelectionInterface* combobox_bumpiness =
			      childGetSelectionInterface("combobox bumpiness");
			if (combobox_bumpiness)
			{
				combobox_bumpiness->selectNthItem((S32)bumpf);
			}
			else
			{
				llwarns << "failed childGetSelectionInterface for 'combobox bumpiness'" << llendl;
			}
			childSetEnabled("combobox bumpiness",editable);
			childSetTentative("combobox bumpiness",!identical);
			childSetEnabled("label bumpiness",editable);
		}

		{
			F32 genf = 0.f;
			struct f11 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return (F32)(object->getTE(face)->getTexGen());
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, genf );
			S32 selected_texgen = ((S32) genf) >> TEM_TEX_GEN_SHIFT;
			LLCtrlSelectionInterface* combobox_texgen =
			      childGetSelectionInterface("combobox texgen");
			if (combobox_texgen)
			{
				combobox_texgen->selectNthItem(selected_texgen);
			}
			else
			{
				llwarns << "failed childGetSelectionInterface for 'combobox texgen'" << llendl;
			}
			childSetEnabled("combobox texgen",editable);
			childSetTentative("combobox texgen",!identical);
			childSetEnabled("tex gen",editable);

			if (selected_texgen == 1)
			{
				childSetText("tex scale",getString("string repeats per meter"));
				childSetValue("TexScaleU", 2.0f * childGetValue("TexScaleU").asReal() );
				childSetValue("TexScaleV", 2.0f * childGetValue("TexScaleV").asReal() );
			}
			else
			{
				childSetText("tex scale",getString("string repeats per face"));
			}

		}

		{
			F32 fullbrightf = 0.f;
			struct f12 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					return (F32)(object->getTE(face)->getFullbright());
				}
			} func;
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, fullbrightf );

			childSetValue("checkbox fullbright",(S32)fullbrightf);
			childSetEnabled("checkbox fullbright",editable);
			childSetTentative("checkbox fullbright",!identical);
		}
		
		// Repeats per meter label
		{
			childSetEnabled("rpt",editable);
		}

		// Repeats per meter
		{
			F32 repeats = 1.f;
			struct f13 : public LLSelectedTEGetFunctor<F32>
			{
				F32 get(LLViewerObject* object, S32 face)
				{
					U32 s_axis = VX;
					U32 t_axis = VY;
					// BUG: Only repeats along S axis
					// BUG: Only works for boxes.
					LLPrimitive::getTESTAxes(face, &s_axis, &t_axis);
					return object->getTE(face)->mScaleS / object->getScale().mV[s_axis];
				}
			} func;			
			identical = LLSelectMgr::getInstance()->getSelection()->getSelectedTEValue( &func, repeats );
			
			childSetValue("rptctrl", editable ? repeats : 0);
			childSetTentative("rptctrl",!identical);
			LLComboBox*	mComboTexGen = getChild<LLComboBox>("combobox texgen");
			if (mComboTexGen)
			{
				BOOL enabled = editable && (!mComboTexGen || mComboTexGen->getCurrentIndex() != 1);
				childSetEnabled("rptctrl",enabled);
				childSetEnabled("button apply",enabled);
			}
		}

		// Set variable values for numeric expressions
		calcp->setVar(LLCalc::TEX_U_SCALE, childGetValue("TexScaleU").asReal());
		calcp->setVar(LLCalc::TEX_V_SCALE, childGetValue("TexScaleV").asReal());
		calcp->setVar(LLCalc::TEX_U_OFFSET, childGetValue("TexOffsetU").asReal());
		calcp->setVar(LLCalc::TEX_V_OFFSET, childGetValue("TexOffsetV").asReal());
		calcp->setVar(LLCalc::TEX_ROTATION, childGetValue("TexRot").asReal());
		calcp->setVar(LLCalc::TEX_TRANSPARENCY, childGetValue("ColorTrans").asReal());
		calcp->setVar(LLCalc::TEX_GLOW, childGetValue("glow").asReal());
	}
	else
	{
		// Disable all UICtrls
		clearCtrls();

		// Disable non-UICtrls
		LLTextureCtrl*	texture_ctrl = getChild<LLTextureCtrl>("texture control"); 
		if(texture_ctrl)
		{
			texture_ctrl->setImageAssetID( LLUUID::null );
			texture_ctrl->setFallbackImageName( "locked_image.j2c" );
			texture_ctrl->setEnabled( FALSE );  // this is a LLUICtrl, but we don't want it to have keyboard focus so we add it as a child, not a ctrl.
// 			texture_ctrl->setValid(FALSE);
		}
		LLColorSwatchCtrl* mColorSwatch = getChild<LLColorSwatchCtrl>("colorswatch");
		if(mColorSwatch)
		{
			mColorSwatch->setEnabled( FALSE );			
			mColorSwatch->setFallbackImageName("locked_image.j2c" );
			mColorSwatch->setValid(FALSE);
		}
		childSetEnabled("color trans",FALSE);
		childSetEnabled("rpt",FALSE);
		childSetEnabled("tex scale",FALSE);
		childSetEnabled("tex offset",FALSE);
		childSetEnabled("tex rotate",FALSE);
		childSetEnabled("tex gen",FALSE);
		childSetEnabled("label shininess",FALSE);
		childSetEnabled("label bumpiness",FALSE);

		childSetEnabled("textbox autofix",FALSE);

		childSetEnabled("button align",FALSE);
		childSetEnabled("button apply",FALSE);


		// Set variable values for numeric expressions
		calcp->clearVar(LLCalc::TEX_U_SCALE);
		calcp->clearVar(LLCalc::TEX_V_SCALE);
		calcp->clearVar(LLCalc::TEX_U_OFFSET);
		calcp->clearVar(LLCalc::TEX_V_OFFSET);
		calcp->clearVar(LLCalc::TEX_ROTATION);
		calcp->clearVar(LLCalc::TEX_TRANSPARENCY);
		calcp->clearVar(LLCalc::TEX_GLOW);		
	}
}


void LLPanelFace::refresh()
{
	getState();
}

//
// Static functions
//

// static
F32 LLPanelFace::valueGlow(LLViewerObject* object, S32 face)
{
	return (F32)(object->getTE(face)->getGlow());
}


// static
void LLPanelFace::onCommitColor(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendColor();
}

// static
void LLPanelFace::onCommitAlpha(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendAlpha();
}

// static
void LLPanelFace::onCancelColor(LLUICtrl* ctrl, void* userdata)
{
	LLSelectMgr::getInstance()->selectionRevertColors();
}

// static
void LLPanelFace::onSelectColor(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	LLSelectMgr::getInstance()->saveSelectedObjectColors();
	self->sendColor();
}

// static
void LLPanelFace::onCommitBump(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendBump();
}

// static
void LLPanelFace::onCommitTexGen(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendTexGen();
}

// static
void LLPanelFace::onCommitShiny(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendShiny();
}

// static
void LLPanelFace::onCommitFullbright(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendFullbright();
}

// static
void LLPanelFace::onCommitGlow(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendGlow();
}

// static
BOOL LLPanelFace::onDragTexture(LLUICtrl*, LLInventoryItem* item, void*)
{
	BOOL accept = TRUE;
	for (LLObjectSelection::root_iterator iter = LLSelectMgr::getInstance()->getSelection()->root_begin();
		 iter != LLSelectMgr::getInstance()->getSelection()->root_end(); iter++)
	{
		LLSelectNode* node = *iter;
		LLViewerObject* obj = node->getObject();
		if(!LLToolDragAndDrop::isInventoryDropAcceptable(obj, item))
		{
			accept = FALSE;
			break;
		}
	}
	return accept;
}

// static
void LLPanelFace::onCommitTexture( LLUICtrl* ctrl, void* userdata )
{
	LLPanelFace* self = (LLPanelFace*) userdata;

	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_EDIT_TEXTURE_COUNT );
	
	self->sendTexture();
}

// static
void LLPanelFace::onCancelTexture(LLUICtrl* ctrl, void* userdata)
{
	LLSelectMgr::getInstance()->selectionRevertTextures();
}

// static
void LLPanelFace::onSelectTexture(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	LLSelectMgr::getInstance()->saveSelectedObjectTextures();
	self->sendTexture();
}


// static
void LLPanelFace::onCommitTextureInfo( LLUICtrl* ctrl, void* userdata )
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->sendTextureInfo();
}

// Commit the number of repeats per meter
// static
void LLPanelFace::onClickApply(void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	
	gFocusMgr.setKeyboardFocus( NULL );

	//F32 repeats_per_meter = self->mCtrlRepeatsPerMeter->get();
	F32 repeats_per_meter = (F32)self->childGetValue( "rptctrl" ).asReal();//self->mCtrlRepeatsPerMeter->get();
	LLSelectMgr::getInstance()->selectionTexScaleAutofit( repeats_per_meter );
}

// commit the fit media texture to prim button

struct LLPanelFaceSetMediaFunctor : public LLSelectedTEFunctor
{
	virtual bool apply(LLViewerObject* object, S32 te)
	{
		// TODO: the media impl pointer should actually be stored by the texture
		viewer_media_t pMediaImpl = LLViewerMedia::getMediaImplFromTextureID(object->getTE ( te )->getID());
		// only do this if it's a media texture
		if ( pMediaImpl.notNull())
		{
			LLPluginClassMedia *media = pMediaImpl->getMediaPlugin();
			if(media)
			{
				S32 media_width = media->getWidth();
				S32 media_height = media->getHeight();
				S32 texture_width = media->getTextureWidth();
				S32 texture_height = media->getTextureHeight();
				F32 scale_s = (F32)media_width / (F32)texture_width;
				F32 scale_t = (F32)media_height / (F32)texture_height;

				// set scale and adjust offset
				object->setTEScaleS( te, scale_s );
				object->setTEScaleT( te, scale_t );	// don't need to flip Y anymore since QT does this for us now.
				object->setTEOffsetS( te, -( 1.0f - scale_s ) / 2.0f );
				object->setTEOffsetT( te, -( 1.0f - scale_t ) / 2.0f );
			}
		}
		return true;
	};
};

void LLPanelFace::onClickAutoFix(void* userdata)
{
	LLPanelFaceSetMediaFunctor setfunc;
	LLSelectMgr::getInstance()->getSelection()->applyToTEs(&setfunc);

	LLPanelFaceSendFunctor sendfunc;
	LLSelectMgr::getInstance()->getSelection()->applyToObjects(&sendfunc);
}
// static
void LLPanelFace::onCommitPlanarAlign(LLUICtrl* ctrl, void* userdata)
{
	LLPanelFace* self = (LLPanelFace*) userdata;
	self->getState();
	self->sendTextureInfo();
}

static LLSD textures;

void LLPanelFace::onClickCopy(void* userdata)
{
	LLViewerObject* objectp = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
	if(!objectp)
	{
		objectp = LLSelectMgr::getInstance()->getSelection()->getFirstObject();
		if (!objectp)
		{
			return;
		}
	}
	S32 te_count = objectp->getNumFaces();
	textures.clear();
	for (S32 i = 0; i < te_count; i++)
	{
		llinfos << "Copying params on face " << i << "." << llendl;
		textures.append(objectp->getTE(i)->asLLSD());
	}
}

void LLPanelFace::onClickPaste(void* userdata)
{
	LLViewerObject* objectp = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
	if(!objectp)
	{
		objectp = LLSelectMgr::getInstance()->getSelection()->getFirstObject();
		if (!objectp)
		{
			return;
		}
	}
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectImage);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, objectp->getLocalID());
	msg->addStringFast(_PREHASH_MediaURL, NULL);
	
	LLPrimitive obj;
	obj.setNumTEs(U8(textures.size()));
	
	for (int i = 0; i < textures.size(); i++)
	{
		llinfos << "Pasting params on face " << i << "." << llendl;
		LLTextureEntry tex;
		tex.fromLLSD(textures[i]);
		obj.setTE(U8(i), tex);
	}
	
	obj.packTEMessage(gMessageSystem);
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}
