#ifndef _VALIDATIONS_
#define _VALIDATIONS_

#define MAX_NAME_USER 6
#define MAX_NAME_EVENT 10
#define MAX_FILENAME 24
#define MAX_BUFFER_SIZE 6010
#define DIR_USER "../EUser" 
#define DIR_EVENTS "../EServer"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>

extern char *esip;
extern char *esport;

#define PATH_SIZE 6010

int is_valid_uid(char *uid);
int is_valid_password(const char *password);
int is_valid_eid(char *eid);
int is_valid_nseats(int nseats);
int is_valid_name(char *name);

#endif