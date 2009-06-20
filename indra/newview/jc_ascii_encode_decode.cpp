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
#include "jc_ascii_encode_decode.h"

#include <cstring>
//#include "modp_b85.h"

/*std::string JCStringUnsignedChar::encode(const unsigned char *buffer)
{

	return "<~"+(std::string)((char*)buffer)+"~>";
}*/

S32 JCStringUnsignedChar::decode(std::string str, char *buffer, size_t buf_len)
{
	/*str = str.substr(prepend.length(),str.length() - append.length());
	if(str.length() > length)
	{
		//return -2;//your choice as to what to do here
		str = str.substr(0,length-1);
	}// :s i suppose this was the intended purpose of the length being provided
	//could also return a error code there*/
	//str = str.substr(2,str.length()-2);
	//buffer = ((unsigned char*)(str.c_str()));
	return 1;
}


std::string JCStringUnsignedChar::encode(const char* src_buf, size_t src_len)
{
	if (!src_buf || !src_len) {
		return "";
	}

	std::string result("<~");
    
	result.reserve(src_len / 4 * 5 + 16);

	for (const char* src_ptr = src_buf, *src_end = src_buf + src_len;
	src_ptr < src_end;
	src_len -= 4
	)
	{
		const size_t l = src_len > 4 ? 4 : src_len;
		const size_t grplen = l + 1;
		unsigned long val = 0;
		for (long shft = 8 * (l - 1); shft > 0; shft -= 8, ++src_ptr) {
			val |= ((unsigned char) *src_ptr) << shft;
		}

		// special case - if values are all zero, output 'z'
		//if (val == 0 && grplen == 5) {
		//	result += "z";
		//	continue;
		//}

		char out[5] = { 0 };
		for (int i = 4; i >= 0; --i) {
			const unsigned long quot = val / 85;
			const unsigned long rem = val - quot * 85; // val % 85
			val = quot;
			out[i] = char(rem + '!');
		}

		result.append(out, grplen);
	}

	result.append("~>");
	
	return result;
}
