/* otr_floater_smp_dialog.cpp - SMP dialog box
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
#include "llcombobox.h"
#include "llimpanel.h"
#include "lluictrlfactory.h"
#include "llweb.h"

#include "otr_floater_smp_progress.h"
#include "otr_floater_smp_dialog.h"

std::string OtrFloaterSmpDialog::getStringName(const char *message_name)
{
    LLUIString message = getString(message_name);
    std::string them;
    if (!gCacheName->getFullName(mOtherParticipantUUID, them)) them = getString("otr_generic_name");
    message.setArg("[NAME]", them);
    return message.getString();
}

void OtrFloaterSmpDialog::init()
{
    LLUICtrlFactory::getInstance()->buildFloater(this, "otr_floater_smp_dialog.xml");
}

OtrFloaterSmpDialog::OtrFloaterSmpDialog(
    LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
    std::string my_fingerprint, std::string other_fingerprint)
    :   LLFloater(std::string("otr_floater_smp_dialog"), std::string("OtrFloaterSmpDialogRect"), LLStringUtil::null),
        mIMPanel(im_panel), mIsReply(false),
        mSessionUUID(session_id), mOtherParticipantUUID(other_id),
        mQuestion(""),
        mMyFingerprint(my_fingerprint), mOtherFingerprint(other_fingerprint)
{
    init();
}

OtrFloaterSmpDialog::OtrFloaterSmpDialog(
    LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
    std::string question, OtrlTLV *tlv)
    :   LLFloater(std::string("otr_floater_smp_dialog"), std::string("OtrFloaterSmpDialogRect"), LLStringUtil::null),
        mIMPanel(im_panel), mIsReply(true),
        mSessionUUID(session_id), mOtherParticipantUUID(other_id),
        mQuestion(question),
        mMyFingerprint(""), mOtherFingerprint("")
{
    init();
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    if (!combo)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP how combo" << llendl;
        return;
    }
    combo->removeall();
    combo->add(getString("otr_smp_how_by_qa_challenge"),  ADD_BOTTOM, TRUE);
    combo->selectFirstItem();
    doComboMenu();
}

OtrFloaterSmpDialog::OtrFloaterSmpDialog(
    LLFloaterIMPanel *im_panel, LLUUID session_id, LLUUID other_id,
    OtrlTLV *tlv)
    :   LLFloater(std::string("otr_floater_smp_dialog"), std::string("OtrFloaterSmpDialogRect"), LLStringUtil::null),
        mIMPanel(im_panel), mIsReply(true),
        mSessionUUID(session_id), mOtherParticipantUUID(other_id),
        mQuestion(""),
        mMyFingerprint(""), mOtherFingerprint("")
{
    init();
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    if (!combo)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP how combo" << llendl;
        return;
    }
    combo->removeall();
    combo->add(getString("otr_smp_how_by_ss_challenge"),  ADD_BOTTOM, TRUE);
    combo->selectFirstItem();
    doComboMenu();
}

OtrFloaterSmpDialog::~OtrFloaterSmpDialog()
{
}

BOOL OtrFloaterSmpDialog::postBuild()
{
    childSetAction("otr_smp_help_btn",   onClickHelp, this);
    childSetAction("otr_smp_cancel_btn", onClickCancel, this);
    childSetAction("otr_smp_auth_btn",   onClickAuth, this);
    LLComboBox *veri = getChild<LLComboBox>("otr_smp_veri_combo");
    veri->setCommitCallback(onClickVeri);
    veri->setCallbackUserData(this);
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    combo->setCommitCallback(onClickCombo);
    combo->setCallbackUserData(this);
    combo->removeall();
    combo->add(getString("otr_smp_how_by_qa"),  ADD_BOTTOM, TRUE);
    combo->add(getString("otr_smp_how_by_ss"),  ADD_BOTTOM, TRUE);
    combo->add(getString("otr_smp_how_by_mfv"), ADD_BOTTOM, TRUE);
    combo->selectFirstItem();
    doComboMenu();
    LLTextBox *why = getChild<LLTextBox>("otr_smp_why");
    if (why) why->setWrappedText(getString("otr_smp_why_text"));
    LLTextBox *how = getChild<LLTextBox>("otr_smp_how_text");
    if (mIsReply)
    {
        if (how) how->setWrappedText(getStringName("otr_smp_how_name_challenge_me"));
        setTitle(getStringName("otr_smp_name_auth_me"));
    }
    else
    {
        if (how) how->setWrappedText(getStringName("otr_smp_how_I_challenge_name"));
        setTitle(getStringName("otr_smp_I_auth_name"));
    }
    return TRUE;
}

void OtrFloaterSmpDialog::show()
{
    this->open(); /*Flawfinder: ignore*/
    this->setFocus(TRUE);
    this->center();
}

