/* Copyright (c) 2009
 *
 * Jeffrey MacLellan(Zwagoth Klaar) All rights reserved.
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
 *   3. Neither the name Jeffrey MacLellan or Modular Systems, nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFREY MACLELLAN AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JEFFREY MACLELLAN OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * In other words. It's not perfect. It might break stuff, yo.
 */

#ifndef EM_KDU_H
#define EM_KDU_H

typedef unsigned char U8;
typedef signed char S8;

class EMImageDims
{
public:
	EMImageDims()
		: mHeight(0),
		mWidth(0),
		mComponents(0)
	{}
	~EMImageDims() {}
	int mHeight;
	int mWidth;
	int	mComponents;
};

class EMImageData
{
public:
	EMImageData()
		: mHeight(-1),
		mWidth(-1),
		mFirstComp(-1),
		mComponents(-1),
		mData(0),
		mLength(0),
		mDiscard(-1)
	{}
	~EMImageData() {}
	int mHeight;
	int mWidth;
	int mFirstComp;
	int mComponents;
	U8* mData;
	unsigned int mLength;
	S8 mDiscard;
};

class EMKDUImpl
{	
public:
	virtual ~EMKDUImpl() {}
	virtual bool encodeData(EMImageData* image_data) = 0;
	virtual bool decodeData(EMImageData* image_data) = 0;
	virtual EMImageDims getMetadata(EMImageData* image_data) = 0;
protected:
	virtual bool validateData(EMImageData* image_data, bool for_encode_decode = false) = 0;
};

#endif //EM_KDU_H
