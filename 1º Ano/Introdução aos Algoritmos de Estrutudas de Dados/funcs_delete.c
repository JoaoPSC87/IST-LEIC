/**
 * @file funcs_delete.c
 * @brief Funções que apagam nós nas listas e funções que apagam listas inteiras
 * @author ist157175 (João Carvalho)
 */

#include "funcs_delete.h"
#include "hash.h"

/**
 * @brief Apaga uma vacina na lista de vacinas
 * 
 * Esta função permite apagar um lote na lista de vacinas. Primeiramente a 
 * função percorre a lista até encontrar o lote a apagar, seguidamente verifica
 * se já foram aplicadas doses desse lote, em caso afirmativo simplesmente muda
 * o número de doses máximo para o número de doses aplicadas, fazendo assim com
 * que esse lote fique sem doses disponíveis, caso contrário a função apaga esse
 * nó e liberta a memória alocada a esse nó. A função devolve o número de doses
 * aplicadas desse lote.
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lote lote que se pretende apagar
 * @return Número de aplicações dessa vacina
 */
int apagaLote(Vacinas **lista_vacinas, char lote[]) {
    int num_aplicacoes = 0;
    Vacinas *atual = *lista_vacinas;
    Vacinas *anterior = NULL;

    //Percorre a lista até encontrar o lote
    while (strcmp(atual->lote, lote) != 0) {
        anterior = atual;
        atual = atual->next;
    }

    //Verifica se há doses aplicadas
    if (atual->doses_aplicadas != 0) {
        atual->doses = atual->doses_aplicadas;
        num_aplicacoes = atual->doses_aplicadas;
    } else {
        //Remove o nó da lista
        if (anterior == NULL) {
            // Caso o lote a apagar seja o primeiro elemento lista
            *lista_vacinas = atual->next;  
        } else {
            // Caso o lote a apagar seja um elemento do meio ou fim da lista
            anterior->next = atual->next;  
        }

        //Liberta a memória alocada ao lote removido
        free(atual->nome_vacina);
        free(atual->lote);
        free(atual);
    }

    return num_aplicacoes;
}

/**
 * @brief Apaga a lista de vacinas
 * 
 * Esta função permite apagar a lista inteira de vacinas. A função vai
 * percorrendo a lista de vacinas e vai libertando a memória alocada aos
 * items da lista de vacinas.
 * 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */
void destroyVacinas(Vacinas **lista_vacinas){
    Vacinas *atual = *lista_vacinas;
    //Percorre a lista toda e vai libertando a memória alocada aos 
    //items da lista de vacinas
    while(atual != NULL){
        Vacinas *temp = atual;
        free(temp->lote);
        free(temp->nome_vacina);
        atual = atual->next;
        free(temp); 
    }
    *lista_vacinas = NULL;
    free(*lista_vacinas);
}

/**
 * @brief Apaga a lista de registos de inoculações
 * 
 * Esta função permite apagar a lista inteira de registos de inoculações. A 
 * função vai percorrendo a lista de inoculações e vai libertando a memória
 * alocada aos items da lista de inoculações.
 * Liberta igualmente a tabela de dispersão dos utentes.
 * 
 * @see destroyHash
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void destroyInoculacoes(Registo *registo){
    Inoculacoes *atual = registo->inicio;
    //Percorre a lista toda e vai libertando a memória alocada aos items da 
    //lista de registo de inoculações
    while(atual !=NULL){
        Inoculacoes *temp = atual;
        free(temp->lote);
        free(temp->nome_utente);
        free(temp->nome_vacina);
        atual = atual->next;
        free(temp);
    }
    destroyHash(registo);
    registo->inicio = NULL;
}

/**
 * @brief Apaga todos os registos de inoculação de um utente num determinado dia
 * com um determinado lote
 * 
 * Esta função permite apagar os registos de um utente num determinado dia com
 * um determinado lote na lista de inoculações. Primeiramente a função vai  
 * percorrendo a lista e vai apagando os registos que correspondam aos registos
 * de um utente que tenha sido vacinado com um determinado lote num determinado
 * dia. Ao apagar um registo, a função vai incrementando o número de registos
 * apagados e vai libertando a memória alocada aos items da lista que foram
 * apagados. A função devolve o número total de registos apagados.
 * Cada registo removido é também retirado da lista do respetivo utente na 
 * tabela de dispersão. No fim, o apontador para a cauda da lista é recalculado,
 * uma vez que o último registo pode ter sido apagado.
 * 
 * @see removeDaHash
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @param[in] data data a qual se pretende apagar registos
 * @param[in] lote lote ao qual se pretende apagar registos
 * @return Número de registos apagados
 */
