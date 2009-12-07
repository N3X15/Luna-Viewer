/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
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
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
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

#include "llfloater.h"
#include "lluuid.h"
#include "llstring.h"
#include "llviewerobject.h"

class LLMessageSystem;
class LLViewerObject;


const U32 MY_REQUEST 		= 0x01 << 0;

struct AObjectDetails
{
	LLUUID id;
	std::string name;
	std::string desc;
	LLUUID owner_id;
	LLUUID group_id;
};

class JCFloaterAreaSearch : public LLFloater
{
public:

	JCFloaterAreaSearch();
	virtual ~JCFloaterAreaSearch();

	/*virtual*/ BOOL postBuild();
	/*virtual*/ void draw();
	/*virtual*/ void close(bool app = false);

	static void results();
	static void toggle();
	static void processObjectPropertiesFamily(LLMessageSystem *mesgsys, void **user_data);

private:

	static void search(void* data);
	static void onDoubleClick(void *userdata);
	static void onCommitLine(LLLineEditor* line, void* user_data);

	enum OBJECT_COLUMN_ORDER
	{
		LIST_OBJECT_NAME,
		LIST_OBJECT_DESC,
		LIST_OBJECT_OWNER,
		LIST_OBJECT_GROUP,
	};

	static JCFloaterAreaSearch* sInstance;

	LLScrollListCtrl* mResultList;

	static std::map<LLUUID, AObjectDetails> mObjectDetails;

	static std::string searched_name;
	static std::string searched_desc;
	static std::string searched_owner;
	static std::string searched_group;

	static bool payable_object;
	static bool scripted_object;
	static bool touch_object;
	static bool temporary_object;
	static bool sculpted_object;
	static bool flexible_object;
	static bool phantom_object;
	static bool sound_object;
	static bool particle_object;
	static bool animation_object;
	static bool inventoryadd_object;
	static bool attachment_object;
	static bool hudattachment_object;

	static void onCheckPayable(LLUICtrl* ctrl,void* user_data);
	static void onCheckTouchOnly(LLUICtrl* ctrl,void* user_data);
	static void onCheckScripted(LLUICtrl* ctrl,void* user_data);
	static void onCheckSounds(LLUICtrl* ctrl,void* user_data);
	static void onCheckParticles(LLUICtrl* ctrl,void* user_data);
	static void onCheckAnimations(LLUICtrl* ctrl,void* user_data);
	static void onCheckTemporary(LLUICtrl* ctrl,void* user_data);
	static void onCheckSculpted(LLUICtrl* ctrl,void* user_data);
	static void onCheckFlexible(LLUICtrl* ctrl,void* user_data);
	static void onCheckPhantom(LLUICtrl* ctrl,void* user_data);
	static void onCheckInventoryAdd(LLUICtrl* ctrl,void* user_data);
	static void onCheckAttachment(LLUICtrl* ctrl,void* user_data);
	static void onCheckHudAttachment(LLUICtrl* ctrl,void* user_data);


public:

	static JCFloaterAreaSearch* getInstance(){ return sInstance; }

};

