#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

#include "../func_aux/validations.h"
#include "mensagens.h"

char *handle_myevents(char *response);

char *handle_myreservations(char *response);

char *handle_list(char *response);

int udp_pedido(char *request);


int tcp_pedido(char *request, char *filename, int filesize);

#endif