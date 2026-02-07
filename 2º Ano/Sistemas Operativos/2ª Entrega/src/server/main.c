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
#include "kvs.h"
#include "operations.h"

#include "src/common/constants.h"
#include "src/common/protocol.h"
#include "src/common/io.h"

#include <signal.h>

#define ERROR -1

//file descriptor para o qual o que tiver no buffer vai ser imprimido
int fdToPrint;

//buffer do que vai ser imprimido
char bufferToPrint[2000];

//maximo de backups a serem efectuados ao mesmo tempo.
int maxBackups;

//maximo de threads a processar os ficheiros ao mesmo tempo.
int maxThreads;

int _fdReq;
int _fdResp;
int _fdNotif;

pthread_rwlock_t rwlock;

void sigusr1_handler() {

  if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
    perror("Failed to set up SIGUSR1 handler again");
    exit(EXIT_FAILURE);
  }

  close(_fdResp);

  removeAllSubscriptions(rwlock);

}


void * processEachClient (void * _mensagemConnectRecebida) {

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
    perror("pthread_sigmask failed");
    exit(EXIT_FAILURE);
  }

  

  int subCounter;


  char* mensagemConnectRecebida = (char*)_mensagemConnectRecebida;

  

  char req_pipe_path[40];
  char resp_pipe_path[40];
  char notif_pipe_path[40];


  strncpy(req_pipe_path,mensagemConnectRecebida + 1, 40 );

  strncpy(resp_pipe_path,mensagemConnectRecebida + 41, 40 );

  strncpy(notif_pipe_path,mensagemConnectRecebida + 81, 40 );

  _fdReq = open(req_pipe_path, O_RDONLY);

  _fdResp = open(resp_pipe_path, O_WRONLY);

  write(_fdResp, "10", 2*sizeof(char));

  char clientID[32];

  strncpy(clientID,req_pipe_path + 8, 32 );

  for(;;)
  {
    char mensagemPedido [42];

    read(_fdReq, mensagemPedido, sizeof(mensagemPedido));

    if (atoi(&mensagemPedido[0]) == OP_CODE_DISCONNECT)
    { 
      write(_fdResp, "0", 1);

      close(_fdReq);
      close(_fdResp);
      
      break;
    }

    else if (atoi(&mensagemPedido[0]) == OP_CODE_SUBSCRIBE)
    {
      char keyToSubscribe [40];

      strncpy(keyToSubscribe,mensagemPedido + 1, 40 );

      if (keyExists (keyToSubscribe, rwlock) == 0) {

        write(_fdResp, "30", 2);
      }
      else {

        write(_fdResp, "31", 2);

        subscribeKey (keyToSubscribe, clientID, rwlock);

        subCounter++;
      }

    }

    else if (atoi(&mensagemPedido[0]) == OP_CODE_UNSUBSCRIBE)
    {
      char keyToUnsubscribe [41];
      strncpy(keyToUnsubscribe,mensagemPedido + 1, 41 );

      unsubscribeKey(keyToUnsubscribe, rwlock);

      subCounter--;
    }
  }

  return NULL;
}


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

