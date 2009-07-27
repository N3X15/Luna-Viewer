/* otr_wrapper.h - wrap libotr for use in the second life viewer
   $PLOTR$ See http://www.cypherpunks.ca/otr/

   Copyright (C) 2009 Chris Tuchs

   This is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
 
   This is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
 
   You should have received a copy of the GNU Lesser General Public
   License along with Libgcrypt; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef OTR_WRAPPER_H_INCLUDED
#define OTR_WRAPPER_H_INCLUDED 1

extern "C" {
#if WIN32
#include <unistd.h>     // various defines needed for OTR on windows
#endif
#include <gcrypt.h>     // gcrypt dependancies
#include <userstate.h>  // OTR
#include <proto.h>      // OTR
#include <privkey.h>    // OTR
#include <../libotr/libotr-3.2.0/src/message.h> // OTR
}

class OTR_Wrapper
{
private:
    OtrlUserState userstate; // Global state of OTR: known public keys etc.
    OtrlMessageAppOps uistate; // UI functions collected into a structure for libotr   
    OTR_Wrapper();
    ~OTR_Wrapper();

public:
    static void init();
    OtrlUserState get_userstate()    { return this->userstate; }
    OtrlMessageAppOps *get_uistate() { return &(this->uistate); }
    const char * get_protocolid()    { return "SecondLife"; }
};

extern OTR_Wrapper *gOTR; // the singleton OTR wrapper

// $TODO$ better file names/paths.  Should be per avy, especially the
// private keys, currently it's per install directory.

#define OTR_PRIVATE_KEYS_FILE "OTR-my-private-key.dat"
#define OTR_PUBLIC_KEYS_FILE  "OTR-public-keys.dat"

#endif // ndef OTR_WRAPPER_H_INCLUDED
