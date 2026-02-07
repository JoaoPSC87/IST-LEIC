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

#include "aux_functions.h"

#define ERROR -1

static struct HashTable* kvs_table = NULL;

int backupsInExecution;

/// Calculates a timespec from a delay in milliseconds.
/// @param delay_ms Delay in milliseconds.
/// @return Timespec with the given delay.
static struct timespec delay_to_timespec(unsigned int delay_ms) {
  return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}

int kvs_init(char *bufferToPrint, int fdToPrint) {
  if (kvs_table != NULL) {
    
    snprintf(bufferToPrint, 2000, "KVS state has already been initialized\n" );
    writeToDotOut(bufferToPrint, fdToPrint);
    return 1;
  }

  kvs_table = create_hash_table();
  return kvs_table == NULL;
}

int kvs_terminate(char *bufferToPrint, int fdToPrint) {
  if (kvs_table == NULL) {
    snprintf(bufferToPrint, 2000, "KVS state has already been initialized\n" );
    writeToDotOut(bufferToPrint, fdToPrint);
    return 1;
  }

  free_table(kvs_table);
  return 0;
}

int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE],char *bufferToPrint, int fdToPrint, pthread_rwlock_t rwlock) {
  if (kvs_table == NULL) {
    snprintf(bufferToPrint, 2000, "KVS state must be initialized\n");
    writeToDotOut(bufferToPrint, fdToPrint);
    return 1;
  }

  pthread_rwlock_wrlock(&rwlock);

  for (size_t i = 0; i < num_pairs; i++) {
    if (write_pair(kvs_table, keys[i], values[i]) != 0) {
      snprintf(bufferToPrint, 2000, "Failed to write keypair (%s,%s)\n", keys[i], values[i]);
      writeToDotOut(bufferToPrint, fdToPrint);
    }
  }

  pthread_rwlock_unlock(&rwlock);

  return 0;
}

int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint, pthread_rwlock_t rwlock) {
  if (kvs_table == NULL) {
    snprintf(bufferToPrint, 2000, "KVS state must be initialized\n");
    writeToDotOut(bufferToPrint, fdToPrint);
    return 1;
  }

  pthread_rwlock_rdlock(&rwlock);

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

  pthread_rwlock_unlock(&rwlock);
  return 0;
}

int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], char *bufferToPrint, int fdToPrint, pthread_rwlock_t rwlock) {
  if (kvs_table == NULL) {
    snprintf(bufferToPrint, 2000, "KVS state must be initialized\n");
    writeToDotOut(bufferToPrint, fdToPrint);
    return 1;
  }

  pthread_rwlock_wrlock(&rwlock);

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

  pthread_rwlock_unlock(&rwlock);

  return 0;
}

void kvs_show(char *bufferToPrint, int fdToPrint, pthread_rwlock_t rwlock) {
  
  pthread_rwlock_rdlock(&rwlock);
  
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyNode *keyNode = kvs_table->table[i];
    while (keyNode != NULL) {

      snprintf(bufferToPrint, 2000, "(%s, %s)\n", keyNode->key, keyNode->value);
      writeToDotOut(bufferToPrint, fdToPrint);
      keyNode = keyNode->next;
    }
  }

  pthread_rwlock_unlock(&rwlock);

}

int kvs_backup(char * inputFilePath, char* bufferToPrint, int fdToPrint, int backupCounter, int maxBackups, pthread_rwlock_t rwlock) {


  pthread_rwlock_rdlock(&rwlock);
  
  //se já estiver no maximo de backups simultaneos espera que um acabe
  if (maxBackups == backupsInExecution)
  {
    wait(NULL);
    backupsInExecution--;
  }

  pid_t pid;

  backupsInExecution++;

  pid = fork();
  
  if(pid < 0)
  {
    return -1;
  }
  
  else if ( pid == 0) {


  char outputFilePath[MAX_JOB_FILE_NAME_SIZE];

  strcpy(outputFilePath, inputFilePath);

  char *dot = strrchr(outputFilePath, '.');
  
  if (dot != NULL) {
      char temp[256]; 
      snprintf(temp, sizeof(temp), "-%d.bck", backupCounter);
      strcpy(dot, temp);
  } else {
      char temp[256];
      snprintf(temp, sizeof(temp), "-%d.bck", backupCounter);
      strcat(outputFilePath, temp);
  }

 
  fdToPrint = open(outputFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fdToPrint == ERROR){
    perror("Output file inválido");
    close(fdToPrint);
    return ERROR;
  }

  //percorre a hashtable e faz show ao conteúdo
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyNode *keyNode = kvs_table->table[i];
    while (keyNode != NULL) {

      snprintf(bufferToPrint, 2000, "(%s, %s)\n", keyNode->key, keyNode->value);

      writeToDotOut(bufferToPrint, fdToPrint);
      
      keyNode = keyNode->next;
    }
  }

  close(fdToPrint);

  exit(0);

  }

  pthread_rwlock_unlock(&rwlock);

  return 0;
}

void kvs_wait(unsigned int delay_ms) {
  struct timespec delay = delay_to_timespec(delay_ms);
  nanosleep(&delay, NULL);
}