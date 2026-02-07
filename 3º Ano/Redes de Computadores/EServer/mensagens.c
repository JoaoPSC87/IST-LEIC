#include "mensagens.h"

// ==================== UDP MESSAGES ====================

// Login (RLI)
static const char *LIN_OK() {
    return "RLI OK\n";
}
static const char *LIN_NOK() {
    return "RLI NOK\n";
}
static const char *LIN_REG() {
    return "RLI REG\n";
}
static const char *LIN_ERR() {
    return "RLI ERR\n";
}

// Logout (RLO)
static const char *LOU_OK() {
    return "RLO OK\n";
}
static const char *LOU_NOK() {
    return "RLO NOK\n";
}
static const char *LOU_UNR() {
    return "RLO UNR\n";
}
static const char *LOU_WRP() {
    return "RLO WRP\n";
}
static const char *LOU_ERR() {
    return "RLO ERR\n";
}

// Unregister (RUR)
static const char *UNR_OK() {
    return "RUR OK\n";
}
static const char *UNR_NOK() {
    return "RUR NOK\n";
}
static const char *UNR_UNR() {
    return "RUR UNR\n";
}
static const char *UNR_WRP() {
    return "RUR WRP\n";
}
static const char *UNR_ERR() {
    return "RUR ERR\n";
}

// My Events (RME)
static const char *LME_NOK() {
    return "RME NOK\n";
}
static const char *LME_NLG() {
    return "RME NLG\n";
}
static const char *LME_WRP() {
    return "RME WRP\n";
}
static const char *LME_ERR() {
    return "RME ERR\n";
}
static const char *LME_OK(char *events_list) {
    static char result[MAX_BUFFER_SIZE];  
    snprintf(result, sizeof(result), "RME OK %s\n", events_list);
    return result;
}


// My Reservations (RMR)
static const char *LMR_NOK() {
    return "RMR NOK\n";
}
static const char *LMR_NLG() {
    return "RMR NLG\n";
}
static const char *LMR_WRP() {
    return "RMR WRP\n";
}
static const char *LMR_ERR() {
    return "RMR ERR\n";
}
static const char *LMR_OK(char *reservations_list) {
    static char result[MAX_BUFFER_SIZE];  
    snprintf(result, sizeof(result), "RMR OK %s\n", reservations_list);
    return result;
}

// ==================== TCP MESSAGES ====================

// Create Event (RCE)
static const char *CRE_OK(char *EID) {
    static char result[50];  // Buffer estático
    snprintf(result, sizeof(result), "RCE OK %s\n", EID);
    return result;
}
static const char *CRE_NOK() {
    return "RCE NOK\n";
}
static const char *CRE_NLG() {
    return "RCE NLG\n";
}
static const char *CRE_WRP() {
    return "RCE WRP\n";
}
static const char *CRE_ERR() {
    return "RCE ERR\n";
}

// Close Event (RCL)
static const char *CLS_OK() {
    return "RCL OK\n";
}
static const char *CLS_NOK() {
    return "RCL NOK\n";
}
static const char *CLS_NLG() {
    return "RCL NLG\n";
}
static const char *CLS_NOE(char *EID) {
    static char result[50];
    snprintf(result, sizeof(result), "RCL NOE\n");
    return result;
}

static const char *CLS_EOW(char *EID) {
    static char result[50];
    snprintf(result, sizeof(result), "RCL EOW\n");
    return result;
}
static const char *CLS_SLD(char *EID) {
    static char result[50];
    snprintf(result, sizeof(result), "RCL SLD\n");
    return result;
}
static const char *CLS_PST(char *EID) {
    static char result[50];
    snprintf(result, sizeof(result), "RCL PST\n");
    return result;
}
static const char *CLS_CLO(char *EID) {
    static char result[50];
    snprintf(result, sizeof(result), "RCL CLO\n");
    return result;
}
static const char *CLS_ERR() {
    return "RCL ERR\n";
}

// List Events (RLS)
static const char *LST_NOK() {
    return "RLS NOK\n";
}
static const char *LST_OK(char *events_list) {
    static char result[MAX_BUFFER_SIZE];  // Buffer estático
    snprintf(result, sizeof(result), "RLS OK %s\n", events_list);
    return result;
}
static const char *LST_ERR() {
    return "RLS ERR\n";
}

