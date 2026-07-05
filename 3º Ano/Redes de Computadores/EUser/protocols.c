#include "protocols.h"

extern char *esip;
extern char *esport;
extern char last_eid[4];

// ==================== DEBUG ====================
#define DEBUG 0  // Mudar para 0 para desativar debug

// ==================== UDP ====================

int udp_pedido(char *request) {
    int sockfd;
    struct sockaddr_in server_addr;
    char response[MAX_BUFFER_SIZE];
    socklen_t addrlen;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating UDP socket");
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(esport));
    
    if (inet_pton(AF_INET, esip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        return -1;
    }
    
    // ===== DEBUG: Mostrar mensagem enviada =====
    if (DEBUG) {
        printf("\n[DEBUG UDP] ENVIADO -> %s", request);
    }
    
    if (sendto(sockfd, request, strlen(request), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error sending UDP request");
        close(sockfd);
        return -1;
    }
    
    addrlen = sizeof(server_addr);
    ssize_t n = recvfrom(sockfd, response, MAX_BUFFER_SIZE - 1, 0,
                        (struct sockaddr*)&server_addr, &addrlen);
    
    if (n == -1) {
        perror("Error receiving UDP response");
        close(sockfd);
        return -1;
    }
    
    response[n] = '\0';
    close(sockfd);
    
    // ===== DEBUG: Mostrar mensagem recebida =====
    if (DEBUG) {
        printf("[DEBUG UDP] RECEBIDO <- %s", response);
        if (response[strlen(response)-1] != '\n') printf("\n");
        printf("\n");
    }
    
    char cmd[4], status[4];
    if (sscanf(response, "%s %s", cmd, status) < 2) {
        printf("%s", Responses.ERR());
        return -1;
    }
    
    // LOGIN
    if (strcmp(cmd, "RLI") == 0) {
        if (strcmp(status, "OK") == 0) {
            printf("%s", Responses.RLI_OK());
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RLI_NOK());
            return 1;
        } else if (strcmp(status, "REG") == 0) {
            printf("%s", Responses.RLI_REG());
            return 0;
        }
    }
    
    // LOGOUT
    else if (strcmp(cmd, "RLO") == 0) {
        if (strcmp(status, "OK") == 0) {
            printf("%s", Responses.RLO_OK());
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RLO_NOK());
            return 1;
        } else if (strcmp(status, "UNR") == 0) {
            printf("%s", Responses.RLO_UNR());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RLO_WRP());
            return 1;
        }
    }
    
    // UNREGISTER
    else if (strcmp(cmd, "RUR") == 0) {
        if (strcmp(status, "OK") == 0) {
            printf("%s", Responses.RUR_OK());
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RUR_NOK());
            return 1;
        } else if (strcmp(status, "UNR") == 0) {
            printf("%s", Responses.RUR_UNR());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RUR_WRP());
            return 1;
        }
    }
    
    // MY EVENTS
    else if (strcmp(cmd, "RME") == 0) {
        if (strcmp(status, "OK") == 0) {
            char *events_list = handle_myevents(response);
            printf("%s", events_list);
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RME_NOK());
            return 0;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RME_NLG());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RME_WRP());
            return 1;
        }
    }
    
    // MY RESERVATIONS
    else if (strcmp(cmd, "RMR") == 0) {
        if (strcmp(status, "OK") == 0) {
            char *reservations_list = handle_myreservations(response);
            printf("%s", reservations_list);
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RMR_NOK());
            return 0;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RMR_NLG());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RMR_WRP());
            return 1;
        }
    }
    
    printf("%s", Responses.ERR());
    return -1;
}

// ==================== TCP ====================

