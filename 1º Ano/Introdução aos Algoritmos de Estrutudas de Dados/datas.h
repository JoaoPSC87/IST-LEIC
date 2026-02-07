/**
 * @file datas.h
 * @brief Definições de estruturas e funções para manipulação de datas
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém a definição da estrutura Data e as declarações das
 * funções relacionadas à validação e manipulação de datas para o sistema
 * de simulação que inicia em 2025.
 */

#ifndef _DATAS_
#define _DATAS_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief Estrutura que representa uma data
 * 
 * Esta estrutura armazena os componentes de uma data: dia, mês e ano.
 */

typedef struct Data{
    int dia; /**< Dia do mês (1-31) */
    int mes; /**< Mês do ano (1-12) */
    int ano; /**< Ano (2025 ou posterior) */
} Data;

/**
 * @brief Verifica se uma data é válida
 * 
 * @param[in] data Data a ser verificada
 * @return 1 se a data for válida, 0 caso contrário
 */
int dataValida(Data data);

/**
 * @brief Imprime uma data no formato dd-mm-aaaa
 * 
 * @param[in] data Data a ser impressa
 * @return void (a função não retorna valores)
 */

void escreveData(Data data);

/**
 * @brief Verifica se um ano é bissexto
 * 
 * @param[in] ano Ano a ser verificado
 * @return 1 se o ano for bissexto, 0 caso contrário
 */

int anoBisexto(int ano);

/**
 * @brief Verifica se uma data é mais antiga que outra
 * 
 * @param[in] data1 Primeira data a ser comparada
 * @param[in] data2 Segunda data a ser comparada
 * @return 1 se data1 for mais antiga que data2, 0 caso contrário
 */

int dataMaisAntiga(Data data1, Data data2);


#endif