// Show Event (RSE)
static const char *SED_NOK() {
    return "RSE NOK\n";
}
static const char *SED_OK(char *event_details) {
    static char result[MAX_BUFFER_SIZE];  // Buffer estático
    snprintf(result, sizeof(result), "RSE OK %s \n", event_details);
    return result;
}
static const char *SED_ERR() {
    return "RSE ERR\n";
}

// Reserve (RRI)
static const char *RID_ACC() {
    return "RRI ACC\n";
}
static const char *RID_NOK() {
    return "RRI NOK\n";
}
static const char *RID_NLG() {
    return "RRI NLG\n";
}
static const char *RID_CLS() {
    return "RRI CLS\n";
}
static const char *RID_SLD() {
    return "RRI SLD\n";
}
static const char *RID_REJ(int remaining) {
    static char result[50];  // Buffer estático
    snprintf(result, sizeof(result), "RRI REJ %d\n", remaining);
    return result;
}
static const char *RID_PST() {
    return "RRI PST\n";
}
static const char *RID_WRP() {
    return "RRI WRP\n";
}
static const char *RID_ERR() {
    return "RRI ERR\n";
}

// Change Password (RCP)
static const char *CPS_OK() {
    return "RCP OK\n";
}
static const char *CPS_NOK() {
    return "RCP NOK\n";
}
static const char *CPS_NLG() {
    return "RCP NLG\n";
}
static const char *CPS_NID() {
    return "RCP NID\n";
}
static const char *CPS_ERR() {
    return "RCP ERR\n";
}

// ==================== STRUCTURE INITIALIZATION ====================

const struct ServerMessages Messages = {
    // Login
    .LIN_OK = LIN_OK,
    .LIN_NOK = LIN_NOK,
    .LIN_REG = LIN_REG,
    .LIN_ERR = LIN_ERR,
    // Logout
    .LOU_OK = LOU_OK,
    .LOU_NOK = LOU_NOK,
    .LOU_UNR = LOU_UNR,
    .LOU_WRP = LOU_WRP,
    .LOU_ERR = LOU_ERR,
    // Unregister
    .UNR_OK = UNR_OK,
    .UNR_NOK = UNR_NOK,
    .UNR_UNR = UNR_UNR,
    .UNR_WRP = UNR_WRP,
    .UNR_ERR = UNR_ERR,
    // My Events
    .LME_NOK = LME_NOK,
    .LME_NLG = LME_NLG,
    .LME_WRP = LME_WRP,
    .LME_ERR = LME_ERR,
    .LME_OK = LME_OK,
    // My Reservations
    .LMR_NOK = LMR_NOK,
    .LMR_NLG = LMR_NLG,
    .LMR_WRP = LMR_WRP,
    .LMR_ERR = LMR_ERR,
    .LMR_OK = LMR_OK,
    // Create Event
    .CRE_OK = CRE_OK,
    .CRE_NOK = CRE_NOK,
    .CRE_NLG = CRE_NLG,
    .CRE_WRP = CRE_WRP,
    .CRE_ERR = CRE_ERR,
    // Close Event
    .CLS_OK = CLS_OK,
    .CLS_NOK = CLS_NOK,
    .CLS_NLG = CLS_NLG,
    .CLS_NOE = CLS_NOE,
    .CLS_EOW = CLS_EOW,
    .CLS_SLD = CLS_SLD,
    .CLS_PST = CLS_PST,
    .CLS_CLO = CLS_CLO,
    .CLS_ERR = CLS_ERR,
    // List Events
    .LST_NOK = LST_NOK,
    .LST_OK = LST_OK,
    .LST_ERR = LST_ERR,
    // Show Event
    .SED_NOK = SED_NOK,
    .SED_OK = SED_OK,
    .SED_ERR = SED_ERR,
    // Reserve
    .RID_ACC = RID_ACC,
    .RID_NOK = RID_NOK,
    .RID_NLG = RID_NLG,
    .RID_CLS = RID_CLS,
    .RID_SLD = RID_SLD,
    .RID_REJ = RID_REJ,
    .RID_PST = RID_PST,
    .RID_WRP = RID_WRP,
    .RID_ERR = RID_ERR,
    // Change Password
    .CPS_OK = CPS_OK,
    .CPS_NOK = CPS_NOK,
    .CPS_NLG = CPS_NLG,
    .CPS_NID = CPS_NID,
    .CPS_ERR = CPS_ERR
};