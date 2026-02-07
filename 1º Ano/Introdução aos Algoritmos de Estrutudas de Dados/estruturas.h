/**
 * @file estruturass.h
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
    struct inoculacoes *next;
} Inoculacoes;

#endif