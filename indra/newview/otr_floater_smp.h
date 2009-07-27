/* otr_floater_smp.h - SMP dialog box
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

#ifndef OTR_FLOATER_SMP_H_INCLUDED
#define OTR_FLOATER_SMP_H_INCLUDED 1

#include "llfloater.h"

class OtrFloaterSmp
: public LLFloater
{
private:
    LLUUID mOtherParticipantID;
    LLUUID mSessionUUID;
    std::string mMyFingerprint;
    std::string mOtherFingerprint;

    void doComboMenu();
    static void onClickCombo(LLUICtrl* source, void* userdata);
	static void onClickHelp(void* userdata);
	static void onClickCancel(void* userdata);
    void doClickAuth();
	static void onClickAuth(void* userdata);

public:
	OtrFloaterSmp(LLUUID session_id, LLUUID other_id,
                  std::string my_fingerprint, std::string other_fingerprint);
	virtual ~OtrFloaterSmp();
	/*virtual*/ BOOL postBuild();

	void show();
};
#endif // ndef OTR_FLOATER_SMP_H_INCLUDED
