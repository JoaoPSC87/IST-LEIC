#ifndef KVS_OPERATIONS_H
#define KVS_OPERATIONS_H

#include <stddef.h>

#include "constants.h"

///Inicializa/destrói o estado do KVS (0 em sucesso, 1 em erro)
int kvs_init(void);
int kvs_terminate(void);

///Operações de ficheiros .job (o output vai para o .out via bufferToPrint/fd)
int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE]);
int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint);
int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint);
void kvs_show(char *bufferToPrint, int fdToPrint);
int kvs_backup(char *inputFilePath, char *bufferToPrint, int fdToPrint, int backupCounter, int maxBackups);
void kvs_wait(unsigned int delay_ms);

///Subscrições (chamadas pela tarefa gestora de cada cliente)
int kvs_subscribe(const char *key, int notif_fd); //1 se a chave existia, 0 caso contrário
int kvs_unsubscribe(const char *key, int notif_fd); //0 se removida, 1 caso contrário
void kvs_remove_client(int notif_fd); //remove todas as subscrições do cliente
void kvs_delete_all_subscriptions(void);

#endif  // KVS_OPERATIONS_H
