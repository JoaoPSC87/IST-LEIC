/**
 * @file hash.h
 * @brief Definições da tabela de dispersão de utentes
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém as declarações das funções que gerem a tabela de 
 * dispersão que indexa os registos de inoculação por nome de utente.
 */
#ifndef _HASH_
#define _HASH_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "estruturas.h"

/**
 * @brief Calcula o índice da tabela de dispersão para um nome de utente
 * 
 * @param[in] nome nome do utente a dispersar
 * @return índice da posição da tabela correspondente ao nome
 */
unsigned int hashNome(const char *nome);

/**
 * @brief Procura um utente na tabela de dispersão
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome nome do utente a procurar
 * @return Apontador para o utente, ou NULL se não existir
 */
Utente *procuraUtente(Registo *registo, const char *nome);

/**
 * @brief Associa um registo de inoculação ao respetivo utente
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] reg registo de inoculação a associar ao utente
 * @return 1 se a associação foi bem sucedida, 0 em caso de falta de memória
 */
int insereNaHash(Registo *registo, Inoculacoes *reg);

/**
 * @brief Retira um registo de inoculação da lista do respetivo utente
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] reg registo de inoculação a retirar
 * @return void (a função não retorna valores)
 */
void removeDaHash(Registo *registo, Inoculacoes *reg);

/**
 * @brief Liberta a tabela de dispersão
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void destroyHash(Registo *registo);

#endif