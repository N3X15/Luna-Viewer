/**
* @file flex_constants.h
* @brief FlexLife Viewer Constants
*
* Constants that control many of the viewer aspects
* are stored here for accessibility.
*/
#ifndef FLEX_CONSTANTS_H
#define FLEX_CONSTANTS_H

#include <string>

/**
FlexLife Viewer Version
  Mostly used for the update screen and shit
  http://flexlife.nexisonline.net/login/index.php/os/major/minor/minor/build/revision
  (Revision is unused for now.)
*/
const int FLEX_VERSION_MAJOR = 0;
const int FLEX_VERSION_MINOR = 0;
const int FLEX_VERSION_PATCH = 1;
const int FLEX_VERSION_BUILD = 75; 
static const char FLEX_VERSION_REVISION[] = "$Rev$";

/**
	FlexLife Viewer Update Check URL

	checks for updates via a simple HTTP GET to the following URL plus the
	URL convention above.
*/
static const char	FLEX_UPDATE_HOST[]	= "flex.nexisonline.net";
static const int	FLEX_UPDATE_PORT	= 80;
//static const int	FLEX_UPDATE_PORT		= 443;						// ws-tux-001 can't do XMLRPC yet, citadel is being fitted with nginx.  
																		//  Even though update doesn't use XMLRPC, I need to have XMLRPC available for other shit in the future.

// Client tag UUID, used by Emerald-based clients (and our tag database) to check viewer tags.
// Used in 
//			llprimitive/llprimitive.cpp 	Emerald client detection
// 			newview/llvoavatar.cpp 			Statistics tracking and to prevent invalidation of client tags
static const std::string FLEX_CLIENT_TAG = "806a8249-e235-f051-ac4c-0a58b570f1c1";

#endif

