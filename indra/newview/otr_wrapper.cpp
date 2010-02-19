/* otr_wrapper.cpp - wrap libotr for use in the second life viewer
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
#include "llviewercontrol.h"
#include "llimpanel.h"
#include "otr_wrapper.h"
#include "llagent.h"
#include "llimview.h"


OTR_Wrapper *gOTR = 0;

OTR_Wrapper::OTR_Wrapper()
{
}

OTR_Wrapper::~OTR_Wrapper()
{
    if (gOTR)
    {
        otrl_userstate_free(gOTR->userstate);
    }
}

#define otrwui_tracing 1 // $TODO$ use some debug compile flag?
#if otrwui_tracing
extern "C" void otrwui_trace(const char *msg)
{
    llinfos << "$PLOTR$TRACE$" << msg << llendl;
}
#else
#define otrwui_trace(x) /* nothing */
#endif

static void otrwui_tbd(const char *msg)
{
    llinfos << "$PLOTR$TBD$" << msg << llendl;
}

static OtrlPolicy otrwui_policy(void *opdata, ConnContext *context)
{
    OtrlPolicy result = 0;
    U32 useotr = gSavedSettings.getU32("EmeraldUseOTR");
    if (3 < useotr)
    {
        llwarns << "$PLOTR$ Unknown setting for EmeraldUseOTR" << useotr << llendl;
        useotr = 2;
    }
    if (0 == useotr) // Require use of OTR in IMs
    {
        result = (0
//                  | OTRL_POLICY_ALLOW_V1            // don't even expose this?
                  | OTRL_POLICY_ALLOW_V2
                  | OTRL_POLICY_REQUIRE_ENCRYPTION
                  | OTRL_POLICY_SEND_WHITESPACE_TAG
                  | OTRL_POLICY_WHITESPACE_START_AKE
                  | OTRL_POLICY_ERROR_START_AKE
            );
    }
    else if (1 == useotr) // Request OTR if available
    {
        result = (0
//                  | OTRL_POLICY_ALLOW_V1            // don't even expose this?
                  | OTRL_POLICY_ALLOW_V2
//                  | OTRL_POLICY_REQUIRE_ENCRYPTION
                  | OTRL_POLICY_SEND_WHITESPACE_TAG
                  | OTRL_POLICY_WHITESPACE_START_AKE
                  | OTRL_POLICY_ERROR_START_AKE
            );
    }
    else if (2 == useotr) // Accept OTR requests
    {
        result = (0
//                  | OTRL_POLICY_ALLOW_V1            // don't even expose this?
                  | OTRL_POLICY_ALLOW_V2
//                  | OTRL_POLICY_REQUIRE_ENCRYPTION
//                  | OTRL_POLICY_SEND_WHITESPACE_TAG
                  | OTRL_POLICY_WHITESPACE_START_AKE
                  | OTRL_POLICY_ERROR_START_AKE
            );
    }
    else if (3 == useotr) // Decline use of OTR
    {
        result = (0
//                  | OTRL_POLICY_ALLOW_V1            // don't even expose this?
//                  | OTRL_POLICY_ALLOW_V2
//                  | OTRL_POLICY_REQUIRE_ENCRYPTION
//                  | OTRL_POLICY_SEND_WHITESPACE_TAG
//                  | OTRL_POLICY_WHITESPACE_START_AKE
//                  | OTRL_POLICY_ERROR_START_AKE
            );
    }
    return result;
}



static void otrwui_create_privkey(
    void *opdata, const char *accountname,
    const char *protocol)
{
    otrwui_trace("otrwui_create_privkey()");
    /* Create a private key for the given accountname/protocol if
     * desired. */
    if (gOTR)
    {
        LLUUID session_id = *((LLUUID*)opdata);
        otr_log_message_getstring(session_id, "otr_gen_key_please_wait");
        std::string path =
            gDirUtilp->getExpandedFilename(
                LL_PATH_PER_SL_ACCOUNT, OTR_PRIVATE_KEYS_FILE);
        otrl_privkey_generate(
            gOTR->get_userstate(), path.c_str(), accountname, protocol);
    }
}