void OtrFloaterSmpDialog::doVeriMenu()
{
    LLComboBox *veri = getChild<LLComboBox>("otr_smp_veri_combo");
    if (!veri)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP verified combo" << llendl;
        return;
    }
    LLButton* auth_btn = getChild<LLButton>("otr_smp_auth_btn");
    if (!auth_btn)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP authenticate button" << llendl;
        return;
    }
    if (getString("otr_smp_veri_no") == veri->getSimple())
    {
        auth_btn->setEnabled(FALSE);
    }
    else
    {
        auth_btn->setEnabled(TRUE);
    }
}

void OtrFloaterSmpDialog::doComboMenu()
{
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    if (!combo)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP how combo" << llendl;
        return;
    }
    LLTextBox *howto = getChild<LLTextBox>("otr_smp_howto");
    if (!howto)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP howto control" << llendl;
        return;
    }
    LLTextBox *label1 = getChild<LLTextBox>("otr_smp_label1");
    if (!label1)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP label1 control" << llendl;
        return;
    }
    LLLineEditor *edit1 = getChild<LLLineEditor>("otr_smp_edit1");
    if (!edit1)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP edit1 control" << llendl;
        return;
    }
    LLTextBox *label2 = getChild<LLTextBox>("otr_smp_label2");
    if (!label2)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP label2 control" << llendl;
        return;
    }
    LLLineEditor *edit2 = getChild<LLLineEditor>("otr_smp_edit2");
    if (!edit2)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP edit2 control" << llendl;
        return;
    }
    LLComboBox *veri = getChild<LLComboBox>("otr_smp_veri_combo");
    if (!veri)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP verified combo" << llendl;
        return;
    }
    std::string choice = combo->getSimple();
    if (getString("otr_smp_how_by_qa") == choice)
    {
        howto->setWrappedText(getString("otr_smp_howto_qa"));
        label1->setWrappedText(getString("otr_smp_howto_qa_t1"));
        edit1->clear();
        edit1->setEnabled(TRUE);
        label2->setWrappedText(getString("otr_smp_howto_qa_t2"));
        edit2->clear();
        edit2->setEnabled(TRUE);
        veri->removeall();
        veri->setLabel(getString("otr_empty_string"));
    }
    else if (getString("otr_smp_how_by_qa_challenge") == choice)
    {
        howto->setWrappedText(getString("otr_smp_howto_qa"));
        label1->setWrappedText(getString("otr_smp_howto_qa_t1"));
        edit1->setText(mQuestion);
        edit1->setEnabled(FALSE);
        label2->setWrappedText(getString("otr_smp_howto_qa_t2"));
        edit2->clear();
        edit2->setEnabled(TRUE);
        veri->removeall();
        veri->setLabel(getString("otr_empty_string"));
    }
    else if (getString("otr_smp_how_by_ss") == choice)
    {
        howto->setWrappedText(getString("otr_smp_howto_ss"));
        label1->setWrappedText(getString("otr_smp_howto_ss_t1"));
        edit1->clear();
        edit1->setEnabled(TRUE);
        label2->setWrappedText(getString("otr_smp_howto_ss_t2"));
        edit2->clear();
        edit2->setEnabled(FALSE);
        veri->removeall();
        veri->setLabel(getString("otr_empty_string"));
    }
    else if (getString("otr_smp_how_by_ss_challenge") == choice)
    {
        howto->setWrappedText(getString("otr_smp_howto_ss"));
        label1->setWrappedText(getString("otr_smp_howto_ss_t1"));
        edit1->clear();
        edit1->setEnabled(TRUE);
        label2->setWrappedText(getString("otr_smp_howto_ss_t2"));
        edit2->clear();
        edit2->setEnabled(FALSE);
        veri->removeall();
        veri->setLabel(getString("otr_empty_string"));
    }
    else if (getString("otr_smp_how_by_mfv") == choice)
    {
        howto->setWrappedText(getString("otr_smp_howto_mfv"));
        label1->setWrappedText(getString("otr_smp_howto_mfv_t1"));
        edit1->setText(mMyFingerprint);
        edit1->setEnabled(FALSE);
        label2->setWrappedText(getString("otr_smp_howto_mfv_t2"));
        edit2->setText(mOtherFingerprint);
        edit2->setEnabled(FALSE);
        veri->add(getString("otr_smp_veri_no"),  ADD_BOTTOM, TRUE);
        veri->add(getString("otr_smp_veri_yes"), ADD_BOTTOM, TRUE);
        veri->selectFirstItem();
    }
    else
    {
        llwarns << "$PLOTR$ unknown smp howto choice" << llendl;
    }
    doVeriMenu();
}