int tcp_pedido(char *request, char *filename, int filesize) {
    int sockfd;
    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating TCP socket");
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(esport));
    
    if (inet_pton(AF_INET, esip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(sockfd);
        return -1;
    }
    
    // ===== DEBUG: Mostrar mensagem enviada =====
    if (DEBUG) {
        printf("\n[DEBUG TCP] ENVIADO -> %s", request);
        if (filename != NULL && filesize > 0) {
            printf("[ficheiro]\\n");
            printf("\n[DEBUG TCP] + FICHEIRO: %s (%d bytes)\n", filename, filesize);
        } else {
            printf("\n");
        }
    }
    
    // Enviar request (header)
    ssize_t total_sent = 0;
    ssize_t request_len = strlen(request);
    
    while (total_sent < request_len) {
        ssize_t sent = write(sockfd, request + total_sent, request_len - total_sent);
        if (sent == -1) {
            perror("Error sending TCP request");
            close(sockfd);
            return -1;
        }
        total_sent += sent;
    }
    
    // Se filename existe, enviar ficheiro (CREATE)
    if (filename != NULL && filesize > 0) {
        // Abrir ficheiro para enviar
        int file_fd = open(filename, O_RDONLY);
        if (file_fd == -1) {
            perror("Error opening file");
            close(sockfd);
            return -1;
        }
        
        // Enviar ficheiro usando sendfile (mais eficiente)
        off_t offset = 0;
        ssize_t remaining = filesize;
        while (remaining > 0) {
            ssize_t sent = sendfile(sockfd, file_fd, &offset, remaining);
            if (sent <= 0) {
                perror("Error sending file");
                close(file_fd);
                close(sockfd);
                return -1;
            }
            remaining -= sent;
        }
        
        close(file_fd);
        
        // IMPORTANTE: Enviar \n DEPOIS do ficheiro!
        if (write(sockfd, "\n", 1) == -1) {
            perror("Error sending newline");
            close(sockfd);
            return -1;
        }
    }
    
    // ========== RECEBER RESPOSTA ==========
    
    // Ler de forma robusta (o read pode devolver menos que o esperado):
    //  - respostas de texto terminam em '\n';
    //  - a resposta do SHOW (RSE OK) tem cabeçalho de texto + ficheiro binário,
    //    por isso lê-se o cabeçalho até ao 10º espaço (a seguir vêm os dados do ficheiro).
    char response[MAX_BUFFER_SIZE];
    ssize_t n = 0;
    int header_done = 0;

    while (!header_done && n < MAX_BUFFER_SIZE - 1) {
        ssize_t r = read(sockfd, response + n, MAX_BUFFER_SIZE - 1 - n);
        if (r <= 0) {
            close(sockfd);
            return -1;
        }
        n += r;
        response[n] = '\0';

        char cmd[8] = "", st[8] = "";
        sscanf(response, "%7s %7s", cmd, st);
        if (strcmp(cmd, "RSE") == 0 && strcmp(st, "OK") == 0) {
            // Cabeçalho: RSE OK uid name date time attendance reserved fname fsize <esp>
            int spaces = 0;
            for (ssize_t i = 0; i < n; i++) {
                if (response[i] == ' ' && ++spaces == 10) { header_done = 1; break; }
            }
        } else if (memchr(response, '\n', n) != NULL) {
            header_done = 1;  // resposta de texto completa
        } 
    }
    

    
    // ===== DEBUG: Mostrar mensagem recebida =====
    if (DEBUG) {
        // Para não mostrar dados binários, só mostramos até ao primeiro \n ou 200 chars
        char debug_response[201];
        int i;
        for (i = 0; i < 200 && i < n && response[i] != '\0'; i++) {
            if (response[i] == '\n') {
                debug_response[i] = '\n';
                i++;
                break;
            }
            // Substituir caracteres não imprimíveis por '.'
            if (response[i] >= 32 && response[i] < 127) {
                debug_response[i] = response[i];
            } else {
                debug_response[i] = '.';
            }
        }
        debug_response[i] = '\0';
        printf("[DEBUG TCP] RECEBIDO <- %s", debug_response);
        if (n > i) {
            printf("[DEBUG TCP] ... + %ld bytes adicionais (dados binários)\n", n - i);
        }
        printf("\n");
    }
    
    // Verificar se é SHOW com ficheiro
    char cmd[4], status[4];
    if (sscanf(response, "%s %s", cmd, status) >= 2) {
        if (strcmp(cmd, "RSE") == 0 && strcmp(status, "OK") == 0) {
            // RSE OK UID name date time attendance reserved fname fsize [dados]
            char uid[7], name[11], date[12], time[6];
            char recv_fname[25];
            int attendance, reserved, recv_fsize;
            
            int parsed = sscanf(response, "RSE OK %s %s %s %s %d %d %s %d",
                              uid, name, date, time, &attendance, &reserved, 
                              recv_fname, &recv_fsize);
            
            if (parsed != 8) {
                printf("Error parsing SHOW response\n");
                close(sockfd);
                return -1;
            }
            
            printf("Event: %s by %s on %s %s\n", name, uid, date, time);
            printf("Seats: %d/%d reserved\n", reserved, attendance);
            printf("Filename: %s Size: %d Bytes\n", recv_fname, recv_fsize);
            
            // Procurar onde começam os dados do ficheiro
            char search[50];
            sprintf(search, "%s %d ", recv_fname, recv_fsize);
            char *file_start = strstr(response, search);
            
            if (file_start) {
                file_start += strlen(search);
            } else {
                file_start = response + n;
            }
            
            int already_read = (response + n) - file_start;
            
            // Abrir ficheiro
            FILE *fp = fopen(recv_fname, "wb");
            if (fp == NULL) {
                perror("Error creating file");
                close(sockfd);
                return -1;
            }
            
            // Escrever bytes já lidos
            int received = 0;
            if (already_read > 0 && already_read <= recv_fsize) {
                fwrite(file_start, 1, already_read, fp);
                received = already_read;
            }
            
            // Ler o resto
            char temp[4096];
            while (received < recv_fsize) {
                int to_read = (recv_fsize - received < 4096) ? (recv_fsize - received) : 4096;
                ssize_t bytes_read = read(sockfd, temp, to_read);
                if (bytes_read <= 0) break;
                
                fwrite(temp, 1, bytes_read, fp);
                received += bytes_read;
            }
            
            fclose(fp);
            
            if (received == recv_fsize) {
                printf("File saved: %s (%d bytes)\n", recv_fname, recv_fsize);
            } else {
                printf("Warning: Incomplete file. Expected %d, got %d bytes\n", 
                       recv_fsize, received);
            }
            
            close(sockfd);
            return 0;
        }
    }
    
    close(sockfd);
    
    // ========== PROCESSAR OUTRAS RESPOSTAS ==========
    
    // Extrair \n se existir
    char *newline = strchr(response, '\n');
    if (newline) *newline = '\0';
    
    if (sscanf(response, "%s %s", cmd, status) < 2) {
        printf("%s", Responses.ERR());
        return -1;
    }
    
    // CREATE
    if (strcmp(cmd, "RCE") == 0) {
        if (strcmp(status, "OK") == 0) {
            char EID[4];
            sscanf(response, "%*s %*s %s", EID);
            printf("%s", Responses.RCE_OK(EID));
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RCE_NOK());
            return 1;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RCE_NLG());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RCE_WRP());
            return 1;
        } else if (strcmp(status, "ERR") == 0) {
            printf("%s", Responses.ERR());
            return 1;
        }
    }
    
    // CLOSE
    else if (strcmp(cmd, "RCL") == 0) {
        if (strcmp(status, "OK") == 0) {
            printf("Event Closed Successfully\n");
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RCL_NOK());
            return 1;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RCL_NLG());
            return 1;
        } else if (strcmp(status, "NOE") == 0) {
            printf("Event Does Not Exist\n");
            return 1;
        } else if (strcmp(status, "EOW") == 0) {
            printf("Event Not Owned By User\n");
            return 1;
        } else if (strcmp(status, "SLD") == 0) {
            printf("Event Sold Out\n");
            return 1;
        } else if (strcmp(status, "PST") == 0) {
            printf("Event Date Has Passed\n");
            return 1;
        } else if (strcmp(status, "CLO") == 0) {
            printf("Event Was Already Closed\n");
            return 1;
        }
    }
    
    // LIST
    else if (strcmp(cmd, "RLS") == 0) {
        if (strcmp(status, "OK") == 0) {
            char *events_list = handle_list(response);
            printf("%s", events_list);
            free(events_list);
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RLS_NOK());
            return 0;
        }
    }
    
    // SHOW (sem ficheiro - erro)
    else if (strcmp(cmd, "RSE") == 0) {
        if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RSE_NOK());
            return 1;
        }
    }
    
    // RESERVE
    else if (strcmp(cmd, "RRI") == 0) {
        if (strcmp(status, "ACC") == 0) {
            printf("Reservation Accepted\n");
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RRI_NOK());
            return 1;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RRI_NLG());
            return 1;
        } else if (strcmp(status, "CLS") == 0) {
            printf("%s", Responses.RRI_CLS());
            return 1;
        } else if (strcmp(status, "SLD") == 0) {
            printf("%s", Responses.RRI_SLD());
            return 1;
        } else if (strcmp(status, "REJ") == 0) {
            int remaining;
            sscanf(response, "%*s %*s %d", &remaining);
            printf("%s", Responses.RRI_REJ(remaining));
            return 1;
        } else if (strcmp(status, "PST") == 0) {
            printf("%s", Responses.RRI_PST());
            return 1;
        } else if (strcmp(status, "WRP") == 0) {
            printf("%s", Responses.RRI_WRP());
            return 1;
        }
    }
    
    // CHANGE PASSWORD
    else if (strcmp(cmd, "RCP") == 0) {
        if (strcmp(status, "OK") == 0) {
            printf("%s", Responses.RCP_OK());
            return 0;
        } else if (strcmp(status, "NOK") == 0) {
            printf("%s", Responses.RCP_NOK());
            return 1;
        } else if (strcmp(status, "NLG") == 0) {
            printf("%s", Responses.RCP_NLG());
            return 1;
        } else if (strcmp(status, "NID") == 0) {
            printf("%s", Responses.RCP_NID());
            return 1;
        }
    }
    
    printf("%s", Responses.ERR());
    return -1;
}

