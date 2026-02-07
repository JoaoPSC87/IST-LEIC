#ifndef _FUNCS_H_
#define _FUNCS_H_

#include "../func_aux/validations.h"
#include "protocols.h"


typedef struct {
    char _UID[7];
    char _password[9];
} User;


typedef struct {
    const char *command;
    int (*function)(char *);
} CommandEntry;

// Variáveis globais
extern bool loggedIn;
extern User client;
extern char last_eid[4];

// Declarações das funções de comando
int login(char *buffer);
int logout(char *buffer);
int unregister(char *buffer);
int myevents(char *buffer);
int myreservations(char *buffer);
int create(char *buffer);
int close_event(char *buffer);
int reserve(char *buffer);
int list(char *buffer);
int show(char *buffer);
int changepass(char *buffer);
int exit_(char *buffer);

// Função para executar comandos
void executeCommand(const char *command, char *buffer);

#endif