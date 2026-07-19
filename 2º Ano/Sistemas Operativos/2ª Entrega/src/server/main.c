#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pthread.h>
#include <semaphore.h>

#include "constants.h"
#include "parser.h"
#include "operations.h"
#include "main.h"

#include "src/common/protocol.h"
#include "src/common/io.h"
#include "src/common/constants.h"

#define ERROR -1
#define BUFFER_SIZE MAX_SESSION_COUNT

static char connectBuffer[BUFFER_SIZE][CONNECT_REQUEST_SIZE];
static int bufIn = 0;
static int bufOut = 0;
static sem_t bufFull; // nº de pedidos no buffer (init 0)
static sem_t bufEmpty; // nº de espaços livres (init BUFFER_SIZE)
static pthread_mutex_t bufMutex = PTHREAD_MUTEX_INITIALIZER;

static volatile sig_atomic_t sigusr1_received = 0;

// registo das sessões ativas: permite ao SIGUSR1 fechar os pipes de todos
typedef struct {
  int active;
  int resp_fd;
  int notif_fd;
} Session;
static Session sessions[MAX_SESSION_COUNT];
static pthread_mutex_t sessionsMutex = PTHREAD_MUTEX_INITIALIZER;

static void sigusr1_handler(int sig) {
  (void)sig;
  sigusr1_received = 1;
}

// regista uma sessão num slot livre; devolve o índice (há sempre slot: S gestoras)
static int register_session(int resp_fd, int notif_fd) {
  pthread_mutex_lock(&sessionsMutex);
  int idx = -1;
  for (int i = 0; i < MAX_SESSION_COUNT; i++) {
    if (!sessions[i].active) {
      sessions[i].active = 1;
      sessions[i].resp_fd = resp_fd;
      sessions[i].notif_fd = notif_fd;
      idx = i;
      break;
    }
  }
  pthread_mutex_unlock(&sessionsMutex);
  return idx;
}

int maxBackups; // nº máximo de backups concorrentes
int maxThreads; // nª máximo de tarefas a processar jobs

// ---- fila de trabalho dos ficheiros .job (worker pool, da Parte 1) ----
static char **jobFiles = NULL;
static int numJobs = 0;
static int nextJob = 0;
static pthread_mutex_t jobMutex = PTHREAD_MUTEX_INITIALIZER;

// escreve toda a string b no fd (trata escritas parciais)
int writeToDotOut(char *b, int fd) {
  size_t len = strlen(b);
  size_t done = 0;
  while (len > done) {
    ssize_t bytes_written = write(fd, b + done, len - done);
    if (bytes_written < 0) { perror("write error"); return 1; }
    done += (size_t)bytes_written;
  }
  return 0;
}

static int hasJobExtension(const char *name) {
  size_t len = strlen(name);
  return len > 4 && strcmp(name + len - 4, ".job") == 0;
}

