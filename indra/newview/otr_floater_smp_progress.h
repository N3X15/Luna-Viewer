/* otr_floater_smp_progress.h - SMP progress dialog box
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

#ifndef OTR_FLOATER_SMP_PROGRESS_H_INCLUDED
#define OTR_FLOATER_SMP_PROGRESS_H_INCLUDED 1

#include "llfloater.h"
class LLFloaterIMPanel;
class LLProgressBar;

class OtrFloaterSmpProgress
: public LLFloater
{
private:
    LLUUID mOtherParticipantUUID;
    LLUUID mSessionUUID;
    std::string mQuestion;
    std::string mSecretAnswer;
    bool mSecretAnswerOnly;
    LLFloaterIMPanel *mIMPanel;
    LLProgressBar* mProgressBar;
    bool mIsReply;

    static void onClickHelp(void* userdata);
    static void onClickCancel(void* userdata);
    static void onClickOk(void* userdata);

    void init();
public:
    OtrFloaterSmpProgress(LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
                          std::string a_question, std::string a_secret_answer, bool is_reply = false);
    OtrFloaterSmpProgress(LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
                          std::string a_secret_answer, bool is_reply = false);
    void setPercent(float percent);
    void setStatus(const char *message_string_name);
    void setStatusName(const char *message_string_name);
    void setFinalStatus(const char *message_string_name);
    virtual ~OtrFloaterSmpProgress();
    /*virtual*/ BOOL postBuild();
    /*virtual*/ void onClose(bool app_quitting);

    void show();
};
#endif // ndef OTR_FLOATER_SMP_PROGRESS_H_INCLUDED
