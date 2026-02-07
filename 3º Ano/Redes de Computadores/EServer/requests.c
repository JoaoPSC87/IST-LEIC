#include "requests.h"

// Helper function to send UDP replies
void send_udp_reply(int fd, struct sockaddr_in addr, const char *reply) {
    if (sendto(fd, reply, strlen(reply), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Error sending UDP reply");
    }
}

// Helper function to send TCP replies
void send_tcp_reply(int fd, const char *reply) {
    if (write(fd, reply, strlen(reply)) == -1) {
        perror("Error sending TCP reply");
    }
}

// ==================== UDP HANDLERS ====================

int login_handler(int fd, struct sockaddr_in addr, char *buffer) {
    char UID[7];
    char password[9];

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        send_udp_reply(fd, addr, Messages.LIN_ERR());
        return 1;
    }
    
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1) {
        send_udp_reply(fd, addr, Messages.LIN_ERR());
        return 1;
    }

    if (!user_is_registered(UID)) {
        register_user(UID, password);
        send_udp_reply(fd, addr, Messages.LIN_REG());
        log_user_in(UID);
    } else {
        if (!check_user_pass(UID, password)) {
            send_udp_reply(fd, addr, Messages.LIN_NOK());
        } else {
            log_user_in(UID);
            send_udp_reply(fd, addr, Messages.LIN_OK());
        }
    }

    return 0;
}

int logout_handler(int fd, struct sockaddr_in addr, char *buffer) {
    char UID[7];
    char password[9];

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        send_udp_reply(fd, addr, Messages.LOU_ERR());
        return 1;
    }
    
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1) {
        send_udp_reply(fd, addr, Messages.LOU_ERR());
        return 1;
    }

    if (!user_is_registered(UID)) {
        send_udp_reply(fd, addr, Messages.LOU_UNR());
    } else if (!check_user_pass(UID, password)) {
        send_udp_reply(fd, addr, Messages.LOU_WRP());
    } else if (!user_is_logged(UID)) {
        send_udp_reply(fd, addr, Messages.LOU_NOK());
    } else {
        log_user_out(UID);
        send_udp_reply(fd, addr, Messages.LOU_OK());
    }

    return 0;
}

int unregister_handler(int fd, struct sockaddr_in addr, char *buffer) {
    char UID[7];
    char password[9];

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        send_udp_reply(fd, addr, Messages.UNR_ERR());
        return 1;
    }
    
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1) {
        send_udp_reply(fd, addr, Messages.UNR_ERR());
        return 1;
    }

    if (!user_is_registered(UID)) {
        send_udp_reply(fd, addr, Messages.UNR_UNR());
    } else if (!check_user_pass(UID, password)) {
        send_udp_reply(fd, addr, Messages.UNR_WRP());
    } else if (!user_is_logged(UID)) {
        send_udp_reply(fd, addr, Messages.UNR_NOK());
    } else {
        unregister_user(UID);
        send_udp_reply(fd, addr, Messages.UNR_OK());
    }

    return 0;
}

int myevents_handler(int fd, struct sockaddr_in addr, char *buffer) {
    char UID[7];
    char password[9];

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        send_udp_reply(fd, addr, Messages.LME_ERR());
        return 1;
    }
    
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1) {
        send_udp_reply(fd, addr, Messages.LME_ERR());
        return 1;
    }

    if (!user_is_logged(UID)) {
        send_udp_reply(fd, addr, Messages.LME_NLG());
    } else if (!check_user_pass(UID, password)) {
        send_udp_reply(fd, addr, Messages.LME_WRP());
    } else if (user_events_empty(UID)) {
        send_udp_reply(fd, addr, Messages.LME_NOK());
    } else {
        char response[MAX_BUFFER_SIZE];
        get_user_events(UID, response);
        send_udp_reply(fd, addr, Messages.LME_OK(response));
    }

    return 0;
}

int myreservations_handler(int fd, struct sockaddr_in addr, char *buffer) {
    char UID[7];
    char password[9];

    if (sscanf(buffer, "%*s %s %s", UID, password) != 2) {
        send_udp_reply(fd, addr, Messages.LMR_ERR());
        return 1;
    }
    
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1) {
        send_udp_reply(fd, addr, Messages.LMR_ERR());
        return 1;
    }

    if (!user_is_logged(UID)) {
        send_udp_reply(fd, addr, Messages.LMR_NLG());
    } else if (!check_user_pass(UID, password)) {
        send_udp_reply(fd, addr, Messages.LMR_WRP());
    } else if (user_reservations_empty(UID)) {
        send_udp_reply(fd, addr, Messages.LMR_NOK());
    } else {
        char response[MAX_BUFFER_SIZE];
        get_user_reservations(UID, response);  // max 50 reservations
        send_udp_reply(fd, addr, Messages.LMR_OK(response));
    }

    return 0;
}

