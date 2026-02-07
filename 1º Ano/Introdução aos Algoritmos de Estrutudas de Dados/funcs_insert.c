/**
 * @file funcs_insert.c
 * @brief Funções para inserção de vacinas e registos de inoculações
 * @author ist157175 (João Carvalho)
 */

 #include "funcs_insert.h"

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
 * Esta função insere um novo registo de aplicação de vacina numa lista ligada
 * de inoculações, mantendo a ordenação cronológica por data de aplicação.
 *
 * @see dataMaisAntiga
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] aplicacao Apontador para o novo registo de aplicação a ser inserido
 * @return void (a função não retorna valores)
 */
void insereAplicacaoOrdenado(Inoculacoes **lista_inoculacoes, 
    Inoculacoes *aplicacao) {
        Inoculacoes *atual = *lista_inoculacoes;
        Inoculacoes *anterior = NULL;

        // Variável para verificar datas iguais
        int datasIguais;

        // Caso a lista esteja vazia insere no início e retorna
        if (*lista_inoculacoes == NULL) {
            *lista_inoculacoes = aplicacao;
            aplicacao->next = NULL;
            return;
        }

        // Percorre a lista até encontrar o ponto de inserção
        while (atual != NULL) {
            // Verifica se a aplicação nova tem data anterior
            if(dataMaisAntiga(aplicacao->data_aplicacao, atual->data_aplicacao))
                break;
                
            // Verifica se as datas são iguais
            datasIguais = !dataMaisAntiga(atual->data_aplicacao, aplicacao->data_aplicacao) &&
                        !dataMaisAntiga(aplicacao->data_aplicacao, atual->data_aplicacao);
            
            // Se a data for a mesma, avança até o último elemento desse dia
            if (datasIguais) {
                while (atual->next != NULL && 
                        !dataMaisAntiga(atual->next->data_aplicacao, aplicacao->data_aplicacao) &&
                        !dataMaisAntiga(aplicacao->data_aplicacao, atual->next->data_aplicacao)) {
                    anterior = atual;
                    atual = atual->next;
                }
            }
            anterior = atual;
            atual = atual->next;
        }

        // Faz a inserção do novo nó
        aplicacao->next = atual;
        anterior ? (anterior->next = aplicacao) : (*lista_inoculacoes = aplicacao);
}