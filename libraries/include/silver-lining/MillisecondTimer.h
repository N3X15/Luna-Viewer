// Copyright (c) 2007-2009 Sundog Software, LLC. All rights reserved worldwide.

/** \file MillisecondTimer.h
	\brief Exposes an interface applications may use to manipulate time.
*/

#ifndef MILLISECOND_TIMER_H
#define MILLISECOND_TIMER_H

namespace SilverLining
{
/** Exposes an interface to allow applications to control millisecond-level
time. This timer is used to move cloud decks with respect to wind, and to
simulate the real-time passage of time when AtmosphericConditions::EnableTimePassage()
is activated.

By default, SilverLining will call the Win32 function timeGetTime() which
will result in a simulation of real time that always moves forward. However,
applications that want to move time backward for replays, or applications that
want to simulate the acceleration of time, may pass in their own concept of
elapsed milliseconds by extending this class.

To use your own MillisecondTimer in place of SilverLining's default implementation,
instantiate your class that extends MillisecondTimer and implements its own
GetMilliseconds() method. Then, pass this class instance to
AtmosphericConditions::SetMillisecondTimer().
*/

class MillisecondTimer
{
public:
    /** Default constructor. */
    MillisecondTimer() {}

    /** Virtual destructor. */
    virtual ~MillisecondTimer() {}

    /** Returns elapsed milliseconds. It doesn't matter what time this is relative
    to; SilverLining will only use the difference between subsequent calls to
    GetMilliseconds() to simulate the movement of the clouds and astronomical
    objects over time. The base class implementation of GetMilliseconds()
    simply calls the system function timeGetTime().

    Avoid returning a value of zero; this value is special internally to flag
    the first frame.
    */
    virtual unsigned long GetMilliseconds() const;
};
}

#endif