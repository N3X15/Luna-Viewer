/* Copyright (c) 2009
 *
 * Modular Systems Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems Ltd nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS �AS IS�
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "llviewerprecompiledheaders.h"

#include "lggBeamMapFloater.h"

#include "llagentdata.h"
#include "llcommandhandler.h"
#include "llfloater.h"
#include "llsdutil.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llagent.h"
#include "llfilepicker.h"
#include "llpanel.h"
#include "lliconctrl.h"
#include "llbutton.h"
#include "llcolorswatch.h"
#include "lggBeamMaps.h"

#include "llsdserialize.h"
class lggPoint;
class lggBeamMapFloater;

////////////////////////////////////////////////////////////////////////////
// lggBeamMapFloater
class lggBeamMapFloater : public LLFloater, public LLFloaterSingleton<lggBeamMapFloater>
{
public:
	lggBeamMapFloater(const LLSD& seed);
	virtual ~lggBeamMapFloater();
	

	BOOL postBuild(void);
	BOOL handleMouseDown(S32 x,S32 y,MASK mask);
	void update();
	BOOL handleRightMouseDown(S32 x,S32 y,MASK mask);

	void draw();
	void clearPoints();

	LLSD getMyDataSerialized();
	
	std::vector<lggPoint> dots;

	// UI Handlers
	static void onClickSave(void* data);
	static void onClickClear(void* data);
	static void onClickLoad(void* data);

	
private:
	static void onBackgroundChange(LLUICtrl* ctrl, void* userdata);
};
class lggPoint
{
	public:
		S32 x;
		S32 y;
		LLColor4 c;
};

void lggBeamMapFloater::clearPoints()
{
	dots.clear();
	
}
void lggBeamMapFloater::draw()
{
	//getChild<LLPanel>("beamshape_draw")->setBackgroundColor(getChild<LLColorSwatchCtrl>("back_color_swatch")->get());
	LLFloater::draw();
	LLRect rec  = getChild<LLPanel>("beamshape_draw")->getRect();
	
	gGL.pushMatrix();
	gGL.color4fv(LLColor4::white.mV);
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),2.0f,(S32)30,false);
	gGL.color4fv(LLColor4::black.mV);
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),30.0f,(S32)30,false);	
	gGL.color4fv(LLColor4::white.mV);
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),60.0f,(S32)30,false);	
	gGL.color4fv(LLColor4::black.mV);
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),90.0f,(S32)30,false);
	gGL.color4fv(LLColor4::white.mV);
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),120.0f,(S32)30,false);

	for(int i = 0; i < (int)dots.size();i++)
	{	
		gGL.color4fv(LLColor4::white.mV);
		gl_circle_2d(dots[i].x,dots[i].y,9.0f,(S32)30,true);

		gGL.color4fv(LLColor4::black.mV);
		gl_circle_2d(dots[i].x,dots[i].y,8.0f,(S32)30,true);
		
		gGL.color4fv(dots[i].c.mV);
		gl_circle_2d(dots[i].x,dots[i].y,7.0f,(S32)30,true);
		
		
	}
	gGL.popMatrix();
	
}

lggBeamMapFloater::~lggBeamMapFloater()
{
	//if(mCallback) mCallback->detach();
}

lggBeamMapFloater::lggBeamMapFloater(const LLSD& seed)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_beamshape.xml");
	
	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}

}

BOOL lggBeamMapFloater::postBuild(void)
{
	setCanMinimize(false);
	childSetAction("beamshape_save",onClickSave,this);
	childSetAction("beamshape_clear",onClickClear,this);
	childSetAction("beamshape_load",onClickLoad,this);
	getChild<LLColorSwatchCtrl>("back_color_swatch")->setCommitCallback(onBackgroundChange);
	
	
	return true;
}
BOOL lggBeamMapFloater::handleMouseDown(S32 x,S32 y,MASK mask)
{
	if(y>39 && x>16 && x<394 && y<317)
	{
		lggPoint a;
		a.x=x;
		a.y=y;
		a.c= 	getChild<LLColorSwatchCtrl>("beam_color_swatch")->get();
		dots.push_back(a);
	
		llinfos << "we got clicked at (" << x << ", " << y << " and color was " << a.c << llendl;
	}
	
	return LLFloater::handleMouseDown(x,y,mask);
}
BOOL lggBeamMapFloater::handleRightMouseDown(S32 x,S32 y,MASK mask)
{
	std::vector<lggPoint> newDots;
	for(int i = 0; i < (int)dots.size();i++)
	{
		if(dist_vec(LLVector2(x,y),LLVector2(dots[i].x,dots[i].y)) < 7)
		{
			
		}else
		{
			newDots.push_back(dots[i]);
		}
	
	}
	dots = newDots;
		
	return LLFloater::handleMouseDown(x,y,mask);
}
void lggBeamMapFloater::onBackgroundChange(LLUICtrl* ctrl, void* userdata)
{
	lggBeamMapFloater* self = (lggBeamMapFloater*)userdata;
	
	LLColorSwatchCtrl* cctrl = (LLColorSwatchCtrl*)ctrl;
	
	if(cctrl)
	{
		self->getChild<LLPanel>("beamshape_draw")->setBackgroundColor(cctrl->get());
	}
	
}
void lggBeamMapFloater::update()
{
	
}
LLSD lggBeamMapFloater::getMyDataSerialized()
{
	LLSD out;
	LLRect r  = getChild<LLPanel>("beamshape_draw")->getRect();
	for(int i =0; i<(int)dots.size();i++)
	{
		LLSD point;
		lggPoint t = dots[i];
		LLVector3 vec = LLVector3((F32)0.0,(F32)t.x,(F32)t.y);
		vec -= LLVector3((F32)0.0,(F32)r.getCenterX(),r.getCenterY());
		
		point["offset"]= vec.getValue();
		point["color"] = t.c.getValue();

		out[i]=point;
	}
	return out;
}
void lggBeamMapFloater::onClickSave(void* data)
{
	
	lggBeamMapFloater* self = (lggBeamMapFloater*)data;
	LLRect r  = self->getChild<LLPanel>("beamshape_draw")->getRect();
	
	LLFilePicker& picker = LLFilePicker::instance();
	
	std::string filename=	gDirUtilp->getAppRODataDir() 
						+gDirUtilp->getDirDelimiter()
						+"beams"
						+gDirUtilp->getDirDelimiter()
						+"myNewBeam.xml";
	if(!picker.getSaveFile( LLFilePicker::FFSAVE_BEAM, filename ) )
	{return;
	}	
	
	filename = gDirUtilp->getAppRODataDir() 
						+gDirUtilp->getDirDelimiter()
						+"beams"
						+gDirUtilp->getDirDelimiter()
						+gDirUtilp->getBaseFileName(picker.getFirstFile());

	
	LLSD main;
	main["scale"] = 8.0f/(r.getWidth());
	main["data"]=self->getMyDataSerialized();
  
	llofstream export_file;
	export_file.open(filename);
	LLSDSerialize::toPrettyXML(main, export_file);
	export_file.close();
	gSavedSettings.setString("EmeraldBeamShape",gDirUtilp->getBaseFileName(filename,true));
	
}

void lggBeamMapFloater::onClickClear(void* data)
{
	lggBeamMapFloater* self = (lggBeamMapFloater*)data;
	
	
	self->clearPoints();
	
}
void lggBeamMapFloater::onClickLoad(void* data)
{
	
	lggBeamMapFloater* self = (lggBeamMapFloater*)data;
	
	LLFilePicker& picker = LLFilePicker::instance();
	if(!picker.getOpenFile( LLFilePicker::FFLOAD_XML ) )
	{
		return;
	}
	self->dots.clear();
	LLSD mydata;
	llifstream importer(picker.getFirstFile());
	LLSDSerialize::fromXMLDocument(mydata, importer);
	LLSD myPicture = mydata["data"];	
	F32 scale = (F32)mydata["scale"].asReal();

			
	for(int i = 0; i < myPicture.size(); i++)
	{
		LLRect rec  = self->getChild<LLPanel>("beamshape_draw")->getRect();
	
		
		LLSD beamData = myPicture[i];
		lggPoint p;
		LLVector3 vec =  beamData["offset"];
		vec *= (scale)/(8.0f/(rec.getWidth()));
		LLColor4 color = beamData["color"];
		p.c = color;
		p.x = (S32)(vec.mV[VY]+rec.getCenterX());
		p.y = (S32)(vec.mV[VZ]+rec.getCenterY());
		
						
		self->dots.push_back(p);
	}
	
}

void LggBeamMap::show(BOOL showin)
{
	//lggBeamMapFloater* beam_floater = 
	if(showin)
	lggBeamMapFloater::showInstance();
	//beam_floater->update();
}
