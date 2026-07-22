/**
 * @file funcs_insert.h
 * @brief Definições de funções para inserção de vacinas e registos de
 * inoculações
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções de inserção de forma ordenada 
 * nas listas de vacinas e de registo de inoculações.
 * 
 */

#ifndef _FUNCS_INSERT_
#define _FUNCS_INSERT_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Insere uma vacina ordenadamente na lista de vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] nova_vacina Apontador para a nova vacina a ser inserida
 * @return void (a função não retorna valores)
 */
void inserirOrdenado(Vacinas **lista_vacinas, Vacinas *nova_vacina);

/**
 * @brief Insere um registo de aplicação ordenadamente na lista de inoculações
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] aplicacao Apontador para o novo registo de aplicação a ser inserido
 * @return void (a função não retorna valores)
 */
void insereAplicacaoOrdenado(Registo *registo, Inoculacoes *aplicacao);

/**
 * @brief Inicializa o registo de inoculações
 * 
 * Esta função coloca o registo de inoculações no estado inicial: lista 
 * cronológica vazia (início e cauda a NULL) e tabela de dispersão sem 
 * qualquer utente.
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void iniciaRegisto(Registo *registo);

#endif