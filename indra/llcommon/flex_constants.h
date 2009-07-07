/**
* @file shooped_constants.h
* @brief FlexLife Viewer Constants
*
* NIGGERNIGGERNIGGERNIGGERNIGGERNIGGER
*/
#ifndef SHOOPED_CONSTANTS_H
#define SHOOPED_CONSTANTS_H

#include <string>

/**
FlexLife Viewer Version
  Mostly used for the update screen and shit
  http://FlexLife Viewer.nexisonline.net/login/index.php/os/major/minor/minor/build/revision
  (Revision is just for lulz)
*/
const int SHL_VERSION_MAJOR	= 0;
const int SHL_VERSION_MINOR = 3;
const int SHL_VERSION_PATCH = 0;
const int SHL_VERSION_BUILD = 75; 
static const char SHL_VERSION_REVISION[] = "$Rev: 75 $";

/**
	Faked Client Hash - Needed to bypass client blacklist.

	To obtain the official SL viewer's CLient Hash:
	1. Open up the official secondlife client for your platform.
	2. Edit > Preferences
	3. About button
	4. Copy the Viewer Digest entry
	5. ???
	6. PROFIT

	Then, if they try to ban the client hash from SL, they essentially ban everyone
	from the grid :3
*/
#if LL_WINDOWS || LL_LINUX
static const char SHL_FAKED_CLIHASH[]	= "6b1bac12-2a86-e1f2-aa5e-3108db5c8eeb";
//#elif LL_DARWIN	// Don't have mac client hashes yet
//#elif LL_SOLARIS	// Don't have Solaris support yet (And who the fuck does gaming on Solaris?! What's next, SL on a Cray?)
#endif

/**
	FlexLife Viewer Update Check URL

	checks for updates via a simple HTTP GET to the following URL plus the
	URL convention above.
*/
static const char	SHL_UPDATE_HOST[]	= "FlexLife Viewer.nexisonline.net";
static const int	SHL_UPDATE_PORT		= 80;
//static const int	SHL_UPDATE_PORT		= 443;								// ws-tux-001 can't do XMLRPC yet, citadel is being fitted with nginx.  
																		//  Even though update doesn't use XMLRPC, I need to have XMLRPC available for other shit in the future.

// What is a sensible amount of LLNotifyBoxes to keep open at any one time before enabling flood control?
const int  SHL_SENSIBLE_NOTIFY_BOX_COUNT = 20;

// On startup, initiate unit tests?
//#define SHL_RUN_UNIT_TESTS 1

#endif

