#include "api.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h>

#include <string.h>

#include <unistd.h>

#include "src/common/constants.h"
#include "src/common/protocol.h"



char _req_pipe_path[40];
char _resp_pipe_path[40];
char _notif_pipe_path[40];

int _fdReq;
int _fdResp;
int _fdNotif;

void terminate(){

  close(_fdReq);
  close(_fdResp);
  close(_fdNotif);

  unlink(_req_pipe_path);
  unlink(_resp_pipe_path);
  unlink(_notif_pipe_path);

}


int getFdNotif () {

  return _fdNotif;
}

int kvs_connect(char const *req_pipe_path, char const *resp_pipe_path,
                char const *server_pipe_path, char const *notif_pipe_path) {
  // create pipes and connect

  
  strcpy(_req_pipe_path,req_pipe_path);
  strcpy(_resp_pipe_path,resp_pipe_path);
  strcpy(_notif_pipe_path,notif_pipe_path);



  unlink(_req_pipe_path);
  unlink(_resp_pipe_path);
  unlink(_notif_pipe_path);

  mkfifo(_req_pipe_path, 0777);
  mkfifo(_resp_pipe_path, 0777);
  mkfifo(_notif_pipe_path, 0777);

  int fdClienteRegisto = open(server_pipe_path , O_WRONLY);

  char mensagemEnviar [40*3+1];

  strcpy(mensagemEnviar, "1");
  strcat(mensagemEnviar, _req_pipe_path);

  while (strlen(mensagemEnviar) < 41)
  {
    strcat(mensagemEnviar, " ");
  }

  strcat(mensagemEnviar, _resp_pipe_path);

  while (strlen(mensagemEnviar) < 81)
  {
    strcat(mensagemEnviar, " ");
  }


  strcat(mensagemEnviar, _notif_pipe_path);

  while (strlen(mensagemEnviar) < 121)
  {
    strcat(mensagemEnviar, " ");
  }


  write(fdClienteRegisto, mensagemEnviar, sizeof(mensagemEnviar));


  _fdReq = open(_req_pipe_path, O_WRONLY);
  _fdResp = open(_resp_pipe_path, O_RDONLY);
  _fdNotif = open(_notif_pipe_path, O_RDONLY);


  char resposta [2];

  read(_fdResp, resposta, sizeof(resposta));

  printf("\nServer returned %c for operation: connect\n", resposta[1]);

  

  return 0;
}

int kvs_disconnect(void) {
  // close pipes and unlink pipe files


  write(_fdReq, "2", sizeof(char));

  char resposta [2];

  if (read(_fdResp, resposta, sizeof(resposta)) == EOF){
    terminate();
  }

  printf("\nServer returned %c for operation: disconnect\n", resposta[1]);


  close(_fdReq);
  close(_fdResp);
  close(_fdNotif);

  unlink(_req_pipe_path);
  unlink(_resp_pipe_path);
  unlink(_notif_pipe_path);

  return 0;
}

int kvs_subscribe(const char *key) {
  // send subscribe message to request pipe and wait for response in response
  // pipe
  
  char mensagemEnviar [41];

  strcpy(mensagemEnviar, "3");
  strcat(mensagemEnviar, key);


  write(_fdReq, mensagemEnviar, sizeof(mensagemEnviar));

  char resposta [2];


  read(_fdResp, resposta, sizeof(resposta));

  
  if (read(_fdResp, resposta, sizeof(resposta)) == EOF){
    terminate();
  }
  

  printf("\nServer returned %c for operation: subscribe\n", resposta[1]);


  return 0;
}

int kvs_unsubscribe(const char *key) {
  // send unsubscribe message to request pipe and wait for response in response
  // pipe

  char mensagemEnviar [41];

  strcpy(mensagemEnviar, "4");
  strcat(mensagemEnviar, key);

  write(_fdReq, mensagemEnviar, sizeof(mensagemEnviar));

  char resposta [2];

  
  if (read(_fdResp, resposta, sizeof(resposta)) == EOF){
    terminate();
  }

  read(_fdResp, resposta, sizeof(resposta));

  printf("\nServer returned %c for operation: unsubscribe\n", resposta[1]);



  return 0;
}
