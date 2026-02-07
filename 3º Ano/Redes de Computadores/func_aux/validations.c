#include "validations.h"

int is_valid_uid(char *uid){
    // Verificação do UID: tem que ter exatamente 6 digitos

    for(int i = 0; i < strlen(uid); i++){
        if(!isdigit(uid[i])){
            return 1;

        }
    }
    if (strlen(uid) == 6){
        return 0;
    }
    return 1;
}

int is_valid_password(const char *password){
    // verificação da password: tem que ter exatamente 8 caracteres alfanuméricos (letras e números)

    for(int i = 0; i < strlen(password); i++){
        if(!isalnum(password[i])){
            return 1;
        }
      
    }
    if (strlen(password) == 8){
        return 0;
    }
    return 1;
}

int is_valid_eid(char *eid){
    // Verificação do EID: tem que ter exatamente 3 digitos

    for(int i = 0; i < strlen(eid); i++){
        if(!isdigit(eid[i])){
            return 1;

        }
    }
    if (strlen(eid) == 3){
        return 0;
    }
    return 1;
}

int is_valid_nseats(int nseats){
    // Verificação do número de lugares: tem que ser entre 10 e 999

    if (nseats >= 10 && nseats <= 999){
        return 0;
    }
    return 1;
}

int is_valid_name(char *name) {
    // Verificação do nome: max 10 caracteres alfanuméricos
    int len = strlen(name);
    if (len == 0 || len > 10) {
        return 1;
    }
    for (int i = 0; i < len; i++) {
        if (!isalnum(name[i])) {
            return 1;
        }
    }
    return 0;
}