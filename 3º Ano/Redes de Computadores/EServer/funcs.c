#include "funcs.h"
#include <time.h>

// ==================== UTILITY FUNCTIONS ====================

bool is_directory(const char *path) {
    struct stat sa;
    return stat(path, &sa) == 0 && S_ISDIR(sa.st_mode);
}

int is_directory_empty(char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return -1; // Error
    }
    struct dirent *entry;
    int empty = 1; // Assume empty
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            empty = 0; break;
        }
    }
    closedir(dir);
    return empty;
}

// ==================== USER MANAGEMENT ====================

bool user_is_registered(char *UID) {
    char pass_file[PATH_SIZE];
    snprintf(pass_file, sizeof(pass_file), "%s/%s/%s_pass.txt", SERVER_DIR_USERS, UID, UID);
    return access(pass_file, F_OK) != -1;
}

bool user_is_logged(char *UID) {
    char login_file[PATH_SIZE];
    snprintf(login_file, sizeof(login_file), "%s/%s/%s_login.txt", SERVER_DIR_USERS, UID, UID);
    return access(login_file, F_OK) != -1;
}

bool check_user_pass(char *UID, char *password) {
    char pass_file[PATH_SIZE];
    snprintf(pass_file, sizeof(pass_file), "%s/%s/%s_pass.txt", SERVER_DIR_USERS, UID, UID);
    FILE *f = fopen(pass_file, "r");
    if (f == NULL) return false;
    char stored_pass[9];
    if (fscanf(f, "%s", stored_pass) != 1) { fclose(f); return false; }
    fclose(f);
    return strcmp(password, stored_pass) == 0;
}


void register_user(char *UID, char *password) {
    char user_dir[PATH_SIZE];
    snprintf(user_dir, sizeof(user_dir), "%s/%s", SERVER_DIR_USERS, UID);
    mkdir(user_dir, 0700);
    
    // Criar CREATED
    char created_dir[PATH_SIZE];
    snprintf(created_dir, sizeof(created_dir), "%s/CREATED", user_dir);
    mkdir(created_dir, 0700);
    
    // Criar RESERVED
    char reserved_dir[PATH_SIZE];
    snprintf(reserved_dir, sizeof(reserved_dir), "%s/RESERVED", user_dir);
    mkdir(reserved_dir, 0700);
    
    // Criar ficheiro de password
    char pass_file[PATH_SIZE];
    snprintf(pass_file, sizeof(pass_file), "%s/%s_pass.txt", user_dir, UID);
    FILE *f = fopen(pass_file, "w");
    if (f != NULL) { fprintf(f, "%s", password); fclose(f); }
}

void log_user_in(char *UID) {
    char login_file[PATH_SIZE];
    snprintf(login_file, sizeof(login_file), "%s/%s/%s_login.txt", SERVER_DIR_USERS, UID, UID);
    FILE *f = fopen(login_file, "w");
    if (f != NULL) { fprintf(f, "logged_in"); fclose(f); }
}

void log_user_out(char *UID) {
    char login_file[PATH_SIZE];
    snprintf(login_file, sizeof(login_file), "%s/%s/%s_login.txt", SERVER_DIR_USERS, UID, UID);
    unlink(login_file);
}

void unregister_user(char *UID) {
    char user_dir[PATH_SIZE];
    snprintf(user_dir, sizeof(user_dir), "%s/%s", SERVER_DIR_USERS, UID);
    char pass_file[PATH_SIZE];
    snprintf(pass_file, sizeof(pass_file), "%s/%s_pass.txt", user_dir, UID);
    char login_file[PATH_SIZE];
    snprintf(login_file, sizeof(login_file), "%s/%s_login.txt", user_dir, UID);
    unlink(pass_file);
    unlink(login_file);
    
}

void change_password(char *UID, char *new_password) {
    register_user(UID, new_password);
}

// ==================== EVENT MANAGEMENT ====================

bool event_exists(char *EID) {
    char event_dir[PATH_SIZE];
    snprintf(event_dir, sizeof(event_dir), "%s/%s", SERVER_DIR_EVENTS, EID);
    return access(event_dir, F_OK) != -1;
}

