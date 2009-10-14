/* otr_floater_smp_progress.cpp - SMP progress dialog box
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

#include "llviewerprecompiledheaders.h"

#include "llagent.h"
#include "llcachename.h"
#include "llimpanel.h"
#include "llprogressbar.h"
#include "lluictrlfactory.h"
#include "llweb.h"

#include "otr_wrapper.h"
#include "otr_floater_smp_progress.h"

void OtrFloaterSmpProgress::init()
{
    LLUICtrlFactory::getInstance()->buildFloater(this, "otr_floater_smp_progress.xml");
    childSetAction("otr_smp_prog_help_btn",   onClickHelp, this);
    childSetAction("otr_smp_prog_cancel_btn", onClickCancel, this);
    childSetAction("otr_smp_prog_ok_btn",     onClickOk, this);
    setDefaultBtn("otr_smp_prog_cancel_btn");
    LLUIString title = mIsReply
        ? getString("otr_smp_prog_name_auth_me")
        : getString("otr_smp_prog_I_auth_name");
    std::string them;
    if (!gCacheName->getFullName(mOtherParticipantUUID, them)) them = getString("otr_generic_name");
    title.setArg("[NAME]", them);
    setTitle(title.getString());
}

OtrFloaterSmpProgress::OtrFloaterSmpProgress(
    LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
    std::string a_question, std::string a_secret_answer, bool is_reply)
:   LLFloater(std::string("otr_floater_smp_progress"), std::string("OtrFloaterSmpProgRect"), LLStringUtil::null),
        mIMPanel(im_panel), mIsReply(is_reply),
        mSessionUUID(session_id), mOtherParticipantUUID(other_id),
        mQuestion(a_question), mSecretAnswer(a_secret_answer),
        mSecretAnswerOnly(false)
{
    init();
    if (!gOTR)
    {
        llwarns << "$PLOTR$ Can't find gOTR." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    if (!mIMPanel)
    {
        llwarns << "$PLOTR$ Can't find mIMPanel." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    ConnContext *context = mIMPanel->getOtrContext();
    if (!context)
    {
        llwarns << "$PLOTR$ Can't find context." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    if (mIsReply)
    {
        otrl_message_respond_smp(
            gOTR->get_userstate(), 
            gOTR->get_uistate(), 
            &mSessionUUID,
            context,
            (const unsigned char *)mSecretAnswer.c_str(),
            mSecretAnswer.length());
        setPercent(50);
        setStatusName("otr_smp_prog_name_auth_me");
    }
    else
    {
        otrl_message_initiate_smp_q(
            gOTR->get_userstate(), 
            gOTR->get_uistate(), 
            &mSessionUUID,
            context,
            mQuestion.c_str(),
            (const unsigned char *)mSecretAnswer.c_str(),
            mSecretAnswer.length());
        setStatusName("otr_smp_prog_I_auth_name");
    }
}

OtrFloaterSmpProgress::OtrFloaterSmpProgress(
    LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
    std::string a_secret_answer, bool is_reply)
:   LLFloater(std::string("otr_floater_smp_progress"), std::string("OtrFloaterSmpProgRect"), LLStringUtil::null),
        mIMPanel(im_panel), mIsReply(is_reply),
        mSessionUUID(session_id), mOtherParticipantUUID(other_id),
        mSecretAnswer(a_secret_answer),
        mSecretAnswerOnly(true)
{
    init();
    if (!gOTR)
    {
        llwarns << "$PLOTR$ Can't find gOTR." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    if (!mIMPanel)
    {
        llwarns << "$PLOTR$ Can't find mIMPanel." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    ConnContext *context = mIMPanel->getOtrContext();
    if (!context)
    {
        llwarns << "$PLOTR$ Can't find context." << llendl;
        setFinalStatus("otr_smp_prog_auth_errored");
        return;
    }
    if (mIsReply)
    {
        otrl_message_respond_smp(
            gOTR->get_userstate(), 
            gOTR->get_uistate(), 
            &mSessionUUID,
            context,
            (const unsigned char *)mSecretAnswer.c_str(),
            mSecretAnswer.length());
        setPercent(50);
        setStatusName("otr_smp_prog_name_auth_me");
    }
    else
    {
        otrl_message_initiate_smp(
            gOTR->get_userstate(), 
            gOTR->get_uistate(), 
            &mSessionUUID,
            context,
            (const unsigned char *)mSecretAnswer.c_str(),
            mSecretAnswer.length());
        setStatusName("otr_smp_prog_I_auth_name");
    }
}

OtrFloaterSmpProgress::~OtrFloaterSmpProgress()
{
}

BOOL OtrFloaterSmpProgress::postBuild()
{
    mProgressBar = getChild<LLProgressBar>("otr_smp_prog_progress_bar");
    LLButton* cancel_btn = getChild<LLButton>("otr_smp_prog_cancel_btn");
    cancel_btn->setEnabled(TRUE);
    LLButton* ok_btn = getChild<LLButton>("otr_smp_prog_ok_btn");
    ok_btn->setEnabled(FALSE);
    return TRUE;
}

void OtrFloaterSmpProgress::show()
{
    this->open(); /*Flawfinder: ignore*/
    this->center();
}