static int otrwui_is_logged_in(
    void *opdata, const char *accountname,
    const char *protocol, const char *recipient)
{
    /* Report whether you think the given user is online.  Return 1 if
     * you think he is, 0 if you think he isn't, -1 if you're not sure.
     *
     * If you return 1, messages such as heartbeats or other
     * notifications may be sent to the user, which could result in "not
     * logged in" errors if you're wrong. */
    LLUUID recipient_uuid(recipient);
	const LLRelationship* info = NULL;
	info = LLAvatarTracker::instance().getBuddyInfo(recipient_uuid);
    int result;
    if (!info)                  result = 1; // hack, should be -1.  but we'll pretend non-friends are always online
    else if (!info->isOnline()) result = 0;
    else                        result = 1;
#if otrwui_tracing
    std::string msg = "otrwui_is_logged_in()";
    switch(result)
    {
    case -1: msg += " -1 unknown"; break;
    case  0: msg += " 0 no, logged out"; break;
    case  1: msg += " 1 yes, logged in"; break;
    default: msg += " ? imposible result"; break;
    }
    otrwui_trace(msg.c_str());
#endif
    return result;
}

static void otrwui_inject_message(
    void *opdata, const char *accountname,
    const char *protocol, const char *recipient, const char *message)
{
    otrwui_trace("otrwui_inject_message()");
    /* Send the given IM to the given recipient from the given
     * accountname/protocol. */
    if (!opdata)
    {
        // don't know how to deliver this with no opdata.
        llwarns << "$PLOTR$ otrwui_inject_message() called with NULL opdata; not delivering message" << llendl;
    }
    else
    {
        LLUUID sessionUUID = *((LLUUID*)opdata);
        LLUUID otherUUID(recipient);
        otr_deliver_message(message, sessionUUID, otherUUID, IM_NOTHING_SPECIAL);
    }
}

static void otrwui_notify(
    void *opdata, OtrlNotifyLevel level,
    const char *accountname, const char *protocol,
    const char *username, const char *title,
    const char *primary, const char *secondary)
{
    /* Display a notification message for a particular accountname /
     * protocol / username conversation. */
    std::string trace = "otrwui_notify: \n";
    trace += "title(";     trace += title;     trace += ")\n";
    trace += "primary(";   trace += primary;   trace += ")\n";
    trace += "secondary("; trace += secondary; trace += ")";
    otrwui_tbd(trace.c_str()); // $TODO$ write me
}

static int otrwui_display_otr_message(
    void *opdata, const char *accountname,
    const char *protocol, const char *username, const char *msg)
{
    /* Display an OTR control message for a particular accountname /
     * protocol / username conversation.  Return 0 if you are able to
     * successfully display it.  If you return non-0 (or if this
     * function is NULL), the control message will be displayed inline,
     * as a received message, or else by using the above notify()
     * callback. */
    otrwui_trace("otrwui_log_message()");
    LLUUID sessionUUID = *((LLUUID*)opdata);
    otr_log_message(sessionUUID, msg);
    return 0;
}

static void otrwui_update_context_list(
    void *opdata)
{
    /* When the list of ConnContexts changes (including a change in
     * state), this is called so the UI can be updated. */
    otrwui_tbd("otrwui_update_context_list"); // $TODO$ write me
}

static const char *otrwui_protocol_name(
    void *opdata, const char *protocol)
{
    /* Return a newly allocated string containing a human-friendly name
     * for the given protocol id */
    otrwui_trace("otrwui_protocol_name()");
    return "SecondLife";
}

static void otrwui_protocol_name_free(
    void *opdata, const char *protocol_name)
{
    /* Deallocate a string allocated by protocol_name */
    otrwui_trace("otrwui_protocol_name_free()");
    return; // no need to deallocate a const char *
}

static void otrwui_new_fingerprint(
    void *opdata, OtrlUserState us,
	    const char *accountname, const char *protocol,
	    const char *username, unsigned char fingerprint[20])
{
    /* A new fingerprint for the given user has been received. */
    std::string trace= "otrwui_new_fingerprint: ";
    trace += username; trace += "@"; trace += protocol;
    trace += " has a new fingerprint (";
    char fingerprint_copy[45];
    otrl_privkey_hash_to_human(fingerprint_copy, fingerprint);
    trace += fingerprint_copy; trace += ")";
    otrwui_tbd(trace.c_str()); // $TODO$ write me
}

