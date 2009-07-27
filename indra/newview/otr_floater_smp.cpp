/* otr_floater_smp.cpp - SMP dialog box
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

#include "llviewerprecompiledheaders.h"

#include "otr_floater_smp.h"

#include "llcachename.h"
#include "llcombobox.h"
#include "llimpanel.h"
#include "lluictrlfactory.h"
#include "llweb.h"

OtrFloaterSmp::OtrFloaterSmp(
    LLUUID session_id, LLUUID other_id,
    std::string my_fingerprint, std::string other_fingerprint)
    :	LLFloater(std::string("otr_floater_smp"), std::string("OtrFloaterSmpRect"), LLStringUtil::null),
        mSessionUUID(session_id), mOtherParticipantID(other_id),
        mMyFingerprint(my_fingerprint), mOtherFingerprint(other_fingerprint)
{
    LLUICtrlFactory::getInstance()->buildFloater(this, "otr_floater_smp.xml");
    childSetAction("otr_smp_help_btn",   onClickHelp, this);
    childSetAction("otr_smp_cancel_btn", onClickCancel, this);
    childSetAction("otr_smp_auth_btn",   onClickAuth, this);
 	setDefaultBtn("otr_smp_cancel_btn");
}

OtrFloaterSmp::~OtrFloaterSmp()
{
}

BOOL OtrFloaterSmp::postBuild()
{
    LLComboBox *combo = getChild<LLComboBox>("otr_smp_how_combo");
    if (!combo)
    {
        llwarns << "$PLOTR$ Can't find OTR SMP how combo" << llendl;
        return FALSE;
    }
    else
    {
        combo->setCommitCallback(onClickCombo);
        combo->setCallbackUserData(this);
        combo->removeall();
        combo->add(getString("otr_smp_how_by_mfv"), ADD_BOTTOM, TRUE);
        combo->add(getString("otr_smp_how_by_qa"),  ADD_BOTTOM, FALSE);
        combo->add(getString("otr_smp_how_by_ss"),  ADD_BOTTOM, FALSE);
        combo->selectFirstItem();
        doComboMenu();
    }
    LLTextBox *why = getChild<LLTextBox>("otr_smp_why");
    if (why) why->setWrappedText(getString("otr_smp_why_text"));
    return TRUE;
}

void OtrFloaterSmp::show()
{
 	this->open(); /*Flawfinder: ignore*/
 	this->setFocus(TRUE);
    this->center();
}

void OtrFloaterSmp::doComboMenu()
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
    veri->removeall();
    veri->setLabel(getString("otr_empty_string"));
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
}

// static
void OtrFloaterSmp::onClickCombo(LLUICtrl* source, void* userdata)
{
	OtrFloaterSmp* self = (OtrFloaterSmp*)userdata;
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
void OtrFloaterSmp::onClickHelp(void*)
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/help/3.2.0/authenticate.php");
}

// static
void OtrFloaterSmp::onClickCancel(void* userdata)
{
    OtrFloaterSmp *self= (OtrFloaterSmp*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
        return;
    }
    self->close();
    delete self;
}

void OtrFloaterSmp::doClickAuth()
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
        llwarns << "$PLOTR$ OTR SMP QA method not implemented" << llendl;
    }
    else if (getString("otr_smp_how_by_ss") == choice)
    {
        llwarns << "$PLOTR$ OTR SMP SS method not implemented" << llendl;
    }
    else if (getString("otr_smp_how_by_mfv") == choice)
    {
        if (getString("otr_smp_veri_yes") == veri->getSimple())
        {
            llinfos << "$PLOTR$ OTR SMP MFV authenticating " << mOtherParticipantID << llendl;
            otr_authenticate_key(mSessionUUID, "mfv");
        }
    }
    else
    {
        llwarns << "$PLOTR$ unknown smp howto choice" << llendl;
    }
}

// static
void OtrFloaterSmp::onClickAuth(void* userdata)
{
    OtrFloaterSmp *self= (OtrFloaterSmp*)userdata;
    if (!self)
    {
        llwarns << "$PLOTR$ can't find OTR SMP dialog." << llendl;
        return;
    }
    self->doClickAuth();
    self->close();
    delete self;
}