void OtrFloaterSmpProgress::setPercent(float percent)
{
    mProgressBar->setPercent(percent);
    if (100 <= percent)
    {
        LLButton* cancel_btn = getChild<LLButton>("otr_smp_prog_cancel_btn");
        cancel_btn->setEnabled(FALSE);
        LLButton* ok_btn = getChild<LLButton>("otr_smp_prog_ok_btn");
        ok_btn->setEnabled(TRUE);
    }
    else
    {
        LLButton* cancel_btn = getChild<LLButton>("otr_smp_prog_cancel_btn");
        cancel_btn->setEnabled(TRUE);
        LLButton* ok_btn = getChild<LLButton>("otr_smp_prog_ok_btn");
        ok_btn->setEnabled(FALSE);
    }
}

void OtrFloaterSmpProgress::setStatus(const char *message_string_name)
{
    LLTextBox *status = getChild<LLTextBox>("otr_smp_prog_status");
    if (!status)
    {
        llwarns << "$PLOTR$ Can't find OTR progress status textbox" << llendl;
        return;
    }
    status->setWrappedText(getString(message_string_name));
}

void OtrFloaterSmpProgress::setStatusName(const char *message_string_name)
{
    LLTextBox *status = getChild<LLTextBox>("otr_smp_prog_status");
    if (!status)
    {
        llwarns << "$PLOTR$ Can't find OTR progress status textbox" << llendl;
        return;
    }
    LLUIString msg = getString(message_string_name);
    std::string them;
    if (!gCacheName->getFullName(mOtherParticipantUUID, them)) them = getString("otr_generic_name");
    msg.setArg("[NAME]", them);
    status->setWrappedText(msg.getString());
}

void OtrFloaterSmpProgress::setFinalStatus(const char *message_string_name)
{
    setPercent(100);
    setStatusName(message_string_name);
}

// static
void OtrFloaterSmpProgress::onClickHelp(void*)
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/help/3.2.0/authenticate.php");
}

// static
void OtrFloaterSmpProgress::onClickCancel(void* userdata)
{
    OtrFloaterSmpProgress *self= (OtrFloaterSmpProgress*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP PROG dialog." << llendl;
        return;
    }
    if (!self->mIMPanel)
    {
        llwarns << "$PLOTR$ Can't find mIMPanel." << llendl;
        return;
    }
    ConnContext *context = self->mIMPanel->getOtrContext();
    if (!context)
    {
        llwarns << "$PLOTR$ Can't find context." << llendl;
        return;
    }
    otrl_message_abort_smp(
        gOTR->get_userstate(), 
        gOTR->get_uistate(), 
        &(self->mSessionUUID),
        context);
    self->close();
}

// static
void OtrFloaterSmpProgress::onClickOk(void* userdata)
{
    OtrFloaterSmpProgress *self= (OtrFloaterSmpProgress*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP PROG dialog." << llendl;
        return;
    }
    self->close();
}

// virtual
void OtrFloaterSmpProgress::onClose(bool app_quitting)
{
    destroy();
    if (!mIMPanel)
    {
        llwarns << "$PLOTR$ Can't find mIMPanel." << llendl;
    }
    else
    {
        mIMPanel->endSmpProgress();
    }
}
