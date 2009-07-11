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
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS “AS IS”
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

	void draw();
	void clearPoints();

	LLSD getMyDataSerialized();

	// UI Handlers
	static void onClickSave(void* data);
	static void onClickClear(void* data);

	
private:
	std::vector<lggPoint> dots;
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
	LLPanel* panel = getChild<LLPanel>("beamshape_draw");
	if(panel)
	{
		panel->deleteAllChildren();
	}
}
void lggBeamMapFloater::draw()
{
	//getChild<LLPanel>("beamshape_draw")->setBackgroundColor(getChild<LLColorSwatchCtrl>("back_color_swatch")->get());
	LLFloater::draw();

	
	for(int i = 0; i < (int)dots.size();i++)
	{	
		gGL.color4fv(dots[i].c.mV);
		gl_circle_2d(dots[i].x,dots[i].y,7.0f,(S32)30,true);
	}
	gGL.pushMatrix();
	
}

lggBeamMapFloater::~lggBeamMapFloater()
{
	//if(mCallback) mCallback->detach();
}

/*lggBeamMapFloater::lggBeamMapFloater(const LLSD& seed)
: mObjectID(), mObjectName(), mSlurl(), mOwnerID(), mOwnerName(), mOwnerIsGroup(false), lookingforRegion(false), mRegionName()
*/
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
	
	LLPanel* panel = getChild<LLPanel>("beamshape_draw");
	if(panel)
	{
		
	}
	
	return true;
}
BOOL lggBeamMapFloater::handleMouseDown(S32 x,S32 y,MASK mask)
{
	lggPoint a;
	a.x=x;
	a.y=y;
	a.c= 	LLColor4::red;//getChild<LLColorSwatchCtrl>("beam_color_swatch")->get();
	dots.push_back(a);
	
	llinfos << "we got clicked at (" << x << ", " << y << " and color was " << a.c << llendl;
	
	return LLFloater::handleMouseDown(x,y,mask);
}

void lggBeamMapFloater::update()
{
	
}
LLSD lggBeamMapFloater::getMyDataSerialized()
{
	LLSD out;
	for(int i =0; i<(int)dots.size();i++)
	{
		LLSD point;
		lggPoint t = dots[i];
		point["offset"]= LLVector3((F32)t.x,(F32)t.y,(F32)0.0f).getValue();
		point["color"] = t.c.getValue();

		out[i]=point;
	}
	return out;
}
void lggBeamMapFloater::onClickSave(void* data)
{
	lggBeamMapFloater* self = (lggBeamMapFloater*)data;
	LLFilePicker& picker = LLFilePicker::instance();
	
	std::string filename="myNewBeam.xml";
	if(!picker.getSaveFile( LLFilePicker::FFSAVE_XML, filename ) )
	{
   // User canceled save.
		return;
	}	
	filename = picker.getFirstFile();
	LLSD main;
	main["scale"] = 1.0f;
	main["data"]=self->getMyDataSerialized();
  
	llofstream export_file;
	export_file.open(filename);
	LLSDSerialize::toPrettyXML(main, export_file);
	export_file.close();
	
}

void lggBeamMapFloater::onClickClear(void* data)
{
	lggBeamMapFloater* self = (lggBeamMapFloater*)data;
	
	
	self->clearPoints();
	
}

void LggBeamMap::show(BOOL showin)
{
	//lggBeamMapFloater* beam_floater = 
	if(showin)
	lggBeamMapFloater::showInstance();
	//beam_floater->update();
}