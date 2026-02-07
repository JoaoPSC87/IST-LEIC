#ifndef _MENSAGENS_
#define _MENSAGENS_

#include "../func_aux/validations.h"

// Define a structure to hold function pointers for server messages
struct ServerMessages {
    // Login (RLI)
    const char *(*LIN_OK)();
    const char *(*LIN_NOK)();
    const char *(*LIN_REG)();
    const char *(*LIN_ERR)();
    
    // Logout (RLO)
    const char *(*LOU_OK)();
    const char *(*LOU_NOK)();
    const char *(*LOU_UNR)();
    const char *(*LOU_WRP)();
    const char *(*LOU_ERR)();
    
    // Unregister (RUR)
    const char *(*UNR_OK)();
    const char *(*UNR_NOK)();
    const char *(*UNR_UNR)();
    const char *(*UNR_WRP)();
    const char *(*UNR_ERR)();
    
    // My Events (RME)
    const char *(*LME_NOK)();
    const char *(*LME_NLG)();
    const char *(*LME_WRP)();
    const char *(*LME_ERR)();
    const char *(*LME_OK)(char *events_list);
    
    // My Reservations (RMR)
    const char *(*LMR_NOK)();
    const char *(*LMR_NLG)();
    const char *(*LMR_WRP)();
    const char *(*LMR_ERR)();
    const char *(*LMR_OK)(char *reservations_list);
    
    // Create Event (RCE)
    const char *(*CRE_OK)(char *EID);
    const char *(*CRE_NOK)();
    const char *(*CRE_NLG)();
    const char *(*CRE_WRP)();
    const char *(*CRE_ERR)();
    
    // Close Event (RCL)
    const char *(*CLS_OK)();
    const char *(*CLS_NOK)();
    const char *(*CLS_NLG)();
    const char *(*CLS_NOE)(char *EID);
    const char *(*CLS_EOW)(char *EID);
    const char *(*CLS_SLD)(char *EID);
    const char *(*CLS_PST)(char *EID);
    const char *(*CLS_CLO)(char *EID);
    const char *(*CLS_ERR)();
    
    // List Events (RLS)
    const char *(*LST_NOK)();
    const char *(*LST_OK)(char *events_list);
    const char *(*LST_ERR)();
    
    // Show Event (RSE)
    const char *(*SED_NOK)();
    const char *(*SED_OK)(char *event_details);
    const char *(*SED_ERR)();
    
    // Reserve (RRI)
    const char *(*RID_ACC)();
    const char *(*RID_NOK)();
    const char *(*RID_NLG)();
    const char *(*RID_CLS)();
    const char *(*RID_SLD)();
    const char *(*RID_REJ)(int remaining);
    const char *(*RID_PST)();
    const char *(*RID_WRP)();
    const char *(*RID_ERR)();
    
    // Change Password (RCP)
    const char *(*CPS_OK)();
    const char *(*CPS_NOK)();
    const char *(*CPS_NLG)();
    const char *(*CPS_NID)();
    const char *(*CPS_ERR)();
};

extern const struct ServerMessages Messages;

#endif