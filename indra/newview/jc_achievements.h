/* Copyright (c) 2009
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

#include "llviewerprecompiledheaders.h"

#include "llcontrol.h"

#include "jc_lslviewerbridge.h"

class JCAchievementTracker : public LLEventTimer, public LLHTTPClient::Responder
{
public:
	JCAchievementTracker();
	~JCAchievementTracker();
	BOOL tick();

	static void loadAchievementData();

	static void saveAchievementData();

	static void AchievementSecretLoaded();

	void completedRaw(U32 status,
						const std::string& reason,
						const LLChannelDescriptors& channels,
							const LLIOPipe::buffer_ptr_t& buffer);

	static std::string AchievementPath;

	static std::string avatar_secret;

	static LLSD AchievementData;

private:
	static JCAchievementTracker* sInstance;
};

class JCHTTPSecretCallback : public JCBridgeCallback
{
public:
	JCHTTPSecretCallback()
	{
		//avatar = avvie;
	}

	void fire(LLSD data)
	{
		JCAchievementTracker::avatar_secret = data[0].asString();
	}
};


class LLEventPollResponder : public LLHTTPClient::Responder
	{
	public:
		
		static LLHTTPClient::ResponderPtr start(const std::string& pollURL, const LLHost& sender);
		void stop();
		
		void makeRequest();

	private:
		LLEventPollResponder(const std::string&	pollURL, const LLHost& sender);
		~LLEventPollResponder();

		
		void handleMessage(const LLSD& content);
		virtual	void error(U32 status, const std::string& reason);
		virtual	void result(const LLSD&	content);

		virtual void completedRaw(U32 status,
									const std::string& reason,
									const LLChannelDescriptors& channels,
									const LLIOPipe::buffer_ptr_t& buffer);
	private:

		bool	mDone;

		std::string			mPollURL;
		std::string			mSender;
		
		LLSD	mAcknowledge;
		
		// these are only here for debugging so	we can see which poller	is which
		static int sCount;
		int	mCount;
		S32 mErrorCount;
	};