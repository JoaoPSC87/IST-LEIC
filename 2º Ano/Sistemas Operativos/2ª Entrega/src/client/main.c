#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h> 
#include <sys/types.h>

#include <string.h>

#include "parser.h"
#include "src/client/api.h"
#include "src/common/constants.h"
#include "src/common/io.h"
#include "src/common/protocol.h"

// 1 quando é o cliente a iniciar o disconnect: permite à thread de notificações
// distinguir um disconnect normal de um fecho forçado pelo servidor (SIGUSR1)
static volatile int client_disconnecting = 0;

void *handle_notifications () {
  int fdNotif = getFdNotif();

  while (1) {
    char key[KEY_SIZE]; 
    char value[VALUE_SIZE];

    //lê a notificação: [key(41)][value(41)]
    if (read_all(fdNotif, key, KEY_SIZE, NULL) != 1) break;
    if (read_all(fdNotif, value, VALUE_SIZE, NULL) != 1) break;

    printf("(%s,%s)\n", key, value);
    fflush(stdout);
  }
  // o servidor fechou o pipe de notificações
  if (client_disconnecting) return NULL; // disconnect normal - > deixa o main terminar
  exit(0); // fecho forçado (SIGUSR1) -> termina já
}


int main(int argc, char *argv[]) {
 
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <client_unique_id> <register_pipe_path>\n",
            argv[0]);
    return 1;
  }


  char fifoRegistoPath[100];

  strcpy(fifoRegistoPath, "/tmp/");  
  strcat(fifoRegistoPath, argv[2]);

  char req_pipe_path[40] = "/tmp/req";
  char resp_pipe_path[40] = "/tmp/resp";
  char notif_pipe_path[40] = "/tmp/notif";

  char keys[MAX_NUMBER_SUB][MAX_STRING_SIZE] = {0};
  unsigned int delay_ms;
  size_t num;

  strncat(req_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
  strncat(resp_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
  strncat(notif_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
  
  kvs_connect(req_pipe_path, resp_pipe_path, fifoRegistoPath, notif_pipe_path);

  pthread_t threadToProcessNotifications;

  pthread_create(&threadToProcessNotifications, NULL, &handle_notifications, NULL);


  while (1) {
    switch (get_next(STDIN_FILENO)) {
    case CMD_DISCONNECT:
    client_disconnecting = 1;
      if (kvs_disconnect() != 0) {
        fprintf(stderr, "Failed to disconnect to the server\n");
        return 1;
      }

      printf("Disconnected from server\n");
      return 0;

    case CMD_SUBSCRIBE:
      num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
      if (num == 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }
      
      if (kvs_subscribe(keys[0])) {
        fprintf(stderr, "Command subscribe failed\n");
      }

      break;

    case CMD_UNSUBSCRIBE:
      num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
      if (num == 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }

      if (kvs_unsubscribe(keys[0])) {
        fprintf(stderr, "Command subscribe failed\n");
      }

      break;

    case CMD_DELAY:
      if (parse_delay(STDIN_FILENO, &delay_ms) == -1) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }

      if (delay_ms > 0) {
        printf("Waiting...\n");
        delay(delay_ms);
      }
      break;

    case CMD_INVALID:
      fprintf(stderr, "Invalid command. See HELP for usage\n");
      break;

    case CMD_EMPTY:
      break;

    case EOC:
      // fim do input sem DISCONNECT explícito: desliga e termina
      client_disconnecting = 1;
      kvs_disconnect();
      return 0;
    }
  }
}
