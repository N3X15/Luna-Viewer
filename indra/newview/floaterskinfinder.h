/* Copyright (c) 2010 Katharine Berry All rights reserved.
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
 *   3. Neither the name Katharine Berry nor the names of any contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KATHARINE BERRY AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KATHARINE BERRY OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLOATERSKINFINDER_H
#define FLOATERSKINFINDER_H

#include "llfloaterhtmlsimple.h"

class FloaterSkinfinder : public LLFloaterHtmlSimple, public LLFloaterSingleton<FloaterSkinfinder>
{
	friend class LLUISingleton<FloaterSkinfinder, VisibilityPolicy<LLFloater> >;
	
public:
	FloaterSkinfinder(const LLSD& key);
	~FloaterSkinfinder() { }
	
	static void show(void*);
};

class AscentSkinDownloader : public LLHTTPClient::Responder
{
public:
	AscentSkinDownloader(const std::string& skin_name, const std::string& display_name);
	~AscentSkinDownloader() { }
	void error(U32 status, const std::string& reason);
	void completedRaw(
							  U32 status,
							  const std::string& reason,
							  const LLChannelDescriptors& channels,
							  const LLIOPipe::buffer_ptr_t& buffer);
private:
	std::string mSkinName;
	std::string mDisplayName;
	std::string mSkinDir;
	
	bool decompressSkin(const std::string& zip, const std::string& dir);
	static void download_complete_dismissed(const LLSD& notification, const LLSD& response);
};

#endif // FLOATERSKINFINDER_H
