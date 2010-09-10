/** 
 * @file llfixedbuffer.h
 * @brief A fixed size buffer of lines.
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#ifndef LL_LLFIXEDBUFFER_H
#define LL_LLFIXEDBUFFER_H

#include "timer.h"
#include <deque>
#include <string>
#include "llstring.h"
#include "llthread.h"
#include "llerrorcontrol.h"

//  fixed buffer implementation
class LLFixedBuffer : public LLLineBuffer
{
public:
	LLFixedBuffer(const U32 max_lines = 20);
	~LLFixedBuffer();

	LLTimer	mTimer;
	U32		mMaxLines;
	std::deque<LLWString>	mLines;
	std::deque<F32>			mAddTimes;
	std::deque<S32>			mLineLengths;

	/*virtual*/ void clear(); // Clear the buffer, and reset it.

	/*virtual*/ void addLine(const std::string& utf8line);

	void setMaxLines(S32 max_lines);
	
protected:
	void removeExtraLines();
	void addWLine(const LLWString& line);

protected:
	LLMutex mMutex ;
};

#endif //LL_FIXED_BUFFER_H
