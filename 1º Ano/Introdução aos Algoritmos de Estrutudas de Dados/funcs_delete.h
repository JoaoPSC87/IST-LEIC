/**
 * @file funcs_delete.h
 * @brief Definições de funções que apagam nós nas listas e funções que apagam 
 * listas inteiras
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções que apagam nós das listas e 
 * funções que apagam listas inteiras.
 * 
 */

#ifndef _FUNCS_DELETE_
#define _FUNCS_DELETE_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Apaga uma vacina na lista de vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lote lote que se pretende apagar
 * @return Número de aplicações dessa vacina
 */
int apagaLote(Vacinas **lista_vacinas, char lote[]);

/**
 * @brief Apaga a lista de vacinas
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */
void destroyVacinas(Vacinas **lista_vacinas);

/**
 * @brief Apaga a lista de registos de inoculações
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void destroyInoculacoes(Registo *registo);

/**
 * @brief Apaga todos os registos de inoculação de um utente num determinado dia
 * com um determinado lote
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @param[in] data data a qual se pretende apagar registos
 * @param[in] lote lote ao qual se pretende apagar registos
 * @return Número de registos apagados
 */
int apagaInoculacoesUtenteLote(Registo *registo,
     char nome_utente[], Data *data, char lote[]);

/**
 * @brief Apaga todos os registos de inoculação de um utente num determinado dia
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @param[in] data data a qual se pretende apagar registos
 * @return Número de registos apagados
 */
int apagaInoculacoesUtenteData(Registo *registo,
     char nome_utente[], Data *data);

/**
 * @brief Apaga todos os registos de inoculação de um utente
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @return Número de registos apagados
 */     
int apagaInoculacoesUtente(Registo *registo, char nome_utente[]);

#endif