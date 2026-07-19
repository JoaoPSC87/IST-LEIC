#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <pthread.h>

#include <linux/limits.h>

#include "constants.h"
#include "parser.h"
#include "operations.h"

#include "main.h"

#define ERROR -1

//maximo de backups a serem efectuados ao mesmo tempo.
int maxBackups;

//maximo de threads a processar os ficheiros ao mesmo tempo.
int maxThreads;

//fila de trabalho: caminhos dos ficheiros .job por processar
char **jobFiles = NULL;
int numJobs = 0;
int nextJob = 0;
pthread_mutex_t jobMutex = PTHREAD_MUTEX_INITIALIZER;

//função que recebe uma string e um file descriptor
//e imprime nesse ficheiro o conteudo da string
int writeToDotOut(char *b, int fd) {
    
    size_t len = strlen(b);
    size_t done = 0;

    while (len > done) {
        ssize_t bytes_written = write(fd, b + done, len - done);

        if (bytes_written < 0) {
            perror("write error");
            return EXIT_FAILURE;
        }
        done += (size_t)bytes_written;
    }

    return 0;
}

//verifica se o nome do ficheiro termina mesmo em ".job"
static int hasJobExtension(const char *name) {
  size_t len = strlen(name);
  return len > 4 && strcmp(name + len - 4, ".job") == 0;
}

//processa todos os comandos de um ficheiro .job,
//escrevendo os resultados no ficheiro .out correspondente
//(chamada pelos workers do pool)
void processEachFile (char *inputFilePath) {

  //counter para o numero de backups efectuados por cada ficheiro
  int backupCounter = 0;

  //buffer e fd de output LOCAIS - cada thread/ficheiro tem os seus
  char bufferToPrint[2000];
  int fdToPrint;

  char outputFilePath[MAX_JOB_FILE_NAME_SIZE];
  
  int fdJobs = open(inputFilePath, O_RDONLY);
    if (fdJobs == ERROR){
    perror("Input file inválido");
    return;
    }

  //constroi o caminho do ficheiro que vai ser imprimido
  strcpy(outputFilePath, inputFilePath);
  char *dot = strrchr(outputFilePath, '.');
  if (dot != NULL) {
    strcpy(dot, ".out");
  } else {
    strcat(outputFilePath, ".out");
  }


  fdToPrint = open(outputFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fdToPrint == ERROR){
    perror("Output file inválido");
    close(fdJobs);
    return;

  }
  //ciclo que processa cada comando contido no ficheiro a ser processado
  int finish = 0;
  while (!finish) {
    char keys[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    char values[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    unsigned int delay;
    size_t num_pairs;

    switch (get_next(fdJobs)) {
      case CMD_WRITE:
        num_pairs = parse_write(fdJobs, keys, values, MAX_WRITE_SIZE, MAX_STRING_SIZE);
        if (num_pairs == 0) {
          fprintf(stderr, "Invalid command. See HELP for usage\n" );
          continue;
        }

        if (kvs_write(num_pairs, keys, values)) {
          fprintf(stderr, "Failed to write pair\n" );
        }

        break;

      case CMD_READ:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);

        if (num_pairs == 0) {
          fprintf(stderr, "Invalid command. See HELP for usage\n" );
          continue;
        }

        if (kvs_read(num_pairs, keys, bufferToPrint, fdToPrint)) {
         fprintf(stderr, "Failed to read pair\n" );
        }
        break;

      case CMD_DELETE:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);

        if (num_pairs == 0) {
          fprintf(stderr, "Invalid command. See HELP for usage\n" );
          continue;
        }

        if (kvs_delete(num_pairs, keys, bufferToPrint, fdToPrint)) {
          fprintf(stderr, "Failed to delete pair\n" );
        }
        break;

      case CMD_SHOW:

        kvs_show(bufferToPrint, fdToPrint);
        break;

      case CMD_WAIT:
        if (parse_wait(fdJobs, &delay, NULL) == -1) {
          fprintf(stderr, "Invalid command. See HELP for usage\n" );
          continue;
        }

        if (delay > 0) {
          printf("Waiting...\n");
          kvs_wait(delay);
        }
        break;

      case CMD_BACKUP:

        backupCounter++;
        if (kvs_backup(inputFilePath, bufferToPrint, fdToPrint, backupCounter, maxBackups)) {
          fprintf(stderr, "Failed to perform backup.\n" );
        }
        break;

      case CMD_INVALID:
        fprintf(stderr, "Invalid command. See HELP for usage\n" );
        break;

      case CMD_HELP:
        printf("Available commands:\nWRITE [(key,value)(key2,value2),...]\nREAD [key,key2,...]\nDELETE [key,key2,...]\nSHOW\nWAIT <delay_ms>\nBACKUP\nHELP\n" );
        break;
        
      case CMD_EMPTY:
        break;

      case EOC:
        close(fdToPrint);
        finish = 1;
        break;
    }
  }

  close(fdJobs);
}

