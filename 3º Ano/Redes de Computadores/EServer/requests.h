#ifndef _REQUESTS_
#define _REQUESTS_

#include "../func_aux/validations.h"
#include "funcs.h"
#include "mensagens.h"

typedef int (*CommandFunction)();

// Command table entry
typedef struct {
    const char *command;
    CommandFunction function;
} CommandEntry;

// Main handlers
void handle_udp_request(int socket, struct sockaddr_in client_addr, 
                       char *command, char *buffer);
void handle_tcp_request(int socket, char *command, char *buffer);

// UDP request handlers
int login_handler(int fd, struct sockaddr_in addr, char *buffer);
int logout_handler(int fd, struct sockaddr_in addr, char *buffer);
int unregister_handler(int fd, struct sockaddr_in addr, char *buffer);
int myevents_handler(int fd, struct sockaddr_in addr, char *buffer);
int myreservations_handler(int fd, struct sockaddr_in addr, char *buffer);

// TCP request handlers
int create_handler(int fd, char *buffer);
int close_handler(int fd, char *buffer);
int list_handler(int fd, char *buffer);
int show_handler(int fd, char *buffer);
int reserve_handler(int fd, char *buffer);
int changepass_handler(int fd, char *buffer);

#endif