// processa um ficheiro .job, escrevendo os resultados no .out correspondente
void processEachFile(char *inputFilePath) {
  int backupCounter = 0;
  char bufferToPrint[2000];
  int fdToPrint;
  char outputFilePath[MAX_JOB_FILE_NAME_SIZE];

  int fdJobs = open(inputFilePath, O_RDONLY);
  if (fdJobs == ERROR) { perror("Input file inválido"); return; }

  strcpy(outputFilePath, inputFilePath);
  char *dot = strrchr(outputFilePath, '.');
  if (dot != NULL) strcpy(dot, ".out");
  else strcat(outputFilePath, ".out");

  fdToPrint = open(outputFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fdToPrint == ERROR) { perror("Output file inválido"); close(fdJobs); return; }

  int finish = 0;
  while (!finish) {
    char keys[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    char values[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    unsigned int delay;
    size_t num_pairs;

    switch (get_next(fdJobs)) {
      case CMD_WRITE:
        num_pairs = parse_write(fdJobs, keys, values, MAX_WRITE_SIZE, MAX_STRING_SIZE);
        if (num_pairs == 0) { fprintf(stderr, "Invalid command. See HELP for usage\n"); continue; }
        if (kvs_write(num_pairs, keys, values)) fprintf(stderr, "Failed to write pair\n");
        break;
      case CMD_READ:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);
        if (num_pairs == 0) { fprintf(stderr, "Invalid command. See HELP for usage\n"); continue; }
        if (kvs_read(num_pairs, keys, bufferToPrint, fdToPrint)) fprintf(stderr, "Failed to read pair\n");
        break;
      case CMD_DELETE:
        num_pairs = parse_read_delete(fdJobs, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);
        if (num_pairs == 0) { fprintf(stderr, "Invalid command. See HELP of usage\n"); continue; }
        if (kvs_delete(num_pairs, keys, bufferToPrint, fdToPrint)) fprintf(stderr, "Failed to delete pair\n");
        break;
      case CMD_SHOW:
        kvs_show(bufferToPrint, fdToPrint);
        break;
      case CMD_WAIT:
        if (parse_wait(fdJobs, &delay, NULL) == -1) { fprintf(stderr, "Invalid command. See HELP for usage\n"); continue; }
        if (delay > 0) kvs_wait(delay);
        break;
      case CMD_BACKUP:
        backupCounter++;
        if (kvs_backup(inputFilePath, bufferToPrint, fdToPrint, backupCounter, maxBackups))
          fprintf(stderr, "Failed to perform backup.\n");
        break;
      case CMD_INVALID:
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        break;
      case CMD_HELP:
        printf("Available commands: \nWRITE\nREAD\nDELETE\nSHOW\nWAIT\nBACKUP\nHELP\n");
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

void *jobWorker(void *arg) {
  (void)arg;
  while (1) {
    pthread_mutex_lock(&jobMutex);
    if (nextJob >= numJobs) { pthread_mutex_unlock(&jobMutex); break; }
    char *path = jobFiles[nextJob++];
    pthread_mutex_unlock(&jobMutex);
    processEachFile(path);
    free(path);
  }
  return NULL;
}

// serve uma sessão de cliente do início ao fim (chamada por uma tarefa gestora)
static void serveSession(const char *connectMsg) {
  // extrai os 3 caminhos (40 bytes cada, já com padding '\0')
  char req_path[PIPE_PATH_SIZE + 1];
  char resp_path[PIPE_PATH_SIZE + 1];
  char notif_path[PIPE_PATH_SIZE + 1];
  memcpy(req_path, connectMsg + 1, PIPE_PATH_SIZE);
  memcpy(resp_path, connectMsg + 1 + PIPE_PATH_SIZE, PIPE_PATH_SIZE);
  memcpy(notif_path, connectMsg + 1 + 2 * PIPE_PATH_SIZE, PIPE_PATH_SIZE);
  req_path[PIPE_PATH_SIZE] = '\0';
  resp_path[PIPE_PATH_SIZE] = '\0';
  notif_path[PIPE_PATH_SIZE] = '\0';

  // abre pela MESMA ordem do cliente (req, resp, notif) para não haver deadlock
  int fdReq = open(req_path, O_RDONLY);
  int fdResp = open(resp_path, O_WRONLY);
  int fdNotif = open(notif_path, O_WRONLY);
  if (fdReq == -1 || fdResp == -1 || fdNotif == -1) {
    perror("open session pipes (server)");
    if (fdReq != -1) close(fdReq);
    if (fdResp != -1) close(fdResp);
    if (fdNotif != -1) close(fdNotif);
    return;
  }

  // resposta ao connect: [OP=1][result=0]
  char connResp[RESPONSE_SIZE] = { (char)OP_CODE_CONNECT, 0 };
  int slot = register_session(fdResp, fdNotif);
  write_all(fdResp, connResp, RESPONSE_SIZE);

  int active = 1;
  while (active) {
    char op;
    if (read_all(fdReq, &op, 1, NULL) != 1) break; // cliente fechou o pipe

    if (op == OP_CODE_DISCONNECT) {
      char r[RESPONSE_SIZE] = { (char)OP_CODE_DISCONNECT, 0 };
      write_all(fdResp, r, RESPONSE_SIZE);
      active = 0;
    
    } else if (op == OP_CODE_SUBSCRIBE || op == OP_CODE_UNSUBSCRIBE) {
      char key[KEY_SIZE];
      if (read_all(fdReq, key, KEY_SIZE, NULL) != 1) break;
      key[KEY_SIZE - 1] = '\0';

      char result;
      if (op == OP_CODE_SUBSCRIBE) result = (char)kvs_subscribe(key, fdNotif);
      else result = (char)kvs_unsubscribe(key, fdNotif);

      char r[RESPONSE_SIZE] = {op, result};
      write_all(fdResp, r, RESPONSE_SIZE);
    }
  }

  // fim da sessão: desregista. O SIGUSR1 pode já ter fechado resp/notif -
  // nesse caso (was_active==0) não os voltamos a fechar
  pthread_mutex_lock(&sessionsMutex);
  int was_active = (slot >= 0) ? sessions[slot].active : 0;
  if (slot >= 0) sessions[slot].active = 0;
  pthread_mutex_unlock(&sessionsMutex);

  kvs_remove_client(fdNotif);
  close(fdReq);
  if (was_active) {
    close(fdResp);
    close(fdNotif);
  }
 
}

// tarefa gestora: consome pedidos de connect do buffer e serve cada sessão
void *managerTask(void *arg) {
  (void)arg;
  while(1) {
    char connectMsg[CONNECT_REQUEST_SIZE];

    // consumidor: espera por um pedido e retira-o do buffer
    sem_wait(&bufFull);
    pthread_mutex_lock(&bufMutex);
    memcpy(connectMsg, connectBuffer[bufOut], CONNECT_REQUEST_SIZE);
    bufOut = (bufOut + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&bufMutex);
    sem_post(&bufEmpty);

    serveSession(connectMsg);
  }
  return NULL;
}

// ---- tarefa anfitriã: aceita pedidos de connect no FIFO de registo ----
void *hostTask(void *arg) {
  char *registerPath = (char *)arg;

  //abre o FIFO de registo sem bloquear no arranque, depois volta a bloqueante;
  //um fd de escrita "dummy" evita EOF quando um cliente fecha a sua ponta
  int fdRead = open(registerPath, O_RDONLY | O_NONBLOCK);
  if(fdRead == -1) { perror("open register (read)"); return NULL; }
  int fl = fcntl(fdRead, F_GETFL);
  fcntl(fdRead, F_SETFL, fl & ~O_NONBLOCK);
  int fdWriteDummy = open(registerPath, O_WRONLY);
  if (fdWriteDummy == -1) { perror("open register (write)"); close(fdRead); return NULL; }

  // só a anfitriã escuta o SIGUSR1
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  while(1) {
    int intr = 0;
    char connectMsg[CONNECT_REQUEST_SIZE];
    int r = read_all(fdRead, connectMsg, CONNECT_REQUEST_SIZE, &intr);

    if (sigusr1_received) {
      sigusr1_received = 0;
      // apaga todas as subscrições e fecha os pipes de todos os clientes
      kvs_delete_all_subscriptions();
      pthread_mutex_lock(&sessionsMutex);
      for (int i = 0; i < MAX_SESSION_COUNT; i++) {
        if (sessions[i].active) {
          close(sessions[i].resp_fd);
          close(sessions[i].notif_fd);
          sessions[i].active = 0;
        }
      }
      pthread_mutex_unlock(&sessionsMutex);
      continue;
    }

    if (r != 1) continue;
    if ((unsigned char)connectMsg[0] != OP_CODE_CONNECT) continue;

    sem_wait(&bufEmpty);
    pthread_mutex_lock(&bufMutex);
    memcpy(connectBuffer[bufIn], connectMsg, CONNECT_REQUEST_SIZE);
    bufIn = (bufIn + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&bufMutex);
    sem_post(&bufFull);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  //notificar um cliente que já morreu devolve EPIPE em vez de matar o servidor
  signal(SIGPIPE, SIG_IGN);

  // instala o handler do SIGUSR1 SEM SA_RESTART (para o read da anfitriã
  // ser interrompido) e bloqueia-o em main -> as threads criadas herdam-no
  // bloqueado: só a anfitriã o desbloqueia
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigusr1_handler;
  sigaction(SIGUSR1, &sa, NULL);

  sigset_t blockSet;
  sigemptyset(&blockSet);
  sigaddset(&blockSet, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &blockSet, NULL);

  if(argc != 5) {
    fprintf(stderr, "Usage: %s <dir_jobs> <max_threads> <backups_max> <register_fifo>\n", argv[0]);
    return 1;
  }

  if (kvs_init()) { fprintf(stderr, "Failed to initialize KVS\n"); return 1; }

  maxThreads = atoi(argv[2]);
  maxBackups = atoi(argv[3]);
  if (maxThreads < 1) maxThreads = 1;
  if (maxBackups < 1) maxBackups = 1;

  // cria o FIFO de registo em /tmp/<nome>
  char registerPath[128];
  snprintf(registerPath, sizeof(registerPath), "/tmp/%s", argv[4]);
  unlink(registerPath);
  if (mkfifo(registerPath, 0666) != 0) { perror("mkfifo register"); return 1; }

  // inicializa o buffer produtor-consumidor e lança as S tarefas gestoras
  sem_init(&bufFull, 0, 0);
  sem_init(&bufEmpty, 0, BUFFER_SIZE);
  pthread_t managers[MAX_SESSION_COUNT];
  for (int i = 0; i < MAX_SESSION_COUNT; i++)
    pthread_create(&managers[i], NULL, managerTask, NULL);

  // lança a tarefa anfitriã (serve clientes em paralelo com os jobs)
  pthread_t hostThread;
  pthread_create(&hostThread, NULL, hostTask, registerPath);

  //processa os ficheiros .job da diretoria (worker pool)
  DIR *dir = opendir(argv[1]);
  if (dir == NULL) {
    perror("Invalid directory");
  } else {
    struct dirent * entry;
    while ((entry = readdir(dir)) != NULL) {
      if (!hasJobExtension(entry->d_name)) continue;
      char *path = malloc(MAX_JOB_FILE_NAME_SIZE);
      int w = snprintf(path, MAX_JOB_FILE_NAME_SIZE, "%s/%s", argv[1], entry->d_name);
      if (w < 0 || w >= MAX_JOB_FILE_NAME_SIZE) { free(path); continue; }
      char **tmp = realloc(jobFiles, (size_t)(numJobs + 1) * sizeof(char *));
      if (tmp == NULL) { free(path); break; }
      jobFiles = tmp;
      jobFiles[numJobs++] = path;
    }
    closedir(dir);

    int numWorkers = (maxThreads < numJobs) ? maxThreads : numJobs;
    if (numWorkers > 0) {
      pthread_t workers[numWorkers];
      for (int i = 0; i < numWorkers; i++) pthread_create(&workers[i], NULL, jobWorker, NULL);
      for (int i = 0; i < numWorkers; i++) pthread_join(workers[i], NULL);
    }
    free(jobFiles);
  }

  //reaping dos filhos de backup
  while (wait(NULL) != -1 || errno != ECHILD) {}

  //o servidor fica vivo a servir clientes (a anfitriã não retorna nesta etapa)
  pthread_join(hostThread, NULL);

  kvs_terminate();
  return 0;
}