// cada worker puxa o próximo ficheiro da fila até ela esvaziar
void *workerThread(void *arg) {
  (void)arg;
  while (1) {
    pthread_mutex_lock(&jobMutex);
    if (nextJob >= numJobs) {
      pthread_mutex_unlock(&jobMutex);
      break;
    }
    char *inputFilePath = jobFiles[nextJob];
    nextJob++;
    pthread_mutex_unlock(&jobMutex);

    processEachFile(inputFilePath);
    free(inputFilePath);
  }
  return NULL;
}

int main(int argc, char *argv[]) {

  DIR *dir;
  struct dirent *entry;

  if (kvs_init()) {
    fprintf(stderr, "Failed to initialize KVS\n");
    return 1;
  }

  //verifica o numero de argumentos na linha de comandos
  if (argc != 4){
    fprintf(stderr, "Invalid arguments. See HELP for usage\n");
    return 1;
  }

  maxBackups = atoi(argv[2]);
  maxThreads = atoi(argv[3]);
  if (maxBackups < 1) maxBackups = 1;
  if (maxThreads < 1) maxThreads = 1;

  if ((dir = opendir(argv[1])) == NULL) {
    perror("Invalid directory");
    return ERROR;
  }

  //1ª fase: recolhe os caminhos de todos os ficheiros .job da diretoria
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    if (!hasJobExtension(entry->d_name))
      continue;

    char *inputFilePath = malloc(sizeof(char) * MAX_JOB_FILE_NAME_SIZE);
    int written = snprintf(inputFilePath, MAX_JOB_FILE_NAME_SIZE, "%s/%s", argv[1], entry->d_name);
    if (written < 0 || written >= MAX_JOB_FILE_NAME_SIZE) {
      fprintf(stderr, "Caminho demasiado longo, ignorado: %s/%s\n", argv[1], entry->d_name);
      free(inputFilePath);
      continue;
    }

    char **tmp = realloc(jobFiles, (size_t)(numJobs + 1) * sizeof(char *));
    if (tmp == NULL) {
      free(inputFilePath);
      break;
    }
    jobFiles = tmp;
    jobFiles[numJobs] = inputFilePath;
    numJobs++;
  }
  closedir(dir);

  //2ª fase: lança o pool de workers e espera que a fila esvazie
  int numWorkers = (maxThreads < numJobs) ? maxThreads : numJobs;
  if (numWorkers > 0) {
    pthread_t workers[numWorkers];
    for (int i = 0; i < numWorkers; i++) {
      pthread_create(&workers[i], NULL, workerThread, NULL);
    }
    for (int i = 0; i < numWorkers; i++) {
      pthread_join(workers[i], NULL);
    }
  }
  free(jobFiles);

  //espera que acabem TODOS os processos filhos (backups)
  while (wait(NULL) != -1 || errno != ECHILD) {}

  kvs_terminate();

  return 0;
}

