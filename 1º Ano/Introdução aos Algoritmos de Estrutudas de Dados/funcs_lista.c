/**
 * @file funcs_lista.c
 * @brief Funções de listagem
 * @author ist157175 (João Carvalho)
 */

#include "funcs_lista.h"
#include "hash.h"

/**
 * @brief Imprime todas as vacinas na lista das vacinas
 * 
 * Esta função permite listar todas as vacinas da forma <nome> <lote> <data de 
 * validade> <doses disponiveis> <doses aplicadas>. Primeiramente verifica se a
 * lista está vazia, caso contrário percorre a lista e vai imprimindo as vacinas
 * com doses disponiveis até chegar ao fim da lista
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */

void listaTodasVacinas(Vacinas **lista_vacinas){
    Vacinas *atual = *lista_vacinas;

    //verifica se a lista das vacinas está vazia
    if (lista_vacinas == NULL || *lista_vacinas == NULL) {
        return;
    }

    // caso a lista não esteja vazia, percorre a lista e imprime todas as 
    // vacinas com doses disponiveis
    while(atual != NULL){
        if(atual->doses >= 0){
            printf("%s %s %02d-%02d-%d %d %d\n", atual->nome_vacina,
                 atual->lote, atual->validade.dia, atual->validade.mes,
                 atual->validade.ano, atual->doses - atual->doses_aplicadas,
                  atual->doses_aplicadas);
        }
        atual = atual->next;
    }

}

/**
 * @brief Imprime todas as vacinas na lista das vacinas com um determinado nome
 * 
 * Esta função permite listar todas as vacinas com o nome passado em argumento 
 * da forma <nome> <lote> <data de validade> <doses disponiveis>
 * <doses aplicadas>. Primeiramente verifica se a lista está vazia, caso
 * contrário percorre a lista e vai imprimindo as vacinas, com o nome que foi
 * passado em argumento, e com doses disponiveis até chegar ao fim da lista.
 *  
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] nome nome da vacina que se pretende listar
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void listaVacinasNome(Vacinas **lista_vacinas, char nome[], char lingua[]){
    Vacinas *atual = *lista_vacinas;
    int existe = 0;
    //verifica se a lista das vacinas está vazia
    if (lista_vacinas == NULL || *lista_vacinas == NULL) {
        printf("%s: %s\n", nome, strcmp(lingua, "pt") == 0 ?
             "vacina inexistente" : "no such vaccine");
        return;
    }
    // caso a lista não esteja vazia, percorre a lista e imprime os lotes com o 
    // nome igual ao que foi passado em argumento
    while(atual != NULL){
        if(!strcmp(atual->nome_vacina, nome)){
            if(atual->doses >= 0){
                existe = 1;
                printf("%s %s %02d-%02d-%d %d %d\n", atual->nome_vacina,
                    atual->lote, atual->validade.dia, atual->validade.mes,
                    atual->validade.ano, atual->doses - atual->doses_aplicadas,
                    atual->doses_aplicadas);
            }
        }
        atual = atual->next;
        
    }
    //Se a vacina que foi passada em argumento não estiver na lista, passa a 
    //mensagem de erro
    if (!existe) {
        printf("%s: %s\n", nome, strcmp(lingua, "pt") == 0 ?
        "vacina inexistente" : "no such vaccine");
    }

}

/**
 * @brief Imprime todos os registos de utentes na lista de inoculações
 * 
 * Esta função permite listar todos os registos de inoculações da forma
 * <nome do utente> <lote> <data de aplicação>. Primeiramente verifica se a
 * lista está vazia, caso contrário percorre a lista e vai imprimindo os 
 * registos até chegar ao fim da lista
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void listaTodosUtentes(Registo *registo){
    Inoculacoes *atual = registo->inicio;
    //verifica se a lista dos registos de inoculações está vazia
    if (registo->inicio == NULL) {
        return;
    }
    //Caso a lista não esteja vazia, percorre a lista e imprime tudo
    while(atual != NULL){
        
        printf("%s %s %02d-%02d-%d\n", atual->nome_utente, atual->lote,
            atual->data_aplicacao.dia, atual->data_aplicacao.mes,
            atual->data_aplicacao.ano);
        
        atual = atual->next;
    }
}

/**
 * @brief Imprime todos os registos de um utente na lista de inoculações
 * 
 * Esta função permite listar todos os registos de inoculações de um utente 
 * da forma <nome do utente> <lote> <data de aplicação>. O utente é obtido
 * diretamente da tabela de dispersão e são percorridos apenas os seus 
 * registos, que já se encontram por ordem cronológica. Caso o utente não 
 * exista ou não tenha registos, imprime uma mensagem de erro.
 * 
 * @see procuraUtente
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome nome do utente que se pretende listar os registos
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void listaUtenteNome(Registo *registo,char nome[], char lingua[]){
    Utente *u = procuraUtente(registo, nome);
    Inoculacoes *atual = (u == NULL) ? NULL : u->primeiro;

    if (atual == NULL) {
        printf("%s: %s\n", nome, strcmp(lingua, "pt") == 0 ?
        "utente inexistente" : "no such user");
        return;
    }
    // percorre apenas os registos deste utente (já em ordem cronológica)
    while (atual != NULL) {
        printf("%s %s %02d-%02d-%d\n", atual->nome_utente, atual->lote,
            atual->data_aplicacao.dia, atual->data_aplicacao.mes,
            atual->data_aplicacao.ano);
        atual = atual->next_utente;
    }
}