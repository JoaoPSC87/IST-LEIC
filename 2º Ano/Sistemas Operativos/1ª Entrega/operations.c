#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/wait.h>

#include <pthread.h>

#include "kvs.h"
#include "constants.h"
#include "main.h"

#define ERROR -1

static struct HashTable* kvs_table = NULL;

static int backupsInExecution;
static pthread_mutex_t backupMutex = PTHREAD_MUTEX_INITIALIZER;

/// Calculates a timespec from a delay in milliseconds.
/// @param delay_ms Delay in milliseconds.
/// @return Timespec with the given delay.
static struct timespec delay_to_timespec(unsigned int delay_ms) {
  return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}

static int compareKeys(const void *a, const void *b) {
  return strcmp((const char *)a, (const char *)b);
}

//marca no vetor needed[] os buckets tocados pelas chaves do comando
static void computeNeededBuckets(size_t num_pairs, char keys[][MAX_STRING_SIZE], int needed[TABLE_SIZE]) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    needed[i] = 0;
  }
  for (size_t i = 0; i < num_pairs; i++) {
    int b = hash(keys[i]);
    if (b >= 0)
      needed[b] = 1;
  }
}

//tranca os buckets marcados por ordem CRESCENTE de índice -
//ordem uniforme de aquisição -> sem interblocagem
static void lockBuckets(const int needed[TABLE_SIZE], int writeMode) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (needed[i]) {
      if (writeMode)
        pthread_rwlock_wrlock(&kvs_table->bucketLocks[i]);
      else
        pthread_rwlock_rdlock(&kvs_table->bucketLocks[i]);
    }
  }
}

static void unlockBuckets(const int needed[TABLE_SIZE]) {
  for (int i = 0; i < TABLE_SIZE; i++)
    if (needed[i])
      pthread_rwlock_unlock(&kvs_table->bucketLocks[i]);
}

int kvs_init() {
  if (kvs_table != NULL) {
    
    fprintf(stderr, "KVS state has already been initialized\n" );
    return 1;
  }

  kvs_table = create_hash_table();
  return kvs_table == NULL;
}

int kvs_terminate() {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n" );
    return 1;
  }

  free_table(kvs_table);
  return 0;
}

int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE]) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }

  int needed[TABLE_SIZE];
  computeNeededBuckets(num_pairs, keys, needed);
  lockBuckets(needed, 1);

  for (size_t i = 0; i < num_pairs; i++) {
    if (write_pair(kvs_table, keys[i], values[i]) != 0) {
      fprintf(stderr, "Failed to write keypair (%s,%s)\n", keys[i], values[i]);
    }
  }

  unlockBuckets(needed);

  return 0;
}

int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }

  qsort(keys, num_pairs, MAX_STRING_SIZE, compareKeys);

  int needed[TABLE_SIZE];
  computeNeededBuckets(num_pairs, keys, needed);
  lockBuckets(needed, 0);

  snprintf(bufferToPrint, 2000, "[");
  writeToDotOut(bufferToPrint, fdToPrint);

  for (size_t i = 0; i < num_pairs; i++) {
    char* result = read_pair(kvs_table, keys[i]);
    if (result == NULL) {
      snprintf(bufferToPrint, 2000, "(%s,KVSERROR)", keys[i]);
      writeToDotOut(bufferToPrint, fdToPrint);
    } else {
      snprintf(bufferToPrint, 2000, "(%s,%s)", keys[i], result);
      writeToDotOut(bufferToPrint, fdToPrint);
    }
    free(result);
  }
  snprintf(bufferToPrint, 2000, "]\n");
  writeToDotOut(bufferToPrint, fdToPrint);

  unlockBuckets(needed);
  return 0;
}

int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint) {
  if (kvs_table == NULL) {
    fprintf(stderr, "KVS state must be initialized\n");
    return 1;
  }

  qsort(keys, num_pairs, MAX_STRING_SIZE, compareKeys);

  int needed[TABLE_SIZE];
  computeNeededBuckets(num_pairs, keys, needed);
  lockBuckets(needed, 1);

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

  unlockBuckets(needed);

  return 0;
}

void kvs_show(char *bufferToPrint, int fdToPrint) {
  
  for (int i = 0; i < TABLE_SIZE; i++)
    pthread_rwlock_rdlock(&kvs_table->bucketLocks[i]);

  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyNode *keyNode = kvs_table->table[i];
    while (keyNode != NULL) {
      snprintf(bufferToPrint, 2000, "(%s, %s)\n", keyNode->key, keyNode->value);
      writeToDotOut(bufferToPrint, fdToPrint);
      keyNode = keyNode->next;
    }
  }

  for (int i = 0; i < TABLE_SIZE; i++)
    pthread_rwlock_unlock(&kvs_table->bucketLocks[i]);
}

int kvs_backup(char * inputFilePath, char* bufferToPrint, int fdToPrint, int backupCounter, int maxBackups) {

  //garante uma vaga ANTES de trancar a tabela:
  //se já há maxBackups filhos a correr, espera que um termine
  pthread_mutex_lock(&backupMutex);
  if (backupsInExecution == maxBackups) {
    wait(NULL);
    backupsInExecution--;
  }
  backupsInExecution++;
  pthread_mutex_unlock(&backupMutex);

  //tranca TODOS os buckets em leitura: o snapshot herdado pelo fork é consistente
  for (int i = 0; i < TABLE_SIZE; i++)
    pthread_rwlock_rdlock(&kvs_table->bucketLocks[i]);

  pid_t pid = fork();

  if (pid == 0) {
    //processo filho: escreve o estado herdado para o .bck
    char outputFilePath[MAX_JOB_FILE_NAME_SIZE];
    strcpy(outputFilePath, inputFilePath);

    char *dot = strrchr(outputFilePath, '.');
    char temp[256];
    snprintf(temp, sizeof(temp), "-%d.bck", backupCounter);
    if (dot != NULL) {
      strcpy(dot, temp);
    } else {
      strcat(outputFilePath, temp);
    }

    fdToPrint = open(outputFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fdToPrint == ERROR) {
      perror("Output file inválido");
      _exit(1);
    }

    //percorre a hashtable e escreve o conteúdo
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

  for (int i = 0; i < TABLE_SIZE; i++)
    pthread_rwlock_unlock(&kvs_table->bucketLocks[i]);

  if (pid < 0) {
    //fork falhou: devolve a vaga reservada
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