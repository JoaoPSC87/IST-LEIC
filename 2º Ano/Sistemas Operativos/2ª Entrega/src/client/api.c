#include "api.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>


#include "src/common/constants.h"
#include "src/common/protocol.h"
#include "src/common/io.h"

//o cliente tem uma sessão de cada vez: guardamos os caminhos e os fds
static char _req_pipe_path[MAX_PIPE_PATH_LENGTH + 1];
static char _resp_pipe_path[MAX_PIPE_PATH_LENGTH + 1];
static char _notif_pipe_path[MAX_PIPE_PATH_LENGTH + 1];

static int _fdReq = -1;
static int _fdResp = -1;
static int _fdNotif = -1;

int getFdNotif(void) { return _fdNotif; }

//copia src para um campo de tamanho fixo, preenchendo o resto com '\0'
static void fill_field(char *dst, const char *src, size_t field_size) {
  memset(dst, 0, field_size);
  size_t n = strlen(src);
  if (n > field_size) n = field_size;
  memcpy(dst, src, n);
}

//imprime a resposta no formato do enunciado (resp[0]=opcode, resp[1]=result)
static void print_response(const char *op, const char *resp) {
  printf("Server returned %d for operation: %s\n", (int)resp[1], op);
  fflush(stdout);
}

void terminate() {
  if (_fdReq != -1) close(_fdReq);
  if (_fdResp != -1) close(_fdResp);
  if (_fdNotif != -1) close(_fdNotif);
  unlink(_req_pipe_path);
  unlink(_resp_pipe_path);
  unlink(_notif_pipe_path);
}

int kvs_connect(char const *req_pipe_path, char const *resp_pipe_path,
                char const *server_pipe_path, char const *notif_pipe_path) {
  //guarda os caminhos (já limpos, sem padding) para uso posterior
  strncpy(_req_pipe_path, req_pipe_path, MAX_PIPE_PATH_LENGTH);
  strncpy(_resp_pipe_path, resp_pipe_path, MAX_PIPE_PATH_LENGTH);
  strncpy(_notif_pipe_path, notif_pipe_path, MAX_PIPE_PATH_LENGTH);
  _req_pipe_path[MAX_PIPE_PATH_LENGTH] = '\0';
  _resp_pipe_path[MAX_PIPE_PATH_LENGTH] = '\0';
  _notif_pipe_path[MAX_PIPE_PATH_LENGTH] = '\0';

  //cria os 3 FIFOs da sessão (removendo restos de sessões anteriores)
  unlink(_req_pipe_path);
  unlink(_resp_pipe_path);
  unlink(_notif_pipe_path);
  if (mkfifo(_req_pipe_path, 0666) != 0 ||
      mkfifo(_resp_pipe_path, 0666) != 0 ||
      mkfifo(_notif_pipe_path, 0666) != 0) {
    perror("mkfifo");
    return 1;
  }

  //monta o pedido de connect: [OP=1][req(40)][resp(40)][notif(40)]
  char msg[CONNECT_REQUEST_SIZE];
  memset(msg, 0, sizeof(msg));
  msg[0] = (char)OP_CODE_CONNECT;
  fill_field(msg + 1, _req_pipe_path, PIPE_PATH_SIZE);
  fill_field(msg + 1 + PIPE_PATH_SIZE, _resp_pipe_path, PIPE_PATH_SIZE);
  fill_field(msg + 1 + 2 * PIPE_PATH_SIZE, _notif_pipe_path, PIPE_PATH_SIZE);

  //envia o pedido pelo FIFO de registo do servidor
  int fdServer = open(server_pipe_path, O_WRONLY);
  if (fdServer == -1) {
    perror("open server pipe");
    return 1; 
  }
  if (write_all(fdServer, msg, CONNECT_REQUEST_SIZE) == -1) {
    close(fdServer);
    return 1;
  }
  close(fdServer);

  //abre os FIFOs da sessão pela MESMA ordem do servidor (req, resp, notif)
  //para não haver deadlock nos open() bloqueantes dos named pipes
  _fdReq = open(_req_pipe_path, O_WRONLY);
  _fdResp = open(_resp_pipe_path, O_RDONLY);
  _fdNotif = open(_notif_pipe_path, O_RDONLY);
  if (_fdReq == -1 || _fdResp == -1 || _fdNotif == -1) {
    perror("open session pipes");
    return 1;
  }

  //lê a resposta do connect: [OP=1][result]
  char resp[RESPONSE_SIZE];
  if (read_all(_fdResp, resp, RESPONSE_SIZE, NULL) != 1) return 1;
  print_response("connect", resp);
  return resp[1] == 0 ? 0 : 1;
}

int kvs_disconnect(void) {
  char op = (char)OP_CODE_DISCONNECT;
  if (write_all(_fdReq, &op, DISCONNECT_REQUEST_SIZE) == -1) return 1;

  char resp[RESPONSE_SIZE];
  if (read_all(_fdResp, resp, RESPONSE_SIZE, NULL) != 1) return 1;
  print_response("disconnect", resp);

  terminate();
  _fdReq = _fdResp = _fdNotif = -1;
  return resp[1] == 0 ? 0 : 1;
}

//subscribe e unsubscribe só diferem no opcode e no nome impresso
static int send_key_request(char op, const char *op_name, const char *key) {
  char msg[SUB_REQUEST_SIZE];
  memset(msg, 0, sizeof(msg));
  msg[0] = op;
  fill_field(msg + 1, key, KEY_SIZE);

  if (write_all(_fdReq, msg, SUB_REQUEST_SIZE) == -1) return 1;

  char resp[RESPONSE_SIZE];
  if (read_all(_fdResp, resp, RESPONSE_SIZE, NULL) != 1) return 1;
  print_response(op_name, resp);
  return 0;
}

int kvs_subscribe(const char *key) {
  return send_key_request((char)OP_CODE_SUBSCRIBE, "subscribe", key);
}

int kvs_unsubscribe(const char *key) {
  return send_key_request((char)OP_CODE_UNSUBSCRIBE, "unsubscribe", key);
}