/**
 * @file func_aux.h
 * @brief Definições de funções auxiliares para o funcionamento do programa
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções auxiliares ao programa
 * principal nomeadamente a leitura de um nome de um utente e à aplicação de uma
 * vacina.
 * 
 */

#ifndef _FUNC_AUX_
#define _FUNC_AUX_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Lê o nome de um utente do standard input
 * 
 * @param[in,out] size_med Apontador para o tamanho inicial do buffer.
 * @return Apontador para a string ou NULL em caso de falha na alocação
 */
char* leNomeUtente(int *size_med);

/**
 * @brief Atualiza o número de doses aplicadas de um lote de vacina
 * 
 * @param[in] lote Código identificador do lote de vacina a procurar
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */

void vacina(char lote[], Vacinas **lista_vacinas);

#endif