/* Copyright (c) 2009
 *
 * Greg Hendrickson (LordGregGreg Back) All rights reserved.
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
#ifndef  LGGIRCDATA
#define LGGIRCDATA

#include "llviewerprecompiledheaders.h"

#include "llfile.h"
#include "llsdserialize.h"
class lggIrcData
{
	public:
		lggIrcData(std::string iserver,
		std::string iname,
		std::string iport,
		std::string inick,
		std::string ichannel,
		std::string iNickPassword,
		std::string iChannelpassword,
		std::string iServerPassword,
		BOOL iautoLogin,
		LLUUID iid);
		lggIrcData();

		~lggIrcData();
		std::string server;
		std::string name;
		std::string port;
		std::string nick;
		std::string channel;
		std::string serverPassword;
		std::string nickPassword;
		std::string channelPassword;
		BOOL autoLogin;
		LLUUID id;
	public:
		LLSD toLLSD();
		static lggIrcData fromLLSD(LLSD inputData);
		//static lggIrcData newDefault();
		std::string toString();
		// List sorted by name.
		friend bool operator==(const lggIrcData &a, const lggIrcData &b);		// Return a == b
		friend bool operator<(const lggIrcData &a, const lggIrcData &b);		// Return a < b	
};
inline bool operator==(const lggIrcData &a, const lggIrcData &b)
{
	return (  a.id == b.id);
}
inline bool operator<(const lggIrcData &a, const lggIrcData &b)
{
	return (  a.name < b.name);
}


#endif
