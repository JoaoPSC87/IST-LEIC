/**
 * @file hash.c
 * @brief Tabela de dispersão que indexa os registos de inoculações por utente
 * @author ist157175 (João Carvalho)
 * 
 * Este arquivo contém a implementação da tabela de dispersão que associa a 
 * cada nome de utente a lista dos seus registos de inoculação. Permite 
 * responder em tempo praticamente constante às consultas por utente, em vez 
 * de percorrer a lista global de inoculações.
 */
#include "hash.h"

/**
 * @brief Calcula o índice da tabela de dispersão para um nome de utente
 * 
 * Esta função aplica a função de dispersão djb2 ao nome do utente e devolve
 * o índice correspondente na tabela, através do resto da divisão pelo número
 * de posições da tabela.
 * 
 * @param[in] nome nome do utente a dispersar
 * @return índice da posição da tabela correspondente ao nome
 */
unsigned int hashNome(const char *nome) {
    unsigned long h = 5381;
    while (*nome)
        h = ((h << 5) + 5) + (unsigned char)(*nome++);
    return (unsigned int)(h % HASH_SIZE);
}

/**
 * @brief Procura um utente na tabela de dispersão
 * 
 * Esta função calcula a posição da tabela correspondente ao nome e percorre
 * a lista de colisões dessa posição até encontrar o utente pretendido.
 * 
 * @see hashNome
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] nome nome do utente a procurar
 * @return Apontador para o utente, ou NULL se não existir
 */
Utente *procuraUtente(Registo *registo, const char *nome) {
    Utente *u = registo->tabela[hashNome(nome)];
    while (u != NULL) {
        if (strcmp(u->nome, nome) == 0) return u;
        u = u->next;
    }
    return NULL;
}

/**
 * @brief Associa um registo de inoculação ao respetivo utente
 * 
 * Esta função acrescenta um registo de inoculação ao fim da lista do utente
 * correspondente. Caso o utente ainda não exista na tabela, é criada uma nova
 * entrada com uma cópia do seu nome. A inserção é feita em tempo constante
 * através do apontador para o último registo do utente.
 * 
 * @see procuraUtente
 * @see hashNome
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] reg registo de inoculação a associar ao utente
 * @return 1 se a associação foi bem sucedida, 0 em caso de falta de memória
 */
int insereNaHash(Registo *registo, Inoculacoes *reg) {
    Utente *u = procuraUtente(registo, reg->nome_utente);
    if (u == NULL) {
        unsigned int i = hashNome(reg->nome_utente);
        u = malloc(sizeof(Utente));
        if (u == NULL) return 0;
        u->nome = malloc(strlen(reg->nome_utente) + 1);
        if (u->nome == NULL) {
            free(u);
            return 0;
        }
        strcpy(u->nome, reg->nome_utente);
        u->primeiro = NULL;
        u->ultimo = NULL;
        u->next = registo->tabela[i];
        registo->tabela[i] = u;
    }
    reg->next_utente = NULL;
    if (u->primeiro == NULL) u->primeiro = reg;
    else u->ultimo->next_utente = reg;
    u->ultimo = reg;
    return 1;
}

/**
 * @brief Retira um registo de inoculação da lista do respetivo utente
 * 
 * Esta função percorre a lista de registos do utente até encontrar o registo
 * indicado e retira-o dessa lista, atualizando o apontador para o último 
 * registo caso o removido fosse o último. A entrada do utente é mantida na 
 * tabela, mesmo que fique sem registos.
 * 
 * @see procuraUtente
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @param[in] reg registo de inoculação a retirar
 * @return void (a função não retorna valores)
 */
void removeDaHash(Registo *registo, Inoculacoes *reg) {
    Utente *u = procuraUtente(registo, reg->nome_utente);
    if (u == NULL) return;
    Inoculacoes *atual = u->primeiro, *anterior = NULL;
    while (atual != NULL) {
        if (atual == reg) {
            if (anterior == NULL) u->primeiro = atual->next_utente;
            else anterior->next_utente = atual->next_utente;
            if (u ->ultimo == atual) u->ultimo = anterior;
            return;
        }
        anterior = atual;
        atual = atual->next_utente;
    }
}

/**
 * @brief Liberta a tabela de dispersão
 * 
 * Esta função percorre todas as posições da tabela e liberta a memória 
 * alocada a cada utente e ao seu nome. Os registos de inoculação em si não 
 * são libertados aqui, uma vez que pertencem à lista global.
 * 
 * @param[in,out] registo Ponteiro para o registo de inoculações
 * @return void (a função não retorna valores)
 */
void destroyHash(Registo *registo) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Utente *u = registo->tabela[i];
        while (u != NULL) {
            Utente *temp = u;
            u = u->next;
            free(temp->nome);
            free(temp);
        }
        registo->tabela[i] = NULL;
    }
}