static void otrwui_write_fingerprints(
    void *opdata)
{
    otrwui_trace("otrwui_write_fingerprints()");
    /* The list of known fingerprints has changed.  Write them to disk. */
    if (gOTR)
    {
        std::string path =
            gDirUtilp->getExpandedFilename(
                LL_PATH_PER_SL_ACCOUNT, OTR_PUBLIC_KEYS_FILE);
        otrl_privkey_write_fingerprints(gOTR->get_userstate(), path.c_str());
    }
}

static void otrwui_gone_secure(
    void *opdata, ConnContext *context)
{
    otrwui_trace("otrwui_gone_secure()");
    /* A ConnContext has entered a secure state. */
    if (! opdata)
    {
        otrwui_tbd("otrwui_gone_secure() called with null opdata");
        return;
    }
    LLUUID session_id = *((LLUUID*)opdata);
    if (context && context->active_fingerprint &&
        context->active_fingerprint->trust &&
        *(context->active_fingerprint->trust))
    {
        //otr_log_message_getstring_name(session_id, "otr_log_start_private");
    }
    else
    {
        //otr_log_message_getstring_name(session_id, "otr_log_start_unverified");
    }
    otr_show_status(session_id);
}

static void otrwui_gone_insecure(
    void *opdata, ConnContext *context)
{
    otrwui_trace("otrwui_gone_insecure()");
    /* A ConnContext has left a secure state. */
    if (! opdata)
    {
        otrwui_tbd("otrwui_gone_insecure() called with null opdata");
        return;
    }
    LLUUID session_id = *((LLUUID*)opdata);
    otr_log_message_getstring(session_id, "otr_log_gone_insecure");
    otr_show_status(session_id);
}

static void otrwui_still_secure(
    void *opdata, ConnContext *context, int is_reply)
{
    otrwui_trace("otrwui_still_secure()");
    /* We have completed an authentication, using the D-H keys we
     * already knew.  is_reply indicates whether we initiated the AKE. */
    if (! opdata)
    {
        otrwui_tbd("otrwui_still_secure() called with null opdata");
        return;
    }
    LLUUID session_id = *((LLUUID*)opdata);
    if (context && context->active_fingerprint &&
        context->active_fingerprint->trust &&
        *(context->active_fingerprint->trust))
    {
        //otr_log_message_getstring_name(session_id, "otr_log_still_private");
    }
    else
    {
        //otr_log_message_getstring_name(session_id, "otr_log_still_unverified");
    }
    otr_show_status(session_id);
}

static void otrwui_log_message(
    void *opdata, const char *message)
{
    /* Log a message.  The passed message will end in "\n". */
    otrwui_trace("otrwui_log_message()");
    llinfos << message << llendl;
}

static int otrwui_max_message_size(
    void *opdata, ConnContext *context)
{
    /* Find the maximum message size supported by this protocol. */
    return (MAX_MSG_BUF_SIZE - 24);
}

static const char *otrwui_account_name(
    void *opdata, const char *account,
    const char *protocol)
{
    /* Return a newly allocated string containing a human-friendly
     * representation for the given account */
    otrwui_trace("otrwui_account_name");
    std::string name;
    LLUUID uuid = LLUUID(account);
    if (!gCacheName->getFullName(uuid, name)) name = account;
    char *result = (char *)malloc(name.length()+1);
    strncpy(result, name.c_str(), name.length()+1);
    return result;
}

static void otrwui_account_name_free(
    void *opdata, const char *account_name)
{
    /* Deallocate a string returned by account_name */
    otrwui_trace("otrwui_account_name_free");
    free((void *)account_name);
}

