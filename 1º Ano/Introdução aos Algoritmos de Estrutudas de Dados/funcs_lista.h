/**
 * @file funcs_lista.h
 * @brief Definições de funções de listagem
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções de listagems
 * 
 */


#ifndef _FUNCS_LISTA_
#define _FUNCS_LISTA_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Imprime todas as vacinas na lista das vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */
void listaTodasVacinas(Vacinas**lista_vacinas);

/**
 * @brief Imprime todas as vacinas na lista das vacinas com um determinado nome
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] nome nome da vacina que se pretende listar
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void listaVacinasNome(Vacinas**lista_vacinas, char nome[], char lingua[]);

/**
 * @brief Imprime todos os registos de utentes na lista de inoculações
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @return void (a função não retorna valores)
 */
void listaTodosUtentes(Inoculacoes**lista_inoculacoes);

/**
 * @brief Imprime todos os registos de um utente na lista de inoculações
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] nome nome do utente que se pretende listar os registos
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void listaUtenteNome(Inoculacoes**lista_inoculacoes, char nome[],char lingua[]);

#endif