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

#include "llquaternion.h"
#include "v3math.h"
#include "llframetimer.h"

class GUS : public LLSingleton<GUS>
{
	public:
		GUS();
		static const std::string ping_command;
		static const std::string sync_command;
		static const std::string change_channel;
		static LLFrameTimer ping_timer;
		static void ping();
		static void sync(S32 channel);
		static void chan(S32 channel);
		static bool streamData();
		static bool fastEvent();
		static void FELimiter_dec(){ FELimiter = (FELimiter>0)?FELimiter-1:0; }
		static bool Enabled;
		static F32 Refresh;
		static bool FEEnabled;
		static F32 FERefresh;
		static S32 ChatChannel;
		static void whisper(S32, std::string, bool force = false);
		static void say(S32, std::string, bool force = false);
		static std::string sQuat(const LLQuaternion &quat, bool nansmakesnull = true);
		static std::string sVec3(const LLVector3 &vec, bool nansmakesnull = true);
		static S32 rotrad(LLQuaternion &a, LLQuaternion &b);
		static const LLVector3 NaNVector; //hack!
		static const LLQuaternion NaNRotation; //hack!
		static bool pinged();
	private:
		static void initGUS();
		static void gusEnabled(const LLSD &data);
		static void gusRefresh(const LLSD &data);
		static void gusFEEnabled(const LLSD &data);
		static void gusFERefresh(const LLSD &data);
		static std::string genMessage();
		static std::string genFEMessage();
		static bool updateValues();
		static bool FEupdateValues();
		static unsigned char FELimiter;
		static void chatmessage(S32, std::string, U8);
	private:
		static LLQuaternion getEyeRot();
		static LLQuaternion lastEyeRot;
		static bool getEyelidState();
		static bool lastEyelidState;
		static LLVector3 getLookAtVector();
		static LLVector3 lastLookAtVector;
};