int apagaInoculacoesUtenteLote(Registo *registo,
     char nome_utente[], Data *data, char lote[]) {

    Inoculacoes *atual = registo->inicio;
    Inoculacoes *anterior = NULL;
    int registos_apagados = 0;
    
    //Percorre a lista toda e vai apagando os registos do utente com os 
    //argumentos passados
    while (atual != NULL) {
        if ((strcmp(atual->nome_utente, nome_utente) == 0) && 
            (strcmp(atual->lote, lote) == 0) && 
            (atual->data_aplicacao.dia == data->dia && 
             atual->data_aplicacao.mes == data->mes && 
             atual->data_aplicacao.ano == data->ano)) {

            registos_apagados++;

            //guardar o próximo nó antes de libertar o atual
            Inoculacoes *temp = atual->next;

            //Se for o primeiro nó da lista, atualiza a cabeça da lista
            if (anterior == NULL) {
                registo->inicio = temp;
            } else {
                anterior->next = temp;
            }

            //Libertar memória do nó atual
            removeDaHash(registo, atual);
            free(atual->nome_utente);
            free(atual->lote);
            free(atual->nome_vacina);
            free(atual);

            //Avançar para o próximo nó
            atual = temp;
        } else {
            //Só atualiza o 'anterior' se o 'atual' não for removido
            anterior = atual;
            atual = atual->next;
        }
    }
    // as remoções podem ter apagado o último registo: recalcula a cauda
    registo->fim = NULL;
    for (Inoculacoes *p = registo->inicio; p != NULL; p = p->next)
        registo->fim = p;
    return registos_apagados;
}

/**
 * @brief Apaga todos os registos de inoculação de um utente num determinado dia
 * 
 * Esta função permite apagar os registos de um utente num determinado dia na 
 * lista de inoculações. Primeiramente a função vai percorrendo a lista e vai
 * apagando os registos que correspondam aos registos de um utente que tenha 
 * sido vacinado num determinado dia. Ao apagar um registo, a função vai 
 * incrementando o número de registos apagados e vai libertando a memória 
 * alocada aos items da lista que foram apagados. A função devolve o número 
 * total de registos apagados.
 * Cada registo removido é também retirado da lista do respetivo utente na 
 * tabela de dispersão. No fim, o apontador para a cauda da lista é recalculado,
 * uma vez que o último registo pode ter sido apagado.
 * 
 * @see removeDaHash 
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @param[in] data data a qual se pretende apagar registos
 * @return Número de registos apagados
 */
int apagaInoculacoesUtenteData(Registo *registo,
    char nome_utente[], Data *data){

    Inoculacoes *atual = registo->inicio;
    Inoculacoes *anterior = NULL;
    int registos_apagados = 0;

    //Percorre a lista toda e vai apagando os registos do utente com os 
    //argumentos passados
    while (atual != NULL) {
        if ((strcmp(atual->nome_utente, nome_utente) == 0) &&
            (atual->data_aplicacao.dia == data->dia && 
             atual->data_aplicacao.mes == data->mes && 
             atual->data_aplicacao.ano == data->ano)) {

            registos_apagados++;

            // Guardar o próximo nó antes de libertar o atual
            Inoculacoes *temp = atual->next;

            //Se for o primeiro nó da lista, atualiza a cabeça da lista
            if (anterior == NULL) {
                registo->inicio = temp;
            } else {
                anterior->next = temp;
            }

            //Libertar memória do nó atual
            removeDaHash(registo, atual);
            free(atual->nome_utente);
            free(atual->lote);
            free(atual->nome_vacina);
            free(atual);

            //Avançar para o próximo nó
            atual = temp;
        } else {
            //Só atualiza o 'anterior' se o 'atual' não for removido
            anterior = atual;
            atual = atual->next;
        }
    }
    // as remoções podem ter apagado o último registo: recalcula a cauda
    registo->fim = NULL;
    for (Inoculacoes *p = registo->inicio; p != NULL; p = p->next)
        registo->fim = p;
    return registos_apagados;
}

/**
 * @brief Apaga todos os registos de inoculação de um utente
 * 
 * Esta função permite apagar os registos de um utente na lista de inoculações. 
 * Primeiramente a função vai percorrendo a lista e vai apagando os registos que 
 * correspondam aos registos de um determinado utente. Ao apagar um registo, a 
 * função vai incrementando o número de registos apagados e vai libertando a 
 * memória alocada aos items da lista que foram apagados. A função devolve o 
 * número total de registos apagados.
 * Cada registo removido é também retirado da lista do respetivo utente na 
 * tabela de dispersão. No fim, o apontador para a cauda da lista é recalculado,
 * uma vez que o último registo pode ter sido apagado.
 * 
 * @see removeDaHash
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome_utente nome do utente que se pretende apagar registos
 * @return Número de registos apagados
 */
int apagaInoculacoesUtente(Registo *registo, char nome_utente[]){
    Inoculacoes *atual = registo->inicio;
    Inoculacoes *anterior = NULL;
    int registos_apagados = 0;
    
    //Percorre a lista toda e vai apagando os registos de inoculações do utente
    while (atual != NULL) {
        if ((strcmp(atual->nome_utente, nome_utente) == 0)){

            registos_apagados++;

            //guardar o próximo nó antes de libertar o atual
            Inoculacoes *temp = atual->next;

            //Se for o primeiro nó da lista, atualiza a cabeça da lista
            if (anterior == NULL) {
                registo->inicio = temp;
            } else {
                anterior->next = temp;
            }

            // Libertar memória do nó atual
            removeDaHash(registo, atual);
            free(atual->nome_utente);
            free(atual->lote);
            free(atual->nome_vacina);
            free(atual);

            //Avançar para o próximo nó
            atual = temp;
        } else {
            //Só atualiza o 'anterior' se o 'atual' não for removido
            anterior = atual;
            atual = atual->next;
        }
    }
    // as remoções podem ter apagado o último registo: recalcula a cauda
    registo->fim = NULL;
    for (Inoculacoes *p = registo->inicio; p != NULL; p = p->next)
        registo->fim = p;
    return registos_apagados;
}