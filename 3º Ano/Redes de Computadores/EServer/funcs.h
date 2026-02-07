#ifndef _FUNCS_
#define _FUNCS_

#include "../func_aux/validations.h"
#include "mensagens.h"

// Directory paths for server
#define SERVER_DIR_USERS "USERS"
#define SERVER_DIR_EVENTS "EVENTS"

// User management functions
bool user_is_registered(char *UID);
bool user_is_logged(char *UID);
bool check_user_pass(char *UID, char *password);
void register_user(char *UID, char *password);
void log_user_in(char *UID);
void log_user_out(char *UID);
void unregister_user(char *UID);
void change_password(char *UID, char *new_password);

// Event management functions
bool event_exists(char *EID);
bool user_owns_event(char *UID, char *EID);
bool event_is_closed(char *EID);
bool event_is_past(char *EID);
bool event_is_sold_out(char *EID);
int get_event_state(char *EID);
void check_and_close_if_past(char *EID);
void close_event_now(char *EID);
char *get_next_eid();
int create_event(int tcp_fd, char *UID, char *EID, char *name, char *event_date, 
                 int attendance_size, char *fname, int fsize);

// Reservation functions
int get_remaining_seats(char *EID);
void make_reservation(char *UID, char *EID, int num_seats);
bool user_reservations_empty(char *UID);
void get_user_reservations(char *UID, char *response);

// Event listing functions
bool no_events_exist();
bool user_events_empty(char *UID);
void get_user_events(char *UID, char *response);
void get_all_events(char *response);

int get_event_details(char *EID, char *response, char *filename, int *filesize);
void send_event_file(int fd, char *EID, char *filename, int filesize);

void check_server_dirs();

#endif