/**
 * @file funcs_insert.c
 * @brief Funções para inserção de vacinas e registos de inoculações
 * @author ist157175 (João Carvalho)
 */

 #include "funcs_insert.h"
 #include "hash.h"

 /**
  * @brief Insere uma vacina ordenadamente na lista de vacinas
  *
  * Esta função insere uma nova vacina numa lista ligada de vacinas, mantendo
  * a ordenação por data de validade (da mais antiga para a mais recente) e,
  * em caso de empate, por ordem alfabética do lote.
  *
  * @see dataMaisAntiga
  * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
  * @param[in] nova_vacina Apontador para a nova vacina a ser inserida
  * @return void (a função não retorna valores)
  */
 void inserirOrdenado(Vacinas **lista_vacinas, Vacinas *nova_vacina) {
    // Se a lista estiver vazia ou se a nova vacina tiver uma validade mais antiga
    // ou se tiver a mesma validade mas um lote alfabéticamente menor,
    // insere a nova vacina no início da lista.
    if (*lista_vacinas == NULL ||
            dataMaisAntiga(nova_vacina->validade, (*lista_vacinas)->validade) ||
             (nova_vacina->validade.ano == (*lista_vacinas)->validade.ano &&
             nova_vacina->validade.mes == (*lista_vacinas)->validade.mes &&
             nova_vacina->validade.dia == (*lista_vacinas)->validade.dia &&
             strcmp(nova_vacina->lote, (*lista_vacinas)->lote) < 0)) {
        nova_vacina->next = *lista_vacinas; // Aponta para a antiga primeira vacina
        *lista_vacinas = nova_vacina; // Atualiza a cabeça da lista
        return;
    }
     
    Vacinas *atual = *lista_vacinas;
    // Percorre a lista para encontrar a posição correta de inserção
    while (atual->next != NULL &&
            (dataMaisAntiga(atual->next->validade, nova_vacina->validade) ||
            (atual->next->validade.ano == nova_vacina->validade.ano &&
             atual->next->validade.mes == nova_vacina->validade.mes &&
             atual->next->validade.dia == nova_vacina->validade.dia &&
             strcmp(atual->next->lote, nova_vacina->lote) < 0))) {
        atual = atual->next;
    }
     
    // Insere a nova vacina na posição correta
    nova_vacina->next = atual->next;
    atual->next = nova_vacina;
 }
 
/**
 * @brief Insere um registo de aplicação ordenadamente na lista de inoculações
 *
 * Esta função insere um novo registo de aplicação de vacina no registo de
 * inoculações. Como as inoculações são sempre aplicadas na data atual do 
 * sistema, e essa data nunca recua, o novo registo pertence sempre ao fim da
 * lista cronológica: usando o apontador para a cauda, a inserção é feita em
 * tempo constante. O registo é também associado ao respetivo utente na tabela
 * de dispersão, permitindo consultas rápidas por utente.
 * 
 * @see insereNaHash
 *
 * @see dataMaisAntiga
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] aplicacao Apontador para o novo registo de aplicação a ser inserido
 * @return void (a função não retorna valores)
 */
void insereAplicacaoOrdenado(Registo *registo, Inoculacoes *aplicacao) {
    // As inoculações são sempre aplicadas na data atual do sistema, e essa
    // data só avança -> o ponto de inserção é SEMPRE o fim da lista.
    // Guardando a cauda, a inserção passa de O(N) para O(1).
    aplicacao->next = NULL;
    if (registo->inicio == NULL)
        registo->inicio = aplicacao;
    else
        registo->fim->next = aplicacao;
    registo->fim = aplicacao;
    insereNaHash(registo, aplicacao);
}

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
void iniciaRegisto(Registo *registo) {
    registo->inicio = NULL;
    registo->fim = NULL;
    for (int i = 0; i < HASH_SIZE; i++)
    registo->tabela[i] = NULL;
}