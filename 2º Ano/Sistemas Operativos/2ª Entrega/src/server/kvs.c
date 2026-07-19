#include "kvs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "src/common/protocol.h"
#include "src/common/io.h"

// Hash function based on key initial
int hash(const char *key) {
    int firstLetter = tolower((unsigned char)key[0]);
    if (firstLetter >= 'a' && firstLetter <= 'z') {
        return firstLetter - 'a';
    } else if (firstLetter >= '0' && firstLetter <= '9') {
        return firstLetter - '0';
    }
    return -1; // chave que não começa por letra/dígito
}

struct HashTable *create_hash_table() {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    for (int i = 0; i < TABLE_SIZE; i++)
        ht->table[i] = NULL;
    return ht;
}

// envia [key(41)][value(41)] a todos os subscritores da chave
// para um DELETE, passar value = "DELETED"
static void notify_subscribers(KeyNode *node, const char *value) {
    char msg[NOTIFICATION_SIZE];
    memset(msg, 0, sizeof(msg));
    memcpy(msg, node->key, strnlen(node->key, KEY_SIZE - 1));
    memcpy(msg + KEY_SIZE, value, strnlen(value, VALUE_SIZE - 1));
    for (SubNode *s = node->subscribers; s != NULL; s = s->next) {
        write_all(s->notif_fd, msg, NOTIFICATION_SIZE);
    }
}

// liberta a lista de subscritores de um nó
static void free_subscribers(KeyNode *node) {
    SubNode *s = node->subscribers;
    while (s != NULL) {
        SubNode *tmp = s;
        s = s->next;
        free(tmp);
    }
    node->subscribers = NULL;
}

int write_pair(HashTable *ht, const char *key, const char *value) {
    int index = hash(key);
    if (index < 0) return 1;
    KeyNode *keyNode = ht->table[index];

    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            free(keyNode->value);
            keyNode->value = strdup(value);
            notify_subscribers(keyNode, value); //avisa os subscritores
            return 0;
        }
        keyNode = keyNode->next;
    }

    // chave nova (ainda sem subscritores)
    keyNode = malloc(sizeof(KeyNode));
    keyNode->key = strdup(key);
    keyNode->value = strdup(value);
    keyNode->subscribers = NULL;
    keyNode->next = ht->table[index];
    ht->table[index] = keyNode;
    return 0;
}

char *read_pair(HashTable *ht, const char *key) {
    int index = hash(key);
    if (index < 0) return NULL;
    KeyNode *keyNode = ht->table[index];
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0)
            return strdup(keyNode->value);
        keyNode = keyNode->next;
    }
    return NULL;
}

int delete_pair(HashTable *ht, const char *key) {
    int index = hash(key);
    if (index < 0) return 1;
    KeyNode *keyNode = ht->table[index];
    KeyNode *prevNode = NULL;

    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            notify_subscribers(keyNode, "DELETED"); //avisa antes de remover
            free_subscribers(keyNode);
            if (prevNode == NULL)
                ht->table[index] = keyNode->next;
            else
                prevNode->next = keyNode->next;
            free(keyNode->key);
            free(keyNode->value);
            free(keyNode);
            return 0;
        }
        prevNode = keyNode;
        keyNode = keyNode->next;
    }
    return 1;
}

int subscribe_key(HashTable *ht, const char *key, int notif_fd) {
    int index = hash(key);
    if (index < 0) return 0;
    KeyNode *keyNode = ht->table[index];
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            // não duplica a subscrição do mesmo cliente
            for (SubNode *s = keyNode->subscribers; s != NULL; s = s->next)
                if (s->notif_fd == notif_fd) return 1;
            SubNode *sub = malloc(sizeof(SubNode));
            sub->notif_fd = notif_fd;
            sub->next = keyNode->subscribers;
            keyNode->subscribers = sub;
            return 1; //a chave existia
        }
        keyNode = keyNode->next;
    }
    return 0; //a chave não existia
}

int unsubscribe_key(HashTable *ht, const char *key, int notif_fd) {
    int index = hash(key);
    if (index < 0) return 1;
    KeyNode *keyNode = ht->table[index];
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            SubNode *s = keyNode->subscribers, *prev = NULL;
            while (s != NULL) {
                if (s->notif_fd == notif_fd) {
                    if (prev == NULL) keyNode->subscribers = s->next;
                    else prev->next = s->next;
                    free(s);
                    return 0; // existia e foi removida
                }
                prev = s;
                s = s->next;
            }
            return 1; // chave existe mas não estava subscrita
        }
        keyNode = keyNode->next;
    } 
    return 1;
}

void remove_client(HashTable *ht, int notif_fd) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (KeyNode *keyNode = ht->table[i]; keyNode != NULL; keyNode = keyNode->next) {
            SubNode *s = keyNode->subscribers, *prev = NULL;
            while (s != NULL) {
                if (s->notif_fd == notif_fd) {
                    SubNode *tmp = s;
                    if (prev == NULL) keyNode->subscribers = s->next;
                    else prev->next = s->next;
                    s = s->next;
                    free(tmp);
                } else {
                    prev = s;
                    s = s->next;
                }
            }
        }
    }
}

void clear_all_subscriptions(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (KeyNode *k = ht->table[i]; k != NULL; k = k->next){
            free_subscribers(k);
        }
    }
}

void free_table(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyNode *keyNode = ht->table[i];
        while (keyNode != NULL) {
            KeyNode *temp = keyNode;
            keyNode = keyNode->next;
            free_subscribers(temp);
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(ht);
}