/** 
 * @file llchainio.h
 * @author Phoenix
 * @date 2005-08-04
 * @brief This class declares the interface for constructing io chains.
 *
 * $LicenseInfo:firstyear=2005&license=viewergpl$
 * 
 * Copyright (c) 2005-2008, Linden Research, Inc.
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
 * online at http://secondlifegrid.net/programs/open_source/licensing/flossexception
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

#ifndef LL_LLCHAINIO_H
#define LL_LLCHAINIO_H

#include "llpumpio.h"

/** 
 * @class LLDeferredChain
 * @brief This class allows easy addition of a chain which will sleep
 * and then process another chain.
 */
class LLDeferredChain
{
public:
	/**
	 * @brief Add a chain to a pump in a finite # of seconds
	 *
	 * @prarm pump The pump to work on.
	 * @prarm in_seconds The number of seconds from now when chain should start.
	 * @prarm chain The chain to add in in_seconds seconds.
	 * @prarm chain_timeout timeout for chain on the pump.
	 * @return Returns true if the operation was queued.
	 */
	static bool addToPump(
		LLPumpIO* pump,
		F32 in_seconds,
		const LLPumpIO::chain_t& chain,
		F32 chain_timeout);
};

/** 
 * @class LLChainIOFactory
 * @brief This class is an abstract base class for building io chains.
 *
 * This declares an abstract base class for a chain factory. The
 * factory is used to connect an input pipe to the first pipe in the
 * chain, and an output pipe to the last pipe in the chain. This will
 * allow easy construction for buffer based io like services to for
 * API centered IO while abstracting the input and output to simple
 * data passing.
 * To use this class, you should derive a class which implements the
 * <code>build</code> method.
 */
class LLChainIOFactory
{
public:
	// Constructor
	LLChainIOFactory();

	// Destructor
	virtual ~LLChainIOFactory();

	/** 
	 * @brief Build the chian with in as the first and end as the last
	 *
	 * The caller of the LLChainIOFactory is responsible for managing
	 * the memory of the in pipe. All of the chains generated by the
	 * factory will be ref counted as usual, so the caller will also
	 * need to break the links in the chain.
	 * @param chain The chain which will have new pipes appended
	 * @param context A context for use by this factory if you choose
	 * @retrun Returns true if the call was successful.
	 */
	virtual bool build(LLPumpIO::chain_t& chain, LLSD context) const = 0;

protected:
};

/** 
 * @class LLSimpleIOFactory
 * @brief Basic implementation for making a factory that returns a
 * 'chain' of one object
 */
template<class Pipe>
class LLSimpleIOFactory : public LLChainIOFactory
{
public:
	virtual bool build(LLPumpIO::chain_t& chain, LLSD context) const
	{
		chain.push_back(LLIOPipe::ptr_t(new Pipe));
		return true;
	}
};

/** 
 * @class LLCloneIOFactory
 * @brief Implementation for a facory which copies a particular pipe.
 */
template<class Pipe>
class LLCloneIOFactory : public LLChainIOFactory
{
public:
	LLCloneIOFactory(Pipe* original) :
		mHandle(original),
		mOriginal(original) {}

	virtual bool build(LLPumpIO::chain_t& chain, LLSD context) const
	{
		chain.push_back(LLIOPipe::ptr_t(new Pipe(*mOriginal)));
		return true;
	}

protected:
	LLIOPipe::ptr_t mHandle;
	Pipe* mOriginal;
};

#endif // LL_LLCHAINIO_H
