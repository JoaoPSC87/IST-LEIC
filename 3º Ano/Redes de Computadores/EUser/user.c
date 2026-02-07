#include "user.h"

char *esip = "127.0.0.1";
char *esport = "58088";// o nosso grupo é o 88
char *INITIAL = "\ncommands:\n"
    " login [UID] [password]\n"
    " logout\n"
    " unregister\n"
    " exit\n"
    " changePass [oldPassword] [newPassword]\n"
    " create [name] [event_fname] [event_date] [num_attendees]\n"
    " close [EID]\n"
    " myevents or mye\n"
    " list\n"
    " show [EID]\n"
    " reserve [EID] [seats]\n"
    " myreservations or myr\n";



int main(int argc, char **argv) {
    int opt;
    char buffer[MAX_BUFFER_SIZE];
    char command[20];
    
    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                esip = optarg;
                break;
            case 'p':
                esport = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-n ESIP] [-p ESport]\n", argv[0]);
                exit(EXIT_FAILURE);  // erro de argumentos
        }
    }
    
    printf("%s", INITIAL); // mostra as instruções iniciais
    
    while (1) {
        printf("> ");
        fflush(stdout);  // garante que o prompt aparece antes do fgets
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;  // trata Ctrl+D (EOF)
        }
        
        if (sscanf(buffer, "%s", command) != 1) {
            continue;  // linha vazia, ignora
        }
        
        executeCommand(command, buffer);
    }
    
    return 0;
}