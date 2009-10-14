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

#include "llfloater.h"
#include "lluuid.h"
#include "llstring.h"

struct AssetSubjectDetails
{
	LLUUID id;
	std::string name;
	std::string desc;
	LLUUID owner_id;
	LLUUID group_id;
};

class JCAssetComparer : public LLFloater
{
public:

	JCAssetComparer();
	virtual ~JCAssetComparer();

	/*virtual*/ BOOL postBuild();
	/*virtual*/ void close(bool app = false);

	static void toggle();

private:
	static JCAssetComparer* sInstance;

	static void reload(void* data);

	static void onClickObjPickerA(void *userdata);
	static void onClickObjPickerB(void *userdata);
	static void closePickToolA(void *userdata);
	static void closePickToolB(void *userdata);

	static void getData(LLUUID object);

	static AssetSubjectDetails subjectA;
	static AssetSubjectDetails subjectB;

public:
	static JCAssetComparer* getInstance(){ return sInstance; }
};