// ==================== TCP HANDLERS ====================


int create_handler(int fd, char *buffer) {
    char UID[7], password[9], name[11], event_date[11], event_time[6], fname[25];
    int attendance_size, fsize;

    // Parse do header - data e hora são campos separados
    if (sscanf(buffer, "%*s %s %s %s %s %s %d %s %d",
               UID, password, name, event_date, event_time,
               &attendance_size, fname, &fsize) != 8) {
        send_tcp_reply(fd, Messages.CRE_ERR());
        return 1;
    }
    
    // Juntar date e time
    char event_datetime[17];
    snprintf(event_datetime, sizeof(event_datetime), "%s %s", event_date, event_time);

    // Validações
    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1 || 
        is_valid_name(name) == 1 || is_valid_nseats(attendance_size) == 1) {
        send_tcp_reply(fd, Messages.CRE_ERR());
        return 1;
    }
    
    if (fsize <= 0 || fsize > 10000000) {
        send_tcp_reply(fd, Messages.CRE_ERR());
        return 1;
    }

    if (!user_is_logged(UID)) {
        send_tcp_reply(fd, Messages.CRE_NLG());
        return 1;
    }
    
    if (!check_user_pass(UID, password)) {
        send_tcp_reply(fd, Messages.CRE_WRP());
        return 1;
    }
    
    char *EID = get_next_eid();
    if (EID == NULL) {
        send_tcp_reply(fd, Messages.CRE_NOK());
        return 1;
    }
    
    // Criar evento (create_event vai ler o ficheiro do socket)
    if (create_event(fd, UID, EID, name, event_datetime, attendance_size, fname, fsize) == 0) {
        send_tcp_reply(fd, Messages.CRE_OK(EID));
        free(EID);
        return 0;
    } else {
        send_tcp_reply(fd, Messages.CRE_NOK());
        free(EID);
        return 1;
    }
}

int close_handler(int fd, char *buffer) {
    char UID[7], password[9], EID[4];

    if (sscanf(buffer, "%*s %s %s %s", UID, password, EID) != 3) {
        send_tcp_reply(fd, Messages.CLS_ERR());
        return 1;
    }

    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1 || is_valid_eid(EID) == 1) {
        send_tcp_reply(fd, Messages.CLS_ERR());
        return 1;
    }

    if (!user_is_logged(UID)) {
        send_tcp_reply(fd, Messages.CLS_NLG());
    } else if (!check_user_pass(UID, password)) {
        send_tcp_reply(fd, Messages.CLS_NOK());
    } else if (!event_exists(EID)) {
        send_tcp_reply(fd, Messages.CLS_NOE(EID));
    } else if (!user_owns_event(UID, EID)) {
        send_tcp_reply(fd, Messages.CLS_EOW(EID));
    } else if (event_is_past(EID)) {
        send_tcp_reply(fd, Messages.CLS_PST(EID));
    } else if (event_is_sold_out(EID)) {
        send_tcp_reply(fd, Messages.CLS_SLD(EID));
    } else if (event_is_closed(EID)) {
        send_tcp_reply(fd, Messages.CLS_CLO(EID));
    } else {
        close_event_now(EID);
        send_tcp_reply(fd, Messages.CLS_OK());
    }

    return 0;
}

int list_handler(int fd, char *buffer) {
    if (no_events_exist()) {
        send_tcp_reply(fd, Messages.LST_NOK());
    } else {
        char response[MAX_BUFFER_SIZE];
        get_all_events(response);
        send_tcp_reply(fd, Messages.LST_OK(response));
    }

    return 0;
}

int show_handler(int fd, char *buffer) {
    char EID[4];

    if (sscanf(buffer, "%*s %s", EID) != 1) {
        send_tcp_reply(fd, Messages.SED_ERR());
        return 1;
    }

    if (is_valid_eid(EID) == 1) {
        send_tcp_reply(fd, Messages.SED_ERR());
        return 1;
    }

    if (!event_exists(EID)) {
        send_tcp_reply(fd, Messages.SED_NOK());
    } else {
        check_and_close_if_past(EID);
        
        char response[MAX_BUFFER_SIZE];
        char filename[25];
        int filesize;
        
        if (get_event_details(EID, response, filename, &filesize) == 0) {
            send_tcp_reply(fd, Messages.SED_OK(response));
            send_event_file(fd, EID, filename, filesize);
        } else {
            send_tcp_reply(fd, Messages.SED_NOK());
        }
    }

    return 0;
}

