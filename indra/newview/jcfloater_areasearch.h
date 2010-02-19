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

private:

	static void reload(void* data);
	static void search(void* data);
	static void onCommitLine(LLLineEditor* line, void* user_data);
	static void requestifneed(LLViewerObject *objectp);

	enum OBJECT_COLUMN_ORDER
	{
		LIST_OBJECT_NAME,
		LIST_OBJECT_DESC,
		LIST_OBJECT_OWNER,
		LIST_OBJECT_GROUP,
		LIST_OBJECT_PAYABLE,
		LIST_OBJECT_BUYABLE
	};

	static JCFloaterAreaSearch* sInstance;

	LLScrollListCtrl* mResultList;

	static std::map<LLUUID, AObjectDetails> mObjectDetails;

	static std::string searched_name;
	static std::string searched_desc;
	static std::string searched_owner;
	static std::string searched_group;
	static bool payable_object;
	static bool buyable_object;

	static void onDoubleClick(void *userdata);

public:

	static JCFloaterAreaSearch* getInstance(){ return sInstance; }

	static void callbackLoadOwnerName(const LLUUID& id, const std::string& first, const std::string& last, BOOL is_group, void* data);

	static void processObjectPropertiesFamily(LLMessageSystem* msg, void** user_data);
};