// static
void OTR_Wrapper::stopAll()
{
    otrwui_trace("OTR_Wrapper::stopAll()");
    if (!gOTR) return;
    if (gOTR->userstate)
    {
        for (ConnContext *context = gOTR->userstate->context_root; context; context = context->next)
        {
            if (context && (OTRL_MSGSTATE_ENCRYPTED == context->msgstate))
            {
                LLUUID their_uuid = LLUUID(context->username);
                LLUUID session_uuid = LLIMMgr::computeSessionID(IM_NOTHING_SPECIAL, their_uuid);
                LLFloaterIMPanel* pan = gIMMgr->findFloaterBySession(session_uuid);
                if (pan)
                {
                    llinfos << "$PLOTR$ found IM pannel, pan->doOtrStop()" << llendl;
                    pan->doOtrStop();
                }
                else
                {
                    char my_uuid[UUID_STR_SIZE];
                    gAgent.getID().toString(&(my_uuid[0]));
                    llinfos << "$PLOTR$ didn't find IM panel, going lower level"
                            << " c->accountname:" << context->accountname // this avatar, aka me
                            << " c->protocol:"    << context->protocol    // secondlife, IRC, yahoo...
                            << " c->username:"    << context->username    // other participant
                            << " session_uuid:"   << session_uuid
                            << llendl;
                    otrl_message_disconnect(
                        gOTR->get_userstate(), 
                        gOTR->get_uistate(), 
                        &session_uuid,
                        context->accountname,
                        context->protocol,
                        context->username);
                }
            }
        }
    }
}

// static
void OTR_Wrapper::logout()
{
    otrwui_trace("OTR_Wrapper::logout()");
    if (!gOTR) return;
    OTR_Wrapper::stopAll();
    delete gOTR;
    gOTR = NULL;
}

// static
void OTR_Wrapper::init()
{
    otrwui_trace("OTR_Wrapper::init()");
    if (! gOTR)
    {
        gOTR = new OTR_Wrapper;
        if (!gOTR)
        {
            llwarns << "$PLOTR$ Can't new otr_wrapper." << llendl;
        }
        else
        {
            llinfos << "$PLOTR$ newed otr_wrapper." << llendl;
            gOTR->uistate.policy = &otrwui_policy;
            gOTR->uistate.create_privkey = &otrwui_create_privkey;
            gOTR->uistate.is_logged_in = &otrwui_is_logged_in;
            gOTR->uistate.inject_message = &otrwui_inject_message;
            gOTR->uistate.notify = &otrwui_notify;
            gOTR->uistate.display_otr_message = &otrwui_display_otr_message;
            gOTR->uistate.update_context_list = &otrwui_update_context_list;
            gOTR->uistate.protocol_name = &otrwui_protocol_name;
            gOTR->uistate.protocol_name_free = &otrwui_protocol_name_free;
            gOTR->uistate.new_fingerprint = &otrwui_new_fingerprint;
            gOTR->uistate.write_fingerprints = &otrwui_write_fingerprints;
            gOTR->uistate.gone_secure = &otrwui_gone_secure;
            gOTR->uistate.gone_insecure = &otrwui_gone_insecure;
            gOTR->uistate.still_secure = &otrwui_still_secure;
            gOTR->uistate.log_message = &otrwui_log_message;
            gOTR->uistate.max_message_size = &otrwui_max_message_size;
            gOTR->uistate.account_name = &otrwui_account_name;
            gOTR->uistate.account_name_free = &otrwui_account_name_free;
            OTRL_INIT;
            gOTR->userstate = otrl_userstate_create();
            std::string privpath =
                gDirUtilp->getExpandedFilename(
                    LL_PATH_PER_SL_ACCOUNT, OTR_PRIVATE_KEYS_FILE);
            otrl_privkey_read(gOTR->userstate,              privpath.c_str());
            std::string pubpath =
                gDirUtilp->getExpandedFilename(
                    LL_PATH_PER_SL_ACCOUNT, OTR_PUBLIC_KEYS_FILE);
            otrl_privkey_read_fingerprints(gOTR->userstate, pubpath.c_str(), NULL, NULL);

            char *rand_bytes[4]; // move the freeze on first OTR message to startup time.  It still 
            gcry_randomize(rand_bytes, 4, GCRY_STRONG_RANDOM); // freezes, but more predictably this way.

#if 0 // this will gen a key, if the user doesn't have one, at init() time
            if (gOTR && gOTR->userstate)
            {
                OtrlPrivKey *r = gOTR->userstate->privkey_root;
                OtrlPrivKey *k = gOTR->userstate->privkey_root;
                while (k && (k != r))
                {
                    if (0 == strcmp(gOTR->get_protocolid(), k->protocol))
                    {
                        return;
                    }
                }
                char my_uuid[UUID_STR_SIZE];
                gAgent.getID().toString(&(my_uuid[0]));
                otrl_privkey_generate(gOTR->userstate, privpath.c_str(),
                                      my_uuid, gOTR->get_protocolid());
            }
#endif
        }
    }
}
