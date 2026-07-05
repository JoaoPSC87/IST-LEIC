#ifndef _MENSAGENS_H_
#define _MENSAGENS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ResponseMessages {
    // Login (RLI)
    const char *(*RLI_OK)();
    const char *(*RLI_NOK)();
    const char *(*RLI_REG)();
    // Logout (RLO)
    const char *(*RLO_OK)();
    const char *(*RLO_NOK)();
    const char *(*RLO_UNR)();
    const char *(*RLO_WRP)();
    // Unregister (RUR)
    const char *(*RUR_OK)();
    const char *(*RUR_NOK)();
    const char *(*RUR_UNR)();
    const char *(*RUR_WRP)();
    // My Events (RME)
    const char *(*RME_OK)();
    const char *(*RME_NOK)();
    const char *(*RME_NLG)();
    const char *(*RME_WRP)();
    // My Reservations (RMR)
    const char *(*RMR_OK)();
    const char *(*RMR_NOK)();
    const char *(*RMR_NLG)();
    const char *(*RMR_WRP)();
    // Create Event (RCE)
    const char *(*RCE_OK)(char *EID);
    const char *(*RCE_NOK)();
    const char *(*RCE_NLG)();
    const char *(*RCE_WRP)();
    // Close Event (RCL)
    const char *(*RCL_OK)();
    const char *(*RCL_NOK)();
    const char *(*RCL_NLG)();
    const char *(*RCL_NOE)(char *EID);
    const char *(*RCL_EOW)(char *EID);
    const char *(*RCL_SLD)(char *EID);
    const char *(*RCL_PST)(char *EID);
    const char *(*RCL_CLO)(char *EID);
    // List Events (RLS)
    const char *(*RLS_OK)();
    const char *(*RLS_NOK)();
    // Show Event (RSE)
    const char *(*RSE_OK)(char *filename, int fsize);
    const char *(*RSE_NOK)();
    // Reserve (RRI)
    const char *(*RRI_ACC)(char *EID);
    const char *(*RRI_NOK)();
    const char *(*RRI_NLG)();
    const char *(*RRI_CLS)();
    const char *(*RRI_SLD)();
    const char *(*RRI_REJ)(int remaining);
    const char *(*RRI_PST)();
    const char *(*RRI_WRP)();
    // Change Password (RCP)
    const char *(*RCP_OK)();
    const char *(*RCP_NOK)();
    const char *(*RCP_NLG)();
    const char *(*RCP_NID)();
    // Error
    const char *(*ERR)();
};

extern const struct ResponseMessages Responses;

#endif