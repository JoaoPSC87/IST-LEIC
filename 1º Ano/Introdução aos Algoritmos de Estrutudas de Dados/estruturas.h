/**
 * @file estruturas.h
 * @brief Definições de estruturas a utilizar no programa
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém a definição das estruturas Vacinas e Inoculacoes
 * 
 */

#ifndef _ESTRUTURAS_
#define _ESTRUTURAS_
#include "datas.h"

/**
 * @brief Estrutura que representa uma vacina
 * 
 * Esta estrutura armazena os componentes de uma vacina: nome, o lote, a data de
 * validade, o número de doses máximo, o número de doses que já foram aplicadas 
 * e um apontador para o próximo nó na lista.
 * 
 */

typedef struct vacinas{
    char *nome_vacina;
    char *lote;
    Data validade;
    int doses;
    int doses_aplicadas;
    struct vacinas *next;
} Vacinas;

/**
 * @brief Estrutura que representa um registo de uma inoculação
 * 
 * Esta estrutura armazena os componentes de um registo de uma inoculação:
 * o nome do utente, o lote que foi aplicado, o nome da vacina que foi aplicada,
 * a data de aplicação e um apontador para o próximo nó na lista.
 * 
 */

typedef struct inoculacoes{
    char *nome_utente;
    char *lote;
    char *nome_vacina;
    Data data_aplicacao;
    struct inoculacoes *next_utente; /**< próximo registo DO MESMO utente */
    struct inoculacoes *next;
} Inoculacoes;

/** Tamanho da tabela de dispersão (primo, para melhor distribuição) */
#define HASH_SIZE 10007

/**
 * @brief Entrada da tabela de dispersão: um utente e os seus registos
 * 
 * Cada utente guarda a sua própria lista de inoculações (por ordem
 * cronológica), o que permite responder a consultas sobre um utente sem
 * percorrer a lista global.
 */
typedef struct utente {
    char *nome; /**< nome do utente (chave) */
    Inoculacoes *primeiro; /**< primeiro registo deste utente */
    Inoculacoes *ultimo; /**< último registo (inserção O(1)) */
    struct utente *next; /**< próximo utente no mesmo bucket */
} Utente;

/**
 * @brief Estrutura que agrega o registo de todas as inoculações
 * 
 * Guarda o início da lista cronológica e também o seu último elemento
 * (cauda), o que permite inserir uma nova inoculação em tempo constante
 * em vez de percorrer a lista toda.
 */
typedef struct registo {
    Inoculacoes *inicio; /**< primeiro registo (ordem cronológica) */
    Inoculacoes *fim; /**< último registo: permite inserção O(1) */
    Utente *tabela[HASH_SIZE]; /**< dispersão: nome do utente -> registos */
} Registo;

#endif