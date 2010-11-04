/* Copyright (c) 2010
 *
 * Modular Systems All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS �AS IS�
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

#include <map>

class ModularSystemsDownloader : public LLHTTPClient::Responder
{
public:
	ModularSystemsDownloader(void (*callback)(U32, std::string));
	~ModularSystemsDownloader();
	virtual void error(U32 status, const std::string& reason);
	virtual void completedRaw(
		U32 status,
		const std::string& reason,
		const LLChannelDescriptors& channels,
		const LLIOPipe::buffer_ptr_t& buffer);
private:
	void	(*mCallback)(U32, std::string);
};

class PhoenixViewerLink
{
	PhoenixViewerLink();
	~PhoenixViewerLink();
	static PhoenixViewerLink* sInstance;
public:
	static PhoenixViewerLink* getInstance();

	void start_download();
	void downloadClientTags();

	static void msdata(U32 status, std::string body);
	static void msblacklistquery(U32 status, std::string body);
	static void msblacklist(U32 status, std::string body);
	static void updateClientTags(U32 status,std::string body);
	static void updateClientTagsLocal();
	static const U8 EM_SUPPORT		= 0x01;
	static const U8 EM_DEVELOPER	= 0x02;
	static const U8 PH_BETA			= 0x01;
	static const U8 PH_RELEASE		= 0x02;
	//static const U8 x = 0x04;
	//static const U8 x = 0x08;
	//static const U8 x = 0x10;

	std::map<LLUUID, U8> personnel;
	std::map<std::string, U8> versions2;

	static BOOL is_BetaVersion(std::string version);
	static BOOL is_ReleaseVersion(std::string version);
	static BOOL is_developer(LLUUID id);
	static BOOL is_support(LLUUID id);

	static BOOL allowed_login();

	static std::string processRequestForInfo(LLUUID requester,std::string message, std::string name, LLUUID sessionid);
	static std::string getMyInfo(int part =0);
	static void callbackPhoenixReqInfo(const LLSD &notification, const LLSD &response);
	static void sendInfo(LLUUID destination, LLUUID sessionid, std::string myName, EInstantMessage dialog);

	static LLSD phoenix_tags;
	std::set<std::string> blocked_versions;
	static LLSD blocked_login_info;
	std::string ms_motd;
	static BOOL isMSDone() { return msDataDone; }
private:
	static BOOL msDataDone;
	static std::string blacklist_version;


};


