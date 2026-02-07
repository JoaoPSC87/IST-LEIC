/**
 * @file funcs_verificacao.h
 * @brief Definições de funções que verificam e validam parâmetros
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções que verificam e validam os
 * parâmetros que são passados em argumento.
 */

#ifndef _FUNCS_VERIFICACAO_
#define _FUNCS_VERIFICACAO_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Verifica se um determinado lote é válido
 * 
 * @param[in] lote lote que se pretende validar
 * @return 1 se o lote fôr valido, 0 caso contrário
 */
int loteValido(char *lote);

/**
 * @brief Verifica se um determinado lote existe
 * 
 * @param[in] lote lote que se pretende verificar se existe
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return 1 se o lote existir, 0 caso contrário
 */
int loteExiste(char *lote, Vacinas **lista_vacinas);

/**
 * @brief Verifica se uma determinada vacina é válida
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] num_vacinas número total de vacinas guardadas em sistema
 * @param[in] doses número de doses da vacina
 * @param[in] lote lote da vacina
 * @param[in] nome_vacina nome da vacina
 * @param[in] data data de validade da vacina
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se a vacina fôr valida, 0 caso contrário
 */
int vacinaValida(Vacinas **lista_vacinas, int num_vacinas, int doses,
    char lote[], char nome_vacina[], Data data, Data *data_sistema,
    char lingua[]);

/**
 * @brief Verifica se um determinado lote tem stock
 * 
 * @param[in] nome_vacina nome da vacina
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lote lote que se pretende verificar se tem stock
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] data_sistema data atual da simulação
 * @return 1 se o lote tiver stock, 0 caso contrário
 */
int existeStock(char nome_vacina[], Vacinas **lista_vacinas, char lote[],
    char lingua[], Data *data_sistema);

/**
 * @brief Verifica se um determinado utente já foi vacinado nesse dia com uma
 * determinada vacina
 * 
 * @param[in] nome_vacina nome da vacina
 * @param[in] nome_utente nome do utente que se pretende verificar se foi 
 * vacinado nesse dia com uma determinada vacina
 * @param[in] data_sistema data atual da simulação
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se o utente foi vacinado nesse dia com a vacina, 0 caso contrário
 */
int jaFoiVacinado(char nome_vacina[], char nome_utente[], Data *data_sistema,
    Inoculacoes **lista_inoculacoes, char lingua[]);

/**
 * @brief Processa uma determinada aplicação de uma vacina num determinado
 * utente
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] nome_utente nome do utente que se pretende vacinar
 * @param[in] aplicacao vacina que se pretende processar para verificar se é
 * possivel vacinar o utente
 * @return 1 se o utente foi vacinado com sucesso, 0 caso contrário
 */    
int processaAplicacaoVacina(Vacinas **lista_vacinas,
    Inoculacoes **lista_inoculacoes, Data *data_sistema, char lingua[],
    char *nome_utente, Inoculacoes *aplicacao);

/**
 * @brief Verifica se um utente tem registo
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] nome_utente nome do utente que se pretende verificar
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se o utente já tem registo, 0 caso contrário
 */   
int utenteTemRegisto(Inoculacoes **lista_utentes, char nome_utente[],
    char lingua[]);

/**
 * @brief Verificação da data e do lote de um registo de inoculação a apagar
 * 
 * @param[in] data data a verificar
 * @param[in] data_sistema data atual da simulação
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se os parâmetros são válidos, 0 caso contrário
 */  
int validaDataLote(Data *data, char *lote, Data *data_sistema,
    Vacinas **lista_vacinas, char lingua[]);

#endif