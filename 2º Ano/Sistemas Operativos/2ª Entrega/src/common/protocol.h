#ifndef COMMON_PROTOCOL_H
#define COMMON_PROTOCOL_H

// ---- Protocolo cliente-servidor do IST-KVS (mensagens binárias) ----
// Regras (do enunciado):
//  - cada mensagem de pedido começa por 1 byte de OP_CODE (valor numérico, NÃO o caráter '1');
//  - os campos de texto têm tamanho fixo e são preenchidos com '\0' (padding).

// Tamanhos fixos dos campos (bytes)
#define PIPE_PATH_SIZE 40 //nome de um FIFO (padding com '\0')
#define KEY_SIZE       41 //chave: 40 + terminador
#define VALUE_SIZE     41 //valor: 40 + terminador

// Opcodes (1 byte, valor numérico)
enum {
  OP_CODE_CONNECT = 1,
  OP_CODE_DISCONNECT = 2,
  OP_CODE_SUBSCRIBE = 3,
  OP_CODE_UNSUBSCRIBE = 4
};

//Dimensões totais de cada mensagem (para ler/escrever de uma só vez)
#define CONNECT_REQUEST_SIZE    (1 + 3 * PIPE_PATH_SIZE) // 1 + 120 = 121
#define DISCONNECT_REQUEST_SIZE (1)                      // só o opcode
#define SUB_REQUEST_SIZE        (1 + KEY_SIZE)           // 1 + 41 = 42
#define RESPONSE_SIZE           (2)                      // opcode + result
#define NOTIFICATION_SIZE       (KEY_SIZE + VALUE_SIZE)  // 41 + 41 = 82 

#endif // COMMON_PROTOCOL_H
