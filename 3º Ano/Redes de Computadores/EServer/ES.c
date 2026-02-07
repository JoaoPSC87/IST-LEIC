#include "ES.h"
#include <netinet/tcp.h>

bool verbose = false;
char *esport = "58088";  // 58000 + GN (grupo 88)

int main(int argc, char **argv) {
    int opt, port;
    char buffer[MAX_BUFFER_SIZE];
    char command[4];

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "vp:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'p':
                esport = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-p ESport] [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    port = atoi(esport);
    check_server_dirs();
    
    // Create UDP socket
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }

    // Create TCP socket
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        perror("Error creating TCP socket");
        close(udp_socket);
        exit(EXIT_FAILURE);
    }

    // Set up server address structures for both UDP and TCP
    struct sockaddr_in udp_server_address, tcp_server_address;
    memset(&udp_server_address, 0, sizeof(udp_server_address));
    memset(&tcp_server_address, 0, sizeof(tcp_server_address));

    udp_server_address.sin_family = AF_INET;
    udp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_server_address.sin_port = htons(port);

    tcp_server_address.sin_family = AF_INET;
    tcp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_server_address.sin_port = htons(port);

    // Bind UDP socket
    if (bind(udp_socket, (struct sockaddr*)&udp_server_address, sizeof(udp_server_address)) == -1) {
        perror("Error binding UDP socket");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    // Bind TCP socket
    if (bind(tcp_socket, (struct sockaddr*)&tcp_server_address, sizeof(tcp_server_address)) == -1) {
        perror("Error binding TCP socket");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming TCP connections
    if (listen(tcp_socket, 5) == -1) {
        perror("Error listening on TCP socket");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    printf("ES Server is running on port %d...\n", port);
    if (verbose) {
        printf("Verbose mode enabled\n");
    }

    // Main server loop with select()
    while (1) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(udp_socket, &read_fds);
        FD_SET(tcp_socket, &read_fds);

        int max_fd = (udp_socket > tcp_socket) ? udp_socket : tcp_socket;

        // Wait for activity on either UDP or TCP socket
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Error in select");
            continue;
        }

        // Handle UDP requests
        if (FD_ISSET(udp_socket, &read_fds)) {
            struct sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);
            
            ssize_t bytes_received = recvfrom(udp_socket, buffer, MAX_BUFFER_SIZE - 1, 0,
                                             (struct sockaddr*)&client_address, &client_address_len);

            if (bytes_received == -1) {
                perror("Error receiving UDP data");
                continue;
            }

            buffer[bytes_received] = '\0';

            if (verbose) {
                printf("UDP from %s:%d - %s", 
                       inet_ntoa(client_address.sin_addr),
                       ntohs(client_address.sin_port),
                       buffer);
            }

            // Extract command (first 3 characters)
            sscanf(buffer, "%3s", command);
            
            // Process UDP request
            handle_udp_request(udp_socket, client_address, command, buffer);
        }

        // Handle TCP requests
        if (FD_ISSET(tcp_socket, &read_fds)) {
            struct sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);
    
            int client_socket = accept(tcp_socket, (struct sockaddr*)&client_address, &client_address_len);
            if (client_socket == -1) {
                perror("Error accepting TCP connection");
                continue;
            }
    
            int flag = 1;
            setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

            if (verbose) {
                printf("TCP connection from %s:%d\n",
                    inet_ntoa(client_address.sin_addr),
                    ntohs(client_address.sin_port));
            }

            // ========== LEITURA TCP OTIMIZADA ==========
            int total_received = 0;
            int header_complete = 0;
            int spaces_count = 0;
    
            // Primeiro, ler o comando (3 caracteres)
            while (total_received < 3) {
                ssize_t bytes_received = recv(client_socket, buffer + total_received, 3 - total_received, 0);
                if (bytes_received <= 0) {
                    close(client_socket);
                    goto next_connection;
                }
                total_received += bytes_received;
            }
            buffer[3] = '\0';
            strncpy(command, buffer, 3);
            command[3] = '\0';
            
            // Para CRE, ler de forma especial (não procurar \n no header)
            if (strcmp(command, "CRE") == 0) {
                // Formato: CRE UID password name date time attendance fname fsize [fdata]\n
                // Precisamos ler até ao espaço DEPOIS de fsize (9 espaços no total)
                // CRE UID password name date time attendance fname fsize 
                //    1   2        3    4    5    6          7     8     9
                
                while (spaces_count < 9 && total_received < MAX_BUFFER_SIZE - 1) {
                    ssize_t bytes_received = recv(client_socket, buffer + total_received, 1, 0);
                    if (bytes_received <= 0) {
                        close(client_socket);
                        goto next_connection;
                    }
                    if (buffer[total_received] == ' ') {
                        spaces_count++;
                    }
                    total_received++;
                }
                // Remover o último espaço do buffer para o parsing
                buffer[total_received - 1] = '\0';
                header_complete = 1;
            } else {
                // Para outros comandos, ler até \n
                while (!header_complete && total_received < MAX_BUFFER_SIZE - 1) {
                    int space_left = MAX_BUFFER_SIZE - 1 - total_received;
                    int to_read = (space_left < 1024) ? space_left : 1024;
            
                    ssize_t bytes_received = recv(client_socket, buffer + total_received, to_read, 0);
            
                    if (bytes_received <= 0) {
                        close(client_socket);
                        goto next_connection;
                    }
            
                    total_received += bytes_received;
            
                    // Procurar \n no buffer
                    for (int i = total_received - bytes_received; i < total_received; i++) {
                        if (buffer[i] == '\n') {
                            buffer[i] = '\0';
                            header_complete = 1;
                            break;
                        }
                    }
                }
            }
    
            if (!header_complete && strcmp(command, "CRE") != 0) {
                close(client_socket);
                goto next_connection;
            }

            if (verbose) {
                printf("TCP message: %s\n", buffer);
            }

            // Process TCP request
            handle_tcp_request(client_socket, command, buffer);
            close(client_socket);
    
            next_connection:
            continue;
        }
    }

    // Cleanup (never reached in normal operation)
    close(udp_socket);
    close(tcp_socket);

    return 0;
}