# Compilador e flags
CC = gcc
CFLAGS = -Wall -g -I./EUser -I./func_aux -I./EServer

# Diretórios
USER_DIR = EUser
AUX_DIR = func_aux
SERVER_DIR = EServer
# Ficheiros objeto do User
USER_OBJS = $(USER_DIR)/user.o \
            $(USER_DIR)/funcs.o \
            $(USER_DIR)/protocols.o \
            $(USER_DIR)/mensagens.o \
            $(AUX_DIR)/validations.o

# Ficheiros objeto do Server
SERVER_OBJS = $(SERVER_DIR)/ES.o \
			  $(SERVER_DIR)/funcs.o \
			  $(SERVER_DIR)/requests.o \
			  $(SERVER_DIR)/mensagens.o \
			  $(AUX_DIR)/validations.o

# Target principal: compila ambos os executáveis por defeito
all: user ES

# Compilar o executável User
user: $(USER_OBJS)
	$(CC) $(CFLAGS) -o user $(USER_OBJS)

# Compilar o executável Server
ES: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o ES $(SERVER_OBJS)

# Regras para compilar cada .c em .o
$(USER_DIR)/%.o: $(USER_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(AUX_DIR)/%.o: $(AUX_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar ficheiros compilados

clean:
	rm -f $(USER_DIR)/*.o $(AUX_DIR)/*.o $(SERVER_DIR)/*.o user ES
# Phony targets
.PHONY: all clean user ES