// static
void OtrFloaterSmpDialog::onClickCombo(LLUICtrl* source, void* userdata)
{
    OtrFloaterSmpDialog* self = (OtrFloaterSmpDialog*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
    }
    else
    {
        self->doComboMenu();
    }
}

// static
void OtrFloaterSmpDialog::onClickVeri(LLUICtrl* source, void* userdata)
{
    OtrFloaterSmpDialog* self = (OtrFloaterSmpDialog*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
    }
    else
    {
        self->doVeriMenu();
    }
}

// static
void OtrFloaterSmpDialog::onClickHelp(void*)
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/help/3.2.0/authenticate.php");
}

// virtual
void OtrFloaterSmpDialog::onClose(bool app_quitting)
{
    destroy();
    if (!mIMPanel)
    {
        llwarns << "$PLOTR$ Can't find mIMPanel." << llendl;
    }
    else
    {
        mIMPanel->endSmpDialog();
    }
}

// static
void OtrFloaterSmpDialog::onClickCancel(void* userdata)
{
    OtrFloaterSmpDialog *self= (OtrFloaterSmpDialog*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
        return;
    }
    self->close();
}

void OtrFloaterSmpDialog::doClickAuth()
{
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    if (!combo)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP how combo" << llendl;
        return;
    }
    LLTextBox *howto = getChild<LLTextBox>("otr_smp_howto");
    if (!howto)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP howto control" << llendl;
        return;
    }
    LLTextBox *label1 = getChild<LLTextBox>("otr_smp_label1");
    if (!label1)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP label1 control" << llendl;
        return;
    }
    LLLineEditor *edit1 = getChild<LLLineEditor>("otr_smp_edit1");
    if (!edit1)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP edit1 control" << llendl;
        return;
    }
    LLTextBox *label2 = getChild<LLTextBox>("otr_smp_label2");
    if (!label2)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP label2 control" << llendl;
        return;
    }
    LLLineEditor *edit2 = getChild<LLLineEditor>("otr_smp_edit2");
    if (!edit2)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP edit2 control" << llendl;
        return;
    }
    LLComboBox *veri = getChild<LLComboBox>("otr_smp_veri_combo");
    if (!veri)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP verified combo" << llendl;
        return;
    }
    std::string choice = combo->getSimple();
    if (getString("otr_smp_how_by_qa") == choice)
    {
        llinfos << "$PLOTR$ OTR SMP QA method chosen" << llendl;
        std::string question = edit1->getText();
        std::string answer = edit2->getText();
        mIMPanel->startSmpProgress(mSessionUUID, mOtherParticipantUUID, question, answer);
    }
    else if (getString("otr_smp_how_by_ss") == choice)
    {
        llinfos << "$PLOTR$ OTR SMP SS method chosen" << llendl;
        std::string secret = edit1->getText();
        mIMPanel->startSmpProgress(mSessionUUID, mOtherParticipantUUID, secret);
    }
    else if (getString("otr_smp_how_by_mfv") == choice)
    {
        if (getString("otr_smp_veri_yes") == veri->getSimple())
        {
            llinfos << "$PLOTR$ OTR SMP MFV authenticating " << mOtherParticipantUUID << llendl;
            otr_authenticate_key(mSessionUUID, "mfv");
        }
    }
    else if (getString("otr_smp_how_by_qa_challenge") == choice)
    {
        std::string answer = edit2->getText();
        mIMPanel->startSmpProgress(mSessionUUID, mOtherParticipantUUID, mQuestion, answer, true);
    }
    else if (getString("otr_smp_how_by_ss_challenge") == choice)
    {
        std::string secret = edit1->getText();
        mIMPanel->startSmpProgress(mSessionUUID, mOtherParticipantUUID, secret, true);
    }
    else
    {
        llwarns << "$PLOTR$ unknown smp howto choice" << llendl;
    }
}

// static
void OtrFloaterSmpDialog::onClickAuth(void* userdata)
{
    OtrFloaterSmpDialog *self= (OtrFloaterSmpDialog*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
        return;
    }
    self->doClickAuth();
    self->close();
}

