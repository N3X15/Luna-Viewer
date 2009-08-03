/* otr_floater_smp_dialog.h - SMP dialog box
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
   License along with the viewer; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef OTR_FLOATER_SMP_DIALOG_H_INCLUDED
#define OTR_FLOATER_SMP_DIALOG_H_INCLUDED 1

#include "llfloater.h"
class LLFloaterIMPanel;

class OtrFloaterSmpDialog
: public LLFloater
{
private:
    LLUUID mOtherParticipantUUID;
    LLUUID mSessionUUID;
    std::string mQuestion;
    std::string mMyFingerprint;
    std::string mOtherFingerprint;
    LLFloaterIMPanel *mIMPanel;
    bool mIsReply;

    static void onClickHelp(void* userdata);
    static void onClickCancel(void* userdata);
    void doComboMenu();
    static void onClickCombo(LLUICtrl* source, void* userdata);
    void doVeriMenu();
    static void onClickVeri(LLUICtrl* source, void* userdata);
    void doClickAuth();
    static void onClickAuth(void* userdata);
    std::string getStringName(const char *message_name);

public:
    OtrFloaterSmpDialog(LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
                        std::string my_fingerprint, std::string other_fingerprint);
    OtrFloaterSmpDialog(LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
                        std::string question, OtrlTLV *tlv);
    OtrFloaterSmpDialog(LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
                        OtrlTLV *tlv);
    virtual ~OtrFloaterSmpDialog();
    /*virtual*/ BOOL postBuild();
    /*virtual*/ void onClose(bool app_quitting);

    void show();
    void init();
};
#endif // ndef OTR_FLOATER_SMP_DIALOG_H_INCLUDED
