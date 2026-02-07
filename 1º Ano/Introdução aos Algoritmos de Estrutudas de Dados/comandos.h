/**
 * @file comandos.h
 * @brief Definições das funções que executam os comandos dados pelo utilizador
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções que aplicam os comandos da 
 * função principal.
 */

#ifndef _COMANDOS_
#define _COMANDOS_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"


/**
 * @brief Adiciona uma vacina à lista de vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] num_vacinas número atual de vacinas guardadas em sistema
 * @return O número atual de vacinas guardadas em sistema
 */
int adicionaLote(Vacinas **lista_vacinas, Data *data_sistema, char lingua[],
    int num_vacinas);

/**
 * @brief Faz a listagem das vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */    
void listaVacinas(Vacinas **lista_vacinas, char lingua[]);

/**
 * @brief Aplica uma vacina a um utente
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void aplicaVacina(Vacinas **lista_vacinas, Inoculacoes **lista_inoculacoes,
    Data *data_sistema, char lingua[]);

/**
 * @brief Retira uma vacina à lista de vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] num_vacinas número atual de vacinas guardadas em sistema
 * @return O número atual de vacinas guardadas em sistema
 */
int retiraLote(Vacinas **lista_vacinas, char lingua[], int num_vacinas);

/**
 * @brief Apaga registos de inoculações de um utente
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void apagaRegisto(Inoculacoes **lista_inoculacoes, Vacinas ** lista_vacinas,
    Data *data_sistema, char lingua[]);

/**
 * @brief Faz a listagem dos registos de inoculações
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */   
void listaInoculacoes(Inoculacoes **lista_inoculacoes, char lingua[]);

/**
 * @brief Avança a data da simulação
 * 
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return ou retorna NULL ou retorna a nova data para atualizar o sistema
 */
Data *avancarTempo(Data *data_sistema, char lingua[]);

#endif