bool user_owns_event(char *UID, char *EID) {
    char start_file[PATH_SIZE];
    snprintf(start_file, sizeof(start_file), "%s/%s/START_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    FILE *f = fopen(start_file, "r");
    if (f == NULL) return false;
    char owner_uid[7];
    if (fscanf(f, "%s", owner_uid) != 1) { fclose(f); return false; }
    fclose(f);
    return strcmp(UID, owner_uid) == 0;
}

bool event_is_closed(char *EID) {
    char end_file[PATH_SIZE];
    snprintf(end_file, sizeof(end_file), "%s/%s/END_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    return access(end_file, F_OK) != -1;
}

bool event_is_past(char *EID) {
    char start_file[PATH_SIZE];
    snprintf(start_file, sizeof(start_file), "%s/%s/START_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    FILE *f = fopen(start_file, "r");
    if (f == NULL) return false;
    char date_str[11], time_str[6];
    fscanf(f, "%*s %*s %*s %*d %s %s", date_str, time_str);
    fclose(f);
    struct tm tm_evt = {0};
    int day, month, year, hour, min;
    sscanf(date_str, "%d-%d-%d", &day, &month, &year);
    sscanf(time_str, "%d:%d", &hour, &min);
    tm_evt.tm_mday = day; tm_evt.tm_mon = month - 1; tm_evt.tm_year = year - 1900;
    tm_evt.tm_hour = hour; tm_evt.tm_min = min; tm_evt.tm_isdst = -1;
    time_t evt_time = mktime(&tm_evt);
    time_t now = time(NULL);
    return difftime(now, evt_time) > 0;
}

int get_remaining_seats(char *EID) {
    char start_file[PATH_SIZE];
    snprintf(start_file, sizeof(start_file), "%s/%s/START_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    FILE *f_start = fopen(start_file, "r");
    if (f_start == NULL) return 0;
    int capacity;
    fscanf(f_start, "%*s %*s %*s %d %*s %*s", &capacity);
    fclose(f_start);

    char res_file[PATH_SIZE];
    snprintf(res_file, sizeof(res_file), "%s/%s/RES_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    int reserved = 0;
    FILE *f_res = fopen(res_file, "r");
    if (f_res != NULL) {
        fscanf(f_res, "%d", &reserved);
        fclose(f_res);
    }
    return capacity - reserved;
}

bool event_is_sold_out(char *EID) {
    return get_remaining_seats(EID) <= 0;
}

int get_event_state(char *EID) {
    if (event_is_closed(EID)) return 3;
    if (event_is_past(EID)) return 0;
    if (event_is_sold_out(EID)) return 2;
    return 1;
}

void close_event_now(char *EID) {
    char end_file[PATH_SIZE];
    snprintf(end_file, sizeof(end_file), "%s/%s/END_%s.txt", SERVER_DIR_EVENTS, EID, EID);
    FILE *f = fopen(end_file, "w");
    if (f != NULL) fclose(f);
}

char *get_next_eid() {
    DIR *d;
    struct dirent *dir;
    int max_value = 0;
    d = opendir(SERVER_DIR_EVENTS);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                if (is_valid_eid(dir->d_name) == 0) {
                    int val = atoi(dir->d_name);
                    if (val > max_value) max_value = val;
                }
            }
        }
        closedir(d);
    }
    if (max_value >= 999) return NULL;
    char *eid = malloc(5 * sizeof(char));
    if (eid == NULL) return NULL;
    snprintf(eid, 5, "%03d", max_value + 1);
    return eid;
}


int create_event(int tcp_fd, char *UID, char *EID, char *name, char *event_date, 
                 int attendance_size, char *fname, int fsize) {
    // 1. Criar diretório do evento
    char event_dir[PATH_SIZE];
    snprintf(event_dir, sizeof(event_dir), "%s/%s", SERVER_DIR_EVENTS, EID);
    if (mkdir(event_dir, 0700) != 0) return -1;
    
    // 2. Criar DESCRIPTION
    char desc_dir[PATH_SIZE];
    snprintf(desc_dir, sizeof(desc_dir), "%s/DESCRIPTION", event_dir);
    if (mkdir(desc_dir, 0700) != 0) {
        rmdir(event_dir);
        return -1;
    }
    
    // 3. Criar RESERVATIONS
    char reserv_dir[PATH_SIZE];
    snprintf(reserv_dir, sizeof(reserv_dir), "%s/RESERVATIONS", event_dir);
    if (mkdir(reserv_dir, 0700) != 0) {
        rmdir(desc_dir);
        rmdir(event_dir);
        return -1;
    }
    
    // 4. Criar START_EID.txt
    char start_file[PATH_SIZE];
    snprintf(start_file, sizeof(start_file), "%s/START_%s.txt", event_dir, EID);
    FILE *f = fopen(start_file, "w");
    if (f == NULL) {
        rmdir(reserv_dir);
        rmdir(desc_dir);
        rmdir(event_dir);
        return -1;
    }
    fprintf(f, "%s %s %s %d %s", UID, name, fname, attendance_size, event_date);
    fclose(f);
    
    // 5. Criar RES_EID.txt (nome correto!)
    char res_file[PATH_SIZE];
    snprintf(res_file, sizeof(res_file), "%s/RES_%s.txt", event_dir, EID);
    f = fopen(res_file, "w");
    if (f != NULL) { fprintf(f, "0"); fclose(f); }

    // 6. Receber e guardar ficheiro em DESCRIPTION/
    char file_path[PATH_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/DESCRIPTION/%s", event_dir, fname);
    FILE *fp = fopen(file_path, "wb");
    if (fp == NULL) return -1;
    
    char buffer[4096];
    int remaining = fsize;
    while (remaining > 0) {
        int to_read = (remaining < 4096) ? remaining : 4096;
        int n = recv(tcp_fd, buffer, to_read, 0); 
        if (n <= 0) break;
        fwrite(buffer, 1, n, fp);
        remaining -= n;
    }
    fclose(fp);
    
    if (remaining != 0) return -1;
    
    // NOVO: Ler e descartar o \n final
    char newline;
    recv(tcp_fd, &newline, 1, 0);
    
    // 7. Criar entrada em USERS/UID/CREATED/
    char created_file[PATH_SIZE];
    snprintf(created_file, sizeof(created_file), "%s/%s/CREATED/%s.txt", 
             SERVER_DIR_USERS, UID, EID);
    f = fopen(created_file, "w");
    if (f != NULL) {
        fprintf(f, "Created\n");
        fclose(f);
    }
    
    return 0;
}

// ==================== RESERVATIONS ====================


void make_reservation(char *UID, char *EID, int num_seats) {
    // 1. Obter timestamp atual
    time_t now = time(NULL);
    struct tm *tm_now = gmtime(&now);
    
    char date_str[11];  // YYYY-MM-DD
    char time_str[7];   // HHMMSS
    char datetime_str[20]; // DD-MM-YYYY HH:MM:SS
    
    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday);
    snprintf(time_str, sizeof(time_str), "%02d%02d%02d",
             tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    snprintf(datetime_str, sizeof(datetime_str), "%02d-%02d-%04d %02d:%02d:%02d",
             tm_now->tm_mday, tm_now->tm_mon + 1, tm_now->tm_year + 1900,
             tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    
    // 2. Atualizar contador total em RES_EID.txt
    char res_file[PATH_SIZE];
    snprintf(res_file, sizeof(res_file), "%s/%s/RES_%s.txt", 
             SERVER_DIR_EVENTS, EID, EID);
    
    int current_reserved = 0;
    FILE *f_res = fopen(res_file, "r");
    if (f_res != NULL) { 
        fscanf(f_res, "%d", &current_reserved); 
        fclose(f_res); 
    }
    
    f_res = fopen(res_file, "w");
    if (f_res != NULL) { 
        fprintf(f_res, "%d", current_reserved + num_seats); 
        fclose(f_res); 
    }
    
    // 3. Criar ficheiro de reserva em EVENTS/EID/RESERVATIONS/
    char reserv_filename[50];
    snprintf(reserv_filename, sizeof(reserv_filename), 
             "R-%s-%s %s.txt", UID, date_str, time_str);
    
    char reserv_file[PATH_SIZE];
    snprintf(reserv_file, sizeof(reserv_file), 
             "%s/%s/RESERVATIONS/%s", SERVER_DIR_EVENTS, EID, reserv_filename);
    
    FILE *f = fopen(reserv_file, "w");
    if (f != NULL) {
        fprintf(f, "%s %d %s\n", UID, num_seats, datetime_str);
        fclose(f);
    }
    
    // 4. Duplicar em USERS/UID/RESERVED/
    char user_reserv_file[PATH_SIZE];
    snprintf(user_reserv_file, sizeof(user_reserv_file),
             "%s/%s/RESERVED/%s", SERVER_DIR_USERS, UID, reserv_filename);
    
    f = fopen(user_reserv_file, "w");
    if (f != NULL) {
        fprintf(f, "%s %d %s\n", UID, num_seats, datetime_str);
        fclose(f);
    }
}


bool user_reservations_empty(char *UID) {
    DIR *d = opendir(SERVER_DIR_EVENTS);
    if (!d) return true;
    struct dirent *dir;
    bool found = false;
    
    while ((dir = readdir(d)) != NULL) {
        if (is_valid_eid(dir->d_name) == 0) {
            char reserv_dir[PATH_SIZE];
            snprintf(reserv_dir, sizeof(reserv_dir), "%s/%s/RESERVATIONS", 
                     SERVER_DIR_EVENTS, dir->d_name);
            
            DIR *rd = opendir(reserv_dir);
            if (rd) {
                struct dirent *rfile;
                while ((rfile = readdir(rd)) != NULL) {
                    // Procurar ficheiros R-UID-*
                    if (strncmp(rfile->d_name, "R-", 2) == 0) {
                        char check_uid[7];
                        sscanf(rfile->d_name, "R-%6[^-]", check_uid);
                        if (strcmp(check_uid, UID) == 0) {
                            found = true;
                            break;
                        }
                    }
                }
                closedir(rd);
            }
        }
        if (found) break;
    }
    closedir(d);
    return !found;
}

void get_user_reservations(char *UID, char *response) {
    DIR *d = opendir(SERVER_DIR_EVENTS);
    if (!d) return;
    response[0] = '\0';
    struct dirent *dir;
    
    while ((dir = readdir(d)) != NULL) {
        if (is_valid_eid(dir->d_name) == 0) {
            char reserv_dir[PATH_SIZE];
            snprintf(reserv_dir, sizeof(reserv_dir), "%s/%s/RESERVATIONS", 
                     SERVER_DIR_EVENTS, dir->d_name);
            
            DIR *rd = opendir(reserv_dir);
            if (rd) {
                struct dirent *rfile;
                while ((rfile = readdir(rd)) != NULL) {
                    if (strncmp(rfile->d_name, "R-", 2) == 0) {
                        char check_uid[7];
                        sscanf(rfile->d_name, "R-%6[^-]", check_uid);
                        if (strcmp(check_uid, UID) == 0) {
                            // Ler o ficheiro para obter o número de assentos E a data
                            char rfile_path[PATH_SIZE];
                            snprintf(rfile_path, sizeof(rfile_path), "%s/%s", 
                                     reserv_dir, rfile->d_name);
                            FILE *f = fopen(rfile_path, "r");
                            if (f) {
                                char r_uid[7];
                                int seats;
                                char date[11], time_str[9];  // dd-mm-yyyy e hh:mm:ss
                                
                                // Formato do ficheiro: UID seats dd-mm-yyyy hh:mm:ss
                                if (fscanf(f, "%s %d %s %s", r_uid, &seats, date, time_str) == 4) {
                                    char entry[60];
                                    // Formato RMR: EID date time value
                                    snprintf(entry, sizeof(entry), "%s %s %s %d ", dir->d_name, date, time_str, seats);
                                    strcat(response, entry);
                                }
                                fclose(f);
                            }
                        }
                    }
                }
                closedir(rd);
            }
        }
    }
    closedir(d);
}

// ==================== LISTING FUNCTIONS ====================

bool no_events_exist() { return is_directory_empty(SERVER_DIR_EVENTS) == 1; }

void get_all_events(char *response) {
    DIR *d = opendir(SERVER_DIR_EVENTS);
    if (!d) return;
    response[0] = '\0';
    struct dirent *dir;
    
    while ((dir = readdir(d)) != NULL) {
        if (is_valid_eid(dir->d_name) == 0) {
            // Ler START file para obter name e date
            char start_file[PATH_SIZE];
            snprintf(start_file, sizeof(start_file), 
                     "%s/%s/START_%s.txt", SERVER_DIR_EVENTS, dir->d_name, dir->d_name);
            
            FILE *f = fopen(start_file, "r");
            if (f) {
                char uid[7], name[11], fname[25], date[11], time_str[6];
                int attendance;
                fscanf(f, "%s %s %s %d %s %s", uid, name, fname, &attendance, date, time_str);
                fclose(f);
                
                int state = get_event_state(dir->d_name);
                char entry[60];
                snprintf(entry, sizeof(entry), "%s %s %d %s %s ", dir->d_name, name, state, date, time_str);
                strcat(response, entry);
            }
        }
    }
    closedir(d);
}

void get_user_events(char *UID, char *response) {
    DIR *d = opendir(SERVER_DIR_EVENTS);
    if (!d) return;
    response[0] = '\0';
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if (is_valid_eid(dir->d_name) == 0) {
            if (user_owns_event(UID, dir->d_name)) {
                int state = get_event_state(dir->d_name);
                char entry[20];
                snprintf(entry, sizeof(entry), "%s %d ", dir->d_name, state);
                strcat(response, entry);
            }
        }
    }
    closedir(d);
}

bool user_events_empty(char *UID) {
    DIR *d = opendir(SERVER_DIR_EVENTS);
    if (!d) return true;
    struct dirent *dir;
    bool found = false;
    while ((dir = readdir(d)) != NULL) {
        if (is_valid_eid(dir->d_name) == 0) {
            if (user_owns_event(UID, dir->d_name)) { found = true; break; }
        }
    }
    closedir(d);
    return !found;
}

void check_and_close_if_past(char *EID) {
    if (event_is_past(EID)) close_event_now(EID);
}


int get_event_details(char *EID, char *response, char *filename, int *filesize) {
    if (!event_exists(EID)) return -1;
    
    char desc_dir[PATH_SIZE];
    snprintf(desc_dir, sizeof(desc_dir), "%s/%s", SERVER_DIR_EVENTS, EID);
    
    char start_file[PATH_SIZE];
    snprintf(start_file, sizeof(start_file), "%s/START_%s.txt", desc_dir, EID);
    
    FILE *f_start = fopen(start_file, "r");
    if (f_start == NULL) return -1;
    
    char uid[7], name[11], fname[25], date[11], time_str[6];
    int attendance;
    if (fscanf(f_start, "%s %s %s %d %s %s", uid, name, fname, &attendance, date, time_str) != 6) {
        fclose(f_start);
        return -1;
    }
    fclose(f_start);
    

    char res_file[PATH_SIZE];
    snprintf(res_file, sizeof(res_file), "%s/RES_%s.txt", desc_dir, EID);
    FILE *f_res = fopen(res_file, "r");
    int reserved = 0;
    if (f_res != NULL) { 
        fscanf(f_res, "%d", &reserved); 
        fclose(f_res); 
    }
    

    char desc_file[PATH_SIZE];
    snprintf(desc_file, sizeof(desc_file), "%s/DESCRIPTION/%s", desc_dir, fname);
    
    struct stat st;
    if (stat(desc_file, &st) != 0) return -1;
    
    *filesize = st.st_size;
    strcpy(filename, fname);
    
    sprintf(response, "%s %s %s %s %d %d %s %d", 
            uid, name, date, time_str, attendance, reserved, fname, *filesize);
    
    return 0;
}

void send_event_file(int fd, char *EID, char *filename, int filesize) {
    char desc_file[PATH_SIZE];
    sprintf(desc_file, "%s/%s/DESCRIPTION/%s", SERVER_DIR_EVENTS, EID, filename);
    
    int file_fd = open(desc_file, O_RDONLY);
    if (file_fd == -1) {
        perror("Error opening event file");
        return;
    }
    
    char buffer[4096];
    int total_sent = 0;
    
    while (total_sent < filesize) {
        int to_read = (filesize - total_sent < 4096) ? (filesize - total_sent) : 4096;
        int n = read(file_fd, buffer, to_read);
        
        if (n <= 0) break;
        
        int sent = 0;
        while (sent < n) {
            ssize_t s = write(fd, buffer + sent, n - sent);
            if (s == -1) {
                perror("Error sending file");
                close(file_fd);
                return;
            }
            sent += s;
        }
        
        total_sent += n;
    }
    
    close(file_fd);
}

void check_server_dirs() {
    struct stat st = {0};
    if (stat(SERVER_DIR_USERS, &st) == -1) mkdir(SERVER_DIR_USERS, 0700);
    if (stat(SERVER_DIR_EVENTS, &st) == -1) mkdir(SERVER_DIR_EVENTS, 0700);
}