// ==================== HELPERS ====================

char *handle_myevents(char *response) {
    static char output[MAX_BUFFER_SIZE];
    output[0] = '\0';
    
    strcat(output, "My Events:\n");
    strcat(output, "EID  State\n");
    strcat(output, "------------\n");
    
    char *ptr = response + 7;
    char EID[4];
    int state;
    
    while (sscanf(ptr, "%s %d", EID, &state) == 2) {
        char line[50];
        char *state_str;
        
        switch(state) {
            case 0: state_str = "Past"; break;
            case 1: state_str = "Open"; break;
            case 2: state_str = "Sold Out"; break;
            case 3: state_str = "Closed"; break;
            default: state_str = "Unknown"; break;
        }
        
        sprintf(line, "%s  %s\n", EID, state_str);
        strcat(output, line);
        
        ptr = strstr(ptr, EID) + strlen(EID);
        while (*ptr == ' ') ptr++;
        ptr++;
        while (*ptr == ' ') ptr++;
    }
    
    return output;
}

char *handle_myreservations(char *response) {
    static char output[MAX_BUFFER_SIZE];
    output[0] = '\0';
    
    strcat(output, "My Reservations:\n");
    strcat(output, "EID  Date         Time      Seats\n");
    strcat(output, "----------------------------------------\n");
    
    char *ptr = response + 7;  // Pular "RMR OK "
    char EID[4], date[12], time[10];
    int seats;
    
    // Formato: EID dd-mm-yyyy hh:mm:ss seats
    while (sscanf(ptr, "%s %s %s %d", EID, date, time, &seats) == 4) {
        char line[100];
        sprintf(line, "%s  %s  %s  %d\n", EID, date, time, seats);
        strcat(output, line);
        
        // Avançar 4 campos
        for (int i = 0; i < 4; i++) {
            while (*ptr && *ptr == ' ') ptr++;
            while (*ptr && *ptr != ' ') ptr++;
        }
    }
    
    return output;
}

char *handle_list(char *response) {
    char eid[4], name[11], date[12], time[6];
    int state;
    
    response += strlen("RLS OK ");
    
    char *display = (char *)malloc(MAX_BUFFER_SIZE);
    if (display == NULL) exit(1);
    display[0] = '\0';
    
    sprintf(display, "All Events:\n");
    sprintf(display + strlen(display), "EID  Name       State      Date        Time\n");
    sprintf(display + strlen(display), "------------------------------------------------\n");
    
    // Formato: EID name state dd-mm-yyyy hh:mm
    while (sscanf(response, "%s %s %d %s %s", eid, name, &state, date, time) == 5) {
        char *state_str;
        switch (state) {
            case 0: state_str = "Past"; break;
            case 1: state_str = "Open"; break;
            case 2: state_str = "Sold Out"; break;
            case 3: state_str = "Closed"; break;
            default: state_str = "Unknown";
        }
        
        sprintf(display + strlen(display), "%-4s %-10s %-10s %s %s\n", 
                eid, name, state_str, date, time);
        
        // Avançar exatamente 5 tokens
        for (int i = 0; i < 5; i++) {
            while (*response && *response == ' ') response++;
            while (*response && *response != ' ') response++;
        }
    }
    
    return display;
}