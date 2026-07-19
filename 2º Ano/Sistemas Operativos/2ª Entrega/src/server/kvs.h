#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#define TABLE_SIZE 26

#include <stddef.h>

// um subscritor de uma chave: o fd do seu FIFO de notificações
typedef struct SubNode {
    int notif_fd;
    struct SubNode *next;
} SubNode;

typedef struct KeyNode {
    char *key;
    char *value;
    SubNode *subscribers; //lista de subscritores desta chave
    struct KeyNode *next;
} KeyNode;

typedef struct HashTable {
    KeyNode *table[TABLE_SIZE];
} HashTable;

/// Índice da hashtable para uma chave (-1 se inválida)
int hash(const char *key);

/// Cria uma nova hashtable, NULL em falha
struct HashTable *create_hash_table();

/// Escreve/atualiza um par. Se a chave tiver subscritores, notifica-os
int write_pair(HashTable *ht, const char *key, const char *value);

/// Devolve uma cópia do valor da chave (NULL se não existe)
char *read_pair(HashTable *ht, const char *key);

/// Remove um par. Notifica os subscritores com "DELETED"
int delete_pair(HashTable *ht, const char *key);

/// Subscreve um cliente (pelo fd de notificações) a uma chave
/// @return 1 se a chave existia (subscrição feita), 0 caso contrário
int subscribe_key(HashTable *ht, const char *key, int notif_fd);

/// Remove a subscrição de um cliente a uma chave
/// @return 0 se a subscrição existia e foi removida, 1 caso contrário
int unsubscribe_key(HashTable *ht, const char *key, int notif_fd);

/// Remove todas as subscrições de um cliente (em todas as chaves)
void remove_client(HashTable *ht, int notif_fd);

/// Remove todas as subscrições de todas as chaves (usado no SIGSUR1)
void clear_all_subscriptions(HashTable *ht);

/// Liberta a hashtable (e as listas de subscritores)
void free_table(HashTable *ht);

#endif  // KVS_H
