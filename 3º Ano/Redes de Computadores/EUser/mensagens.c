#include "mensagens.h"

// ----------------- UDP messages --------------------

// Login responses (RLI)
static const char *RLI_OK() {
    return "Login Successful\n";
}
static const char *RLI_NOK() {
    return "Incorrect Login Attempt\n";
}
static const char *RLI_REG() {
    return "Registration Successful\n";
}

// Logout responses (RLO)
static const char *RLO_OK() {
    return "Logout Successful\n";
}
static const char *RLO_NOK() {
    return "User Not Logged In\n";
}
static const char *RLO_UNR() {
    return "Unknown User\n";
}
static const char *RLO_WRP() {
    return "Incorrect Password\n";
}

// Unregister responses (RUR)
static const char *RUR_OK() {
    return "Unregister Successful\n";
}
static const char *RUR_NOK() {
    return "User Not Logged In\n";
}
static const char *RUR_UNR() {
    return "Unknown User\n";
}
static const char *RUR_WRP() {
    return "Incorrect Password\n";
}

// My Events responses (RME)
static const char *RME_OK() {
    return ""; // lista será tratada à parte
}
static const char *RME_NOK() {
    return "No Events Associated With User\n";
}
static const char *RME_NLG() {
    return "User Not Logged In\n";
}
static const char *RME_WRP() {
    return "Incorrect Password\n";
}

// My Reservations responses (RMR)
static const char *RMR_OK() {
    return ""; // lista será tratada à parte
}
static const char *RMR_NOK() {
    return "No Reservations Associated With User\n";
}
static const char *RMR_NLG() {
    return "User Not Logged In\n";
}
static const char *RMR_WRP() {
    return "Incorrect Password\n";
}

// ----------------- TCP messages --------------------

// Create Event responses (RCE)
static const char *RCE_OK(char *EID) {
    static char result[100];  // Buffer estático
    snprintf(result, sizeof(result), "Event %s Successfully Created\n", EID);
    return result;
}
static const char *RCE_NOK() {
    return "Unable To Create Event\n";
}
static const char *RCE_NLG() {
    return "User Not Logged In\n";
}
static const char *RCE_WRP() {
    return "Incorrect Password\n";
}

// Close Event responses (RCL)
static const char *RCL_OK() {
    return "Event Closed Successfully\n";
}
static const char *RCL_NOK() {
    return "Unknown User or Incorrect Password\n";
}
static const char *RCL_NLG() {
    return "User Not Logged In\n";
}
static const char *RCL_NOE(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Event %s Does Not Exist\n", EID);
    return result;
}

static const char *RCL_EOW(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Event %s Not Owned By User\n", EID);
    return result;
}
static const char *RCL_SLD(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Event %s Sold Out\n", EID);
    return result;
}

static const char *RCL_PST(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Event %s Date Has Passed\n", EID);
    return result;
}
static const char *RCL_CLO(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Event %s Was Already Closed\n", EID);
    return result;
}

// List Events responses (RLS)
static const char *RLS_OK() {
    return ""; // lista será tratada à parte
}
static const char *RLS_NOK() {
    return "No Events Created yet\n";
}

// Show Event responses (RSE)
static const char *RSE_OK(char *filename, int fsize) {
    static char result[150];
    snprintf(result, sizeof(result), "Filename: %s Size: %d Bytes\n", filename, fsize);
    return result;
}
static const char *RSE_NOK() {
    return "Event Does Not Exist or No File Available\n";
}

// Reserve responses (RRI)
static const char *RRI_ACC(char *EID) {
    static char result[100];
    snprintf(result, sizeof(result), "Reservation For Event %s Accepted\n", EID);
    return result;
}
static const char *RRI_NOK() {
    return "Event Is Not Active\n";
}
static const char *RRI_NLG() {
    return "User Not Logged In\n";
}
static const char *RRI_CLS() {
    return "Event Is Closed\n";
}
static const char *RRI_SLD() {
    return "Event Is Sold Out\n";
}

static const char *RRI_REJ(int remaining) {
    static char result[100];
    snprintf(result, sizeof(result), "Reservation Unsuccessful - Only %d Seats Remaining\n", remaining);
    return result;
}
static const char *RRI_PST() {
    return "Event Date Has Passed\n";
}
static const char *RRI_WRP() {
    return "Incorrect Password\n";
}

// Change Password responses (RCP)
static const char *RCP_OK() {
    return "Password Successfully Changed\n";
}
static const char *RCP_NOK() {
    return "Incorrect Password\n";
}
static const char *RCP_NLG() {
    return "User Not Logged In\n";
}
static const char *RCP_NID() {
    return "Uknown User\n";
}

// Generic error
static const char *ERR() {
    return "Protocol Error\n";
}

// ---------------------------------------------------

// Create an instance of the structure
const struct ResponseMessages Responses = {
    // Login
    .RLI_OK = RLI_OK,
    .RLI_NOK = RLI_NOK,
    .RLI_REG = RLI_REG,
    // Logout
    .RLO_OK = RLO_OK,
    .RLO_NOK = RLO_NOK,
    .RLO_UNR = RLO_UNR,
    .RLO_WRP = RLO_WRP,
    // Unregister
    .RUR_OK = RUR_OK,
    .RUR_NOK = RUR_NOK,
    .RUR_UNR = RUR_UNR,
    .RUR_WRP = RUR_WRP,
    // My Events
    .RME_OK = RME_OK,
    .RME_NOK = RME_NOK,
    .RME_NLG = RME_NLG,
    .RME_WRP = RME_WRP,
    // My Reservations
    .RMR_OK = RMR_OK,
    .RMR_NOK = RMR_NOK,
    .RMR_NLG = RMR_NLG,
    .RMR_WRP = RMR_WRP,
    // Create Event
    .RCE_OK = RCE_OK,
    .RCE_NOK = RCE_NOK,
    .RCE_NLG = RCE_NLG,
    .RCE_WRP = RCE_WRP,
    // Close Event
    .RCL_OK = RCL_OK,
    .RCL_NOK = RCL_NOK,
    .RCL_NLG = RCL_NLG,
    .RCL_NOE = RCL_NOE,
    .RCL_EOW = RCL_EOW,
    .RCL_SLD = RCL_SLD,
    .RCL_PST = RCL_PST,
    .RCL_CLO = RCL_CLO,
    // List Events
    .RLS_OK = RLS_OK,
    .RLS_NOK = RLS_NOK,
    // Show Event
    .RSE_OK = RSE_OK,
    .RSE_NOK = RSE_NOK,
    // Reserve
    .RRI_ACC = RRI_ACC,
    .RRI_NOK = RRI_NOK,
    .RRI_NLG = RRI_NLG,
    .RRI_CLS = RRI_CLS,
    .RRI_SLD = RRI_SLD,
    .RRI_REJ = RRI_REJ,
    .RRI_PST = RRI_PST,
    .RRI_WRP = RRI_WRP,
    // Change Password
    .RCP_OK = RCP_OK,
    .RCP_NOK = RCP_NOK,
    .RCP_NLG = RCP_NLG,
    .RCP_NID = RCP_NID,
    // Error
    .ERR = ERR
};