int reserve_handler(int fd, char *buffer) {
    char UID[7], password[9], EID[4];
    int num_seats;

    if (sscanf(buffer, "%*s %s %s %s %d", UID, password, EID, &num_seats) != 4) {
        send_tcp_reply(fd, Messages.RID_ERR());
        return 1;
    }

    if (is_valid_uid(UID) == 1 || is_valid_password(password) == 1 || 
        is_valid_eid(EID) == 1 || num_seats < 1 || num_seats > 999) {
        send_tcp_reply(fd, Messages.RID_ERR());
        return 1;
    }

    if (!user_is_logged(UID)) {
        send_tcp_reply(fd, Messages.RID_NLG());
    } else if (!check_user_pass(UID, password)) {
        send_tcp_reply(fd, Messages.RID_WRP());
    } else if (!event_exists(EID)) {
        send_tcp_reply(fd, Messages.RID_NOK());
    } else if (event_is_past(EID)) {
        check_and_close_if_past(EID);
        send_tcp_reply(fd, Messages.RID_PST());
    } else if (event_is_closed(EID)) {
        send_tcp_reply(fd, Messages.RID_CLS());
    } else if (event_is_sold_out(EID)) {
        send_tcp_reply(fd, Messages.RID_SLD());
    } else {
        int remaining = get_remaining_seats(EID);
        if (num_seats > remaining) {
            send_tcp_reply(fd, Messages.RID_REJ(remaining));
        } else {
            make_reservation(UID, EID, num_seats);
            send_tcp_reply(fd, Messages.RID_ACC());
        }
    }

    return 0;
}

int changepass_handler(int fd, char *buffer) {
    char UID[7], oldpass[9], newpass[9];

    if (sscanf(buffer, "%*s %s %s %s", UID, oldpass, newpass) != 3) {
        send_tcp_reply(fd, Messages.CPS_ERR());
        return 1;
    }

    if (is_valid_uid(UID) == 1 || is_valid_password(oldpass) == 1 || 
        is_valid_password(newpass) == 1) {
        send_tcp_reply(fd, Messages.CPS_ERR());
        return 1;
    }

    if (!user_is_registered(UID)) {
        send_tcp_reply(fd, Messages.CPS_NID());
    } else if (!user_is_logged(UID)) {
        send_tcp_reply(fd, Messages.CPS_NLG());
    } else if (!check_user_pass(UID, oldpass)) {
        send_tcp_reply(fd, Messages.CPS_NOK());
    } else {
        change_password(UID, newpass);
        send_tcp_reply(fd, Messages.CPS_OK());
    }

    return 0;
}

// ==================== MAIN REQUEST DISPATCHERS ====================

// UDP command table
CommandEntry udp_commandTable[] = {
    {"LIN", login_handler},
    {"LOU", logout_handler},
    {"UNR", unregister_handler},
    {"LME", myevents_handler},
    {"LMR", myreservations_handler},
    {NULL, NULL}
};

// TCP command table
CommandEntry tcp_commandTable[] = {
    {"CRE", create_handler},
    {"CLS", close_handler},
    {"LST", list_handler},
    {"SED", show_handler},
    {"RID", reserve_handler},
    {"CPS", changepass_handler},
    {NULL, NULL}
};

void handle_udp_request(int socket, struct sockaddr_in client_addr, 
                       char *command, char *buffer) {
    for (int i = 0; udp_commandTable[i].command != NULL; i++) {
        if (strcmp(udp_commandTable[i].command, command) == 0) {
            udp_commandTable[i].function(socket, client_addr, buffer);
            return;
        }
    }
    send_udp_reply(socket, client_addr, "ERR\n");
}

void handle_tcp_request(int socket, char *command, char *buffer) {
    for (int i = 0; tcp_commandTable[i].command != NULL; i++) {
        if (strcmp(tcp_commandTable[i].command, command) == 0) {
            tcp_commandTable[i].function(socket, buffer);
            return;
        }
    }
    send_tcp_reply(socket, "ERR\n");
}