//funçao que recebe o caminho do ficheiro a ser processado
//e executa os comandos escritos nesse ficheiro. 
//é executada pelas threads criadas.
void * processEachFile (void* _inputFilePath) {

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
    perror("pthread_sigmask failed");
    exit(EXIT_FAILURE);
  }

  //counter para o numero de backups efectuados por cada ficheiro
  int backupCounter = 0;

  char outputFilePath[MAX_JOB_FILE_NAME_SIZE];
  
  char *inputFilePath = (char*)_inputFilePath;


  int fdJobs = open(inputFilePath, O_RDONLY);
    if (fdJobs == ERROR){
    perror("Input file inválido");
    close(fdJobs);
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
    close(fdToPrint);

  }
  //ciclo que processa cada comando contido no ficheiro a ser processado
  int finish = 0;
  while (!finish) {
    char keys[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    char values[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    unsigned int delay;
    size_t num_pairs;

    
    fflush(stdout);

    switch (get_next(fdJobs)) {
      case CMD_WRITE:
        num_pairs = parse_write(fdJobs, keys, values, MAX_WRITE_SIZE, MAX_STRING_SIZE);
        if (num_pairs == 0) {
          writeToDotOut(bufferToPrint, fdToPrint);
          printf("Invalid command. See HELP for usage\n" );
          continue;
        }

        if (kvs_write(num_pairs, keys, values, bufferToPrint, fdToPrint, rwlock)) {
          snprintf(bufferToPrint, 2000, "Failed to write pair\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
        }

        break;

      case CMD_READ:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);

        if (num_pairs == 0) {
          snprintf(bufferToPrint, 2000, "Invalid command. See HELP for usage\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
          continue;
        }

        if (kvs_read(num_pairs, keys, bufferToPrint, fdToPrint, rwlock)) {
          snprintf(bufferToPrint, 2000, "Failed to write pair\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
        }
        break;

      case CMD_DELETE:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);

        if (num_pairs == 0) {
          snprintf(bufferToPrint, 2000, "Invalid command. See HELP for usage\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
          continue;
        }

        if (kvs_delete(num_pairs, keys, bufferToPrint, fdToPrint, rwlock)) {
          snprintf(bufferToPrint, 2000, "Failed to delete pair\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
        }
        break;

      case CMD_SHOW:

        kvs_show(bufferToPrint, fdToPrint, rwlock);
        break;

      case CMD_WAIT:
        if (parse_wait(fdJobs, &delay, NULL) == -1) {
          snprintf(bufferToPrint, 2000, "Invalid command. See HELP for usage\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
          continue;
        }

        if (delay > 0) {
          snprintf(bufferToPrint, 2000, "Waiting...\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
          kvs_wait(delay);
        }
        break;

      case CMD_BACKUP:

        backupCounter++;
        if (kvs_backup(inputFilePath, bufferToPrint, fdToPrint, backupCounter, maxBackups, rwlock)) {
          snprintf(bufferToPrint, 2000, "Failed to perform backup.\n" );
          writeToDotOut(bufferToPrint, fdToPrint);
        }
        break;

      case CMD_INVALID:
        snprintf(bufferToPrint, 2000, "Invalid command. See HELP for usage\n" );  
        writeToDotOut(bufferToPrint, fdToPrint);
        break;

      case CMD_HELP:
        
        snprintf(bufferToPrint, 2000, "Available commands:\nWRITE [(key,value)(key2,value2),...]\nREAD [key,key2,...]\nDELETE [key,key2,...]\nSHOW\nWAIT <delay_ms>\nBACKUP\nHELP\n" );
        writeToDotOut(bufferToPrint, fdToPrint); 


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

  memset(bufferToPrint, 0, 2000);

  free(_inputFilePath); 

  pthread_exit(0);

  return NULL;
}


int main(int argc, char *argv[]) {

  DIR *dir;
  struct dirent *entry;

  pthread_rwlock_init(&rwlock, NULL);

  if (kvs_init(bufferToPrint,fdToPrint)) {
    fprintf(stderr, "Failed to initialize KVS\n");
    return 1;
  }

  //verifica o numero de argumentos na linha de comandos
  if (argc != 5) {
    fprintf(stderr, "Invalid arguments. See HELP for usage\n");
    return 1;
  }

  if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
    perror("Falha ao configurar o tratador para SIGUSR1");
    exit(EXIT_FAILURE);
  }


  maxBackups = atoi (argv[2]);
  maxThreads = atoi (argv[3]);


  char mensagemConnectRecebida [40*3+1];

  int fdServidorRegisto;

  char fifoRegistoPath[100];

  strcpy(fifoRegistoPath, "/tmp/");  
  strcat(fifoRegistoPath, argv[4]);

  unlink(fifoRegistoPath);

  mkfifo(fifoRegistoPath, 0777);

  fdServidorRegisto = open(fifoRegistoPath, O_RDONLY);

  read (fdServidorRegisto, mensagemConnectRecebida , sizeof(mensagemConnectRecebida) );

  


  pthread_t threadToProcessClient;

  if (atoi(&mensagemConnectRecebida[0]) == OP_CODE_CONNECT) {

    pthread_create(&threadToProcessClient, NULL, &processEachClient, mensagemConnectRecebida);

  }

  //vector de threads disponiveis para processar os ficheiros
  pthread_t threads[maxThreads];

  //vector que verifica se as threads estao ocupadas
  //0:não ocupada,1:ocupada
  int vectorThreadsOcupadas[maxThreads];
  for (int i = 0; i < maxThreads; i++)
  {
    vectorThreadsOcupadas[i]= 0;
  }
  


  if((dir = opendir(argv[1])) == NULL){
    perror("Invalid directory");
    return ERROR;
  }

  //ciclo que processa cada ficheiro da directoria
   while ((entry = readdir(dir)) != NULL)
  {
    

    char * inputFilePath = malloc(sizeof(char)*MAX_JOB_FILE_NAME_SIZE);

    //salta as directorias "." e ".."
    if(strcmp(entry->d_name, ".")  == 0 || strcmp(entry->d_name, "..") == 0){
      free(inputFilePath);
      continue;
    }
      
    
    //verifica se o ficheiro a ser analisado tem ou não extensão .job
    else if (strstr(entry->d_name,".job") != NULL){


      strcpy(inputFilePath,argv[1]);  
      strcat(inputFilePath,"/" );
      strcat(inputFilePath, entry->d_name);


      //ciclo que verifica que threads estão ocupadas ou vazias
      //e cria a thread necessaria para processar o ficheiro
      for (int i = 0; i < maxThreads; i++)
      {
        if (vectorThreadsOcupadas[i] == 0)
        {
          pthread_create(&threads[i], NULL, &processEachFile , inputFilePath);
          vectorThreadsOcupadas[i] = 1;
          break;
          
        }
        else if ((vectorThreadsOcupadas[i] == 1) && (i == maxThreads - 1) ){
          
          pthread_join(threads[0], NULL);
          vectorThreadsOcupadas[0] = 0;

          pthread_create(&threads[0], NULL, &processEachFile , inputFilePath);
          vectorThreadsOcupadas[0] = 1;

          break;
          

        }
      
      }

    }
    else{
      free(inputFilePath);
    }
  }
  

  pthread_join(threadToProcessClient, NULL);

  //faz join a TODAS as threads activas
  for (int i = 0; i < maxThreads; i++)
  {
    if (vectorThreadsOcupadas[i] == 1)
    {
      pthread_join(threads[i], NULL);
      vectorThreadsOcupadas[i] = 0;
    }  
  
  }

  // espera que acabem TODOS os processos filhos do sistema
  while (wait (NULL) != -1 || errno != ECHILD)
  {
  }

  closedir(dir);

  pthread_rwlock_destroy(&rwlock);

  

  kvs_terminate(bufferToPrint,fdToPrint);

  return 0;

}
