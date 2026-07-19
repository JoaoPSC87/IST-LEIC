#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include <pthread.h>

#include "kvs.h"
#include "constants.h"
#include "main.h"

#define ERROR -1

static struct HashTable *kvs_table = NULL;

//rwlock global que protege a hashtable
static pthread_rwlock_t kvs_lock;

//controlo dos backups concorrentes
static int backupsInExecution = 0;
static pthread_mutex_t backupMutex = PTHREAD_MUTEX_INITIALIZER;

static struct timespec delay_to_timespec(unsigned int delay_ms) {
  return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}

//comparador para qsort de chaves (READ/DELETE saem ordenados)
static int compareKeys(const void *a, const void *b) {
  return strcmp((const char *)a, (const char *)b);
}

int kvs_init(void) {
  if (kvs_table != NULL) {
    fprintf(stderr, "KVS state has already been initialized\n");
    return 1;
  }
  kvs_table = create_hash_table();
  if (kvs_table == NULL) return 1;
  pthread_rwlock_init(&kvs_lock, NULL);
  return 0;
}

int kvs_terminate(void) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }
  free_table(kvs_table);
  kvs_table = NULL;
  pthread_rwlock_destroy(&kvs_lock);
  return 0;
}

int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE]) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }
  pthread_rwlock_wrlock(&kvs_lock);
  for (size_t i = 0; i < num_pairs; i++) {
    if (write_pair(kvs_table, keys[i], values[i]) != 0) {
      fprintf(stderr, "Failed to write keypair (%s,%s)\n", keys[i], values[i]);
    }
  }
  pthread_rwlock_unlock(&kvs_lock);
  return 0;
}

int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }
  qsort(keys, num_pairs, MAX_STRING_SIZE, compareKeys);
  
  pthread_rwlock_rdlock(&kvs_lock);
  snprintf(bufferToPrint, 2000, "[");
  writeToDotOut(bufferToPrint, fdToPrint);
  for (size_t i = 0; i < num_pairs; i++) {
    char *result = read_pair(kvs_table, keys[i]);
    if (result == NULL) {
      snprintf(bufferToPrint, 2000, "(%s, KVSERROR)", keys[i]);
    } else {
      snprintf(bufferToPrint, 2000, "(%s,%s)", keys[i], result);
    }
    writeToDotOut(bufferToPrint, fdToPrint);
    free(result);
  }
  snprintf(bufferToPrint, 2000, "]\n");
  writeToDotOut(bufferToPrint, fdToPrint);
  pthread_rwlock_unlock(&kvs_lock);
  return 0;
}

int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }
  qsort(keys, num_pairs, MAX_STRING_SIZE, compareKeys);

  pthread_rwlock_wrlock(&kvs_lock);
  int aux = 0;
  for (size_t i = 0; i < num_pairs; i++) {
    if (delete_pair(kvs_table, keys[i]) != 0) {
      if (!aux) {
        snprintf(bufferToPrint, 2000, "[");
        writeToDotOut(bufferToPrint, fdToPrint);
        aux = 1;
      }
      snprintf(bufferToPrint, 2000, "(%s,KVSMISSING)", keys[i]);
      writeToDotOut(bufferToPrint, fdToPrint);
    }
  }
  if (aux) {
    snprintf(bufferToPrint, 2000, "]\n");
    writeToDotOut(bufferToPrint, fdToPrint);
  }
  pthread_rwlock_unlock(&kvs_lock);
  return 0;
}

void kvs_show(char *bufferToPrint, int fdToPrint) {
  pthread_rwlock_rdlock(&kvs_lock);
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyNode *keyNode = kvs_table->table[i];
    while (keyNode != NULL) {
      snprintf(bufferToPrint, 2000, "(%s, %s)\n", keyNode->key, keyNode->value);
      writeToDotOut(bufferToPrint, fdToPrint);
      keyNode = keyNode->next;
    }
  }
  pthread_rwlock_unlock(&kvs_lock);
}

int kvs_backup(char *inputFilePath, char *bufferToPrint, int fdToPrint, int backupCounter, int maxBackups) {
  //garante uma vaga ANTES de trancar a tabela
  pthread_mutex_lock(&backupMutex);
  if (backupsInExecution == maxBackups) {
    wait(NULL);
    backupsInExecution--;
  }
  backupsInExecution++;
  pthread_mutex_unlock(&backupMutex);

  //rdlock durante o fork: o snapshot herdado pelo filho é consistente
  pthread_rwlock_rdlock(&kvs_lock);

  pid_t pid = fork();

  if (pid == 0) {
    char outputFilePath[MAX_JOB_FILE_NAME_SIZE];
    strcpy(outputFilePath, inputFilePath);
    char *dot = strrchr(outputFilePath, '.');
    char temp[256];
    snprintf(temp, sizeof(temp), "-%d.bck", backupCounter);
    if (dot != NULL) strcpy(dot, temp);
    else strcat(outputFilePath, temp);

    fdToPrint = open(outputFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fdToPrint == ERROR) {
      perror("Output file inválido");
      _exit(1);
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
      KeyNode *keyNode = kvs_table->table[i];
      while (keyNode != NULL) {
        snprintf(bufferToPrint, 2000, "(%s, %s)\n", keyNode->key, keyNode->value);
        writeToDotOut(bufferToPrint, fdToPrint);
        keyNode = keyNode->next;
      }
    }
    close(fdToPrint);
    _exit(0);
  }
  pthread_rwlock_unlock(&kvs_lock);

  if (pid < 0) {
    pthread_mutex_lock(&backupMutex);
    backupsInExecution--;
    pthread_mutex_unlock(&backupMutex);
    return -1;
  }
  return 0;
}

void kvs_wait(unsigned int delay_ms) {
  struct timespec delay = delay_to_timespec(delay_ms);
  nanosleep(&delay, NULL);
}

// ---- subscrições (chamadas pela tarefa gestora de cada cliente) ----

int kvs_subscribe(const char *key, int notif_fd) {
  pthread_rwlock_wrlock(&kvs_lock);
  int existed = subscribe_key(kvs_table, key, notif_fd);
  pthread_rwlock_unlock(&kvs_lock);
  return existed;
}

int kvs_unsubscribe(const char *key, int notif_fd) {
  pthread_rwlock_wrlock(&kvs_lock);
  int removed = unsubscribe_key(kvs_table, key, notif_fd);
  pthread_rwlock_unlock(&kvs_lock);
  return removed;
}

void kvs_remove_client(int notif_fd) {
  pthread_rwlock_wrlock(&kvs_lock);
  remove_client(kvs_table, notif_fd);
  pthread_rwlock_unlock(&kvs_lock);
}

void kvs_delete_all_subscriptions(void) {
  pthread_rwlock_wrlock(&kvs_lock);
  clear_all_subscriptions(kvs_table);
  pthread_rwlock_unlock(&kvs_lock);
}
