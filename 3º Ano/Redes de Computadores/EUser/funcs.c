#include "funcs.h"
#include <sys/stat.h>

bool loggedIn = false;
User client;
char last_eid[4] = "";
//User command functions

//UDP Commands

int login(char *buffer) {
    char UID[7];
    char password[9];
    int response;

    if (loggedIn) {
        printf("Already Logged In!\n");
        return 1;
    }

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        printf("Invalid Input Format\n");
        return 1;
    }

    if (is_valid_uid(UID) == 1) {
        printf("Invalid UID\n");
        return 1;
    }

    if (is_valid_password(password) == 1) {
        printf("Invalid Password Format\n");
        return 1;
    }

    sprintf(buffer, "LIN %s %s\n", UID, password);
    response = udp_pedido(buffer);

    if (response == 0) {
        strcpy(client._UID, UID);
        strcpy(client._password, password);
        loggedIn = true;
    }

    return response;
}

int logout(char *buffer) {
    int response;

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    sprintf(buffer, "LOU %s %s\n", client._UID, client._password);
    response = udp_pedido(buffer);

    if (response == 0) {  // CORRIGIDO: era "response == 1"
        loggedIn = false;
        memset(client._UID, 0, sizeof(client._UID));
        memset(client._password, 0, sizeof(client._password));
    }

    return response;
}

int unregister(char *buffer) {
    int response;

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    sprintf(buffer, "UNR %s %s\n", client._UID, client._password);
    response = udp_pedido(buffer);

    if (response == 0) {  // CORRIGIDO: era "response == 1"
        loggedIn = false;
        memset(client._UID, 0, sizeof(client._UID));
        memset(client._password, 0, sizeof(client._password));
    }

    return response;
}

int myevents(char *buffer) {
    int response;

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    sprintf(buffer, "LME %s %s\n", client._UID, client._password);
    response = udp_pedido(buffer);

    return response;
}

int myreservations(char *buffer) {
    int response;

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    sprintf(buffer, "LMR %s %s\n", client._UID, client._password);
    response = udp_pedido(buffer);

    return response;
}


//TCP Commands
int create(char *buffer) {
    int response, num_attendees, fsize;
    char name[11], event_fname[25], event_date[17];

    if (!loggedIn) {
        printf("user not logged in\n");
        return 1;
    }

    // create name event_fname event_date num_attendees
    if (sscanf(buffer, "%*s %s %s %16[0-9- :] %d", name, event_fname, event_date, &num_attendees) != 4) {
        printf("Invalid input format\n");
        printf("Usage: create [name] [event_fname] [dd-mm-yyyy hh:mm] [num_attendees]\n");
        return 1;
    }

    // Validar nome (max 10 chars, alphanumeric)
    if (is_valid_name(name) == 1) {
        printf("Invalid Event Name\n");
        return 1;
    }

    // Validar num_attendees (10-999)
    if (is_valid_nseats(num_attendees) == 1) {
        printf("Invalid Number Of Attendees\n");
        return 1;
    }

    // Verificar se ficheiro existe e obter tamanho
    struct stat st;
    if (stat(event_fname, &st) != 0) {
        printf("File Not Found: %s\n", event_fname);
        return 1;
    }
    
    fsize = st.st_size;

    // Verificar tamanho máximo (10 MB)
    if (fsize > 10000000) {
        printf("File Too Large\n");
        return 1;
    }

    // Header termina com ESPAÇO (não \n!)
    // O \n será enviado DEPOIS do ficheiro no tcp_pedido
    sprintf(buffer, "CRE %s %s %s %s %d %s %d ", 
            client._UID, client._password, name, event_date, num_attendees, event_fname, fsize);
    
    response = tcp_pedido(buffer, event_fname, fsize);

    return response;
}

int close_event(char *buffer) {
    int response;
    char EID[4];

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    if (sscanf(buffer, "%*s %s", EID) != 1) {
        printf("Invalid Input Format\n");
        return 1;
    }

    if (is_valid_eid(EID) == 1) {
        printf("Invalid EID\n");
        return 1;
    }

    strcpy(last_eid, EID);

    sprintf(buffer, "CLS %s %s %s\n", client._UID, client._password, EID);
    response = tcp_pedido(buffer, NULL, 0);

    return response;
}

int reserve(char *buffer) {
    int response, num_seats;
    char EID[4];

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    if (sscanf(buffer, "%*s %s %d", EID, &num_seats) != 2) {
        printf("Invalid Input Format\n");
        return 1;
    }

    if (is_valid_eid(EID) == 1) {
        printf("Invalid EID\n");
        return 1;
    }

    if (num_seats < 1 || num_seats > 999) {
        printf("Invalid Number Of Seats\n");
        return 1;
    }

    strcpy(last_eid, EID);

    sprintf(buffer, "RID %s %s %s %d\n", client._UID, client._password, EID, num_seats);
    response = tcp_pedido(buffer, NULL, 0);

    return response;
}

int list(char *buffer) {
    int response;

    sprintf(buffer, "LST\n");
    response = tcp_pedido(buffer, NULL, 0);

    return response;
}

int show(char *buffer) {
    int response;
    char EID[4];

    if (sscanf(buffer, "%*s %s", EID) != 1) {
        printf("Invalid Input Format\n");
        return 1;
    }

    if (is_valid_eid(EID) == 1) {
        printf("Invalid EID\n");
        return 1;
    }

    sprintf(buffer, "SED %s\n", EID);
    response = tcp_pedido(buffer, NULL, 0);

    return response;
}

int changepass(char *buffer) {
    int response;
    char oldpass[9], newpass[9];

    if (!loggedIn) {
        printf("Not Logged In\n");
        return 1;
    }

    if (sscanf(buffer, "%*s %s %s", oldpass, newpass) != 2) {
        printf("Invalid Input Format\n");
        return 1;
    }

    if (is_valid_password(oldpass) == 1 || is_valid_password(newpass) == 1) {
        printf("Invalid Password\n");
        return 1;
    }

    sprintf(buffer, "CPS %s %s %s\n", client._UID, oldpass, newpass);
    response = tcp_pedido(buffer, NULL, 0);

    // Se sucesso, atualizar password local
    if (response == 0) {
        strcpy(client._password, newpass);
    }

    return response;
}

//Other Commands

int exit_(char *buffer) {
    if (loggedIn) {
        printf("Please Logout First!\n");
        return 1;
    }
    exit(EXIT_SUCCESS);
}

// Command table
CommandEntry commandTable[] = {
    // UDP commands
    {"login", login},
    {"logout", logout},
    {"unregister", unregister},
    {"myevents", myevents},
    {"mye", myevents},
    {"myreservations", myreservations},
    {"myr", myreservations},
    // TCP commands
    {"create", create},
    {"close", close_event},
    {"list", list},
    {"show", show},
    {"reserve", reserve},
    {"changePass", changepass},
    // Local commands
    {"exit", exit_},
    {NULL, NULL}
};

// Function to execute a command
void executeCommand(const char *command, char* buffer) {
    for (int i = 0; commandTable[i].command != NULL; i++) {
        if (strcmp(commandTable[i].command, command) == 0) {
            commandTable[i].function(buffer);
            return;
        }
    }
    printf("Invalid Command\n");
    return ;
}