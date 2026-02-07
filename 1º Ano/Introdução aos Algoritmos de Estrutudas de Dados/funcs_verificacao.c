/**
 * @file funcs_verificacao.c
 * @brief Definições de funções que verificam e validam parâmetros
 * @author ist157175 (João Carvalho)
 * 
 */

#include "funcs_verificacao.h"
#include "func_aux.h"
#include "funcs_insert.h"
//número máximo de lotes que podem ser registados no sistema
#define MAX_VACINAS 1000 
//número máximo de bytes que o nome da vacina pode ter
#define MAX_BYTES_VACINAS 51 
//número máximo de caracteres que o lote pode ter
#define MAX_CHAR_LOTE 20 

/**
 * @brief Verifica se um determinado lote é válido
 * 
 * Esta função permite verificar se uma string passada em argumento é válida
 * como nome de lote. Para uma string ser válida como nome de lote esta só pode
 * conter caracteres em hexadécimal (números de 0 a 9 e letras de A a F). Notar
 * que as letras só são válidas se forem maiúsculas. A função devolve 1 caso
 * a string corresponde a um nome de lote válido, 0 caso contrário.
 * 
 * @param[in] lote lote que se pretende validar
 * @return 1 se o lote fôr valido, 0 caso contrário
 */
int loteValido(char *lote){
    int i;
    for(i = 0; lote[i] != '\0'; i++){
        //o lote só é válido se for um conjunto de caracteres em hexadecimal
        if((lote[i] >= 'A' && lote[i] <= 'F') || (lote[i] >= '0'
             && lote[i] <= '9')) 
            continue;
        else return 0;
    }
    return 1;
}

/**
 * @brief Verifica se um determinado lote existe
 * 
 * Esta função permite verificar a existência de um determinado lote.
 * Primeiramente a função verifica se a lista está vazia, caso não esteja, a 
 * função vai percorrendo a lista até encontrar um registo com o mesmo lote que
 * foi passado em argumento. A função devolve 1 caso o lote exista e devolve 0 
 * caso o lote não exista.
 * 
 * @param[in] lote lote que se pretende verificar se existe
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return 1 se o lote existir, 0 caso contrário
 */
int loteExiste(char *lote, Vacinas **lista_vacinas) {
    if (!lote || !lista_vacinas || !*lista_vacinas) return 0;
    
    Vacinas *atual = *lista_vacinas;

    //Percorre a lista até encontrar a primeira vacina com o lote igual ao que 
    //foi passado em argumento
    while (atual) {
        if (atual->lote && strcmp(atual->lote, lote) == 0){ 
            return 1;
        }
        atual = atual->next;
    }
    return 0;
}

/**
 * @brief Verifica se uma determinada vacina é válida
 * 
 * Esta função permite validar uma vacina. Primeiramente a função verifica se há
 * espaço para guardar uma nova vacina em sistema. Caso haja espaço verifica se
 * o lote que foi passado em argumento é válido. De seguida verifica se esse
 * mesmo lote já existe em stock. Caso o lote seja válido e não existe em stock,
 * a função verifica se o nome da vacina passado em argumento não excede o 
 * número máximo de caracteres alocado para o nome de vacinas. Seguidamente
 * verifica se a data de validade passada em argumento não é inferior à data
 * atual do sistema. Finalmente verifica se o número de doses passado em 
 * argumento é um número positivo. Caso tudo seja validado a função retorna 1, 
 * caso contrário imprime a mensagem de erro correspondente à primeira 
 * verificação que falhou e retorna 0.
 * 
 * @see loteValido
 * @see loteExiste
 * @see dataValida
 * @see dataMaisAntiga
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
    char lote[],char nome_vacina[], Data data, Data*data_sistema,char lingua[]){
    //Verificação da quantidade de vacinas
    if(num_vacinas == MAX_VACINAS){
        printf(!strcmp(lingua, "pt") ? "demasiadas vacinas\n" : "too many vaccines\n");
        return 0;
    }
    //Verificação do lote (validade e duplicação)
    if (strlen(lote) > MAX_CHAR_LOTE || (!loteValido(lote))) {
        printf(!strcmp(lingua, "pt") ? "lote inválido\n" : "invalid batch\n");
        return 0;
    }
    //Verificação da existência do lote
    if (loteExiste(lote, lista_vacinas)) {
        printf(!strcmp(lingua, "pt") ? "número de lote duplicado\n" : "duplicate batch number\n");
        return 0;
    }
    //Verificação do nome da vacina
    if ((strlen(nome_vacina) + 1) > MAX_BYTES_VACINAS) {
        printf(!strcmp(lingua, "pt") ? "nome inválido\n" : "invalid name\n");
        return 0;
    }
    //Verificação da data de validade
    if ((!dataValida(data)) || (dataMaisAntiga(data, *data_sistema))) {
        printf(!strcmp(lingua, "pt") ? "data inválida\n" : "invalid date\n");
        return 0;
    }
    //Verificação da quantidade das doses
    if (doses < 0) {
        printf(!strcmp(lingua, "pt") ? "quantidade inválida\n" : "invalid quantity\n");
        return 0;
    }
    return 1;
}

/**
 * @brief Verifica se um determinado lote tem stock
 * 
 * Esta função permite verificar se um determinado lote tem stock suficiente 
 * para aplicar uma vacina. Primeiramente a função verifica se a lista de 
 * vacinas está vazia. Caso não esteja, a função percorre a lista de vacinas até
 * encontrar a vacina com o nome correspondente ao que foi passado em argumento
 * e que tenha a data de validade mais próxima de expirar. A função retorna 1 
 * caso haja uma vacina com stock suficiente para aplicar uma vacina, e imprime
 * uma mensagem de erro caso não encontre uma vacina com lote suficiente e
 * retorna 0.
 * 
 * @see dataMaisAntiga
 * @param[in] nome_vacina nome da vacina
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lote lote que se pretende verificar se tem stock
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] data_sistema data atual da simulação
 * @return 1 se o lote tiver stock, 0 caso contrário
 */
int existeStock(char nome_vacina[], Vacinas **lista_vacinas, char lote[],
    char lingua[], Data *data_sistema) {
        
    //Verifica se a lista de vacinas está vazia
    if (*lista_vacinas == NULL) {
        printf(!strcmp(lingua, "pt") ? "esgotado\n" : "no stock\n");
        return 0; 
    }
    
    Vacinas *atual = *lista_vacinas;
    Vacinas *mais_antiga = NULL;
    
    //percorre a lista à procura do nome da vacina
    while (atual != NULL) {
        //verifica se o nome corresponde ao pedido e verifica se a data de 
        //validade não ultrapassou a data de sistema
        if (strcmp(atual->nome_vacina, nome_vacina) == 0 && 
            !dataMaisAntiga(atual->validade, *data_sistema) && 
            (atual->doses > atual->doses_aplicadas)) {

            //Se ainda não encontrar alguma vacina ou verifica se a que foi 
            //encontrada tem validade mais proxima de expirar
            if (mais_antiga == NULL || 
                dataMaisAntiga(atual->validade, mais_antiga->validade)) {
                mais_antiga = atual;
            }
        }
        atual = atual->next;
    }
    //Se foi encontrado alguma vacina válida
    if (mais_antiga != NULL) {
        //Copia o lote
        strcpy(lote, mais_antiga->lote);
        return 1; //Vacina existe e tem stock suficiente
    }
    printf(!strcmp(lingua, "pt") ? "esgotado\n" : "no stock\n");
    return 0; //Não foi encontrada nenhuma vacina válida
}

/**
 * @brief Verifica se um determinado utente já foi vacinado nesse dia com uma
 * determinada vacina. 
 * 
 * Esta função permite verificar se um utente já foi vacinado no dia atual do 
 * sistema com uma determinada vacina.A função vai percorrendo a lista de 
 * inoculações até encontrar o utente com o nome igual ao que foi passado em 
 * argumento. Caso encontre um utente com o mesmo nome ao que foi passado em 
 * argumento, verifica se ele foi vacinado nesse dia com a vacina que foi 
 * passado em argumento. Em caso afirmativo a função imprime uma mensagem de 
 * erro e retorna 1. Caso chegue ao fim da lista sem essas condições se 
 * verificarem retorna 0.
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
    Inoculacoes **lista_inoculacoes, char lingua[]) {

    Inoculacoes *atual = *lista_inoculacoes;

    while (atual != NULL) {
        // Verifica se é o mesmo utente, a mesma vacina e se foi aplicada 
        // no mesmo dia
        if (strcmp(nome_utente, atual->nome_utente) == 0 &&
            strcmp(nome_vacina, atual->nome_vacina) == 0 &&
            (atual->data_aplicacao.dia == data_sistema->dia &&
            atual->data_aplicacao.mes == data_sistema->mes &&
            atual->data_aplicacao.ano == data_sistema->ano)) {
            printf(!strcmp(lingua, "pt") ? "já vacinado\n" : 
            "already vaccinated\n");
            return 1; //O utente já foi vacinado com esta vacina hoje
        }
        atual = atual->next;
    }
    
    return 0; //O utente ainda não foi vacinado hoje com esta vacina
}

/**
 * @brief Processa uma determinada aplicação de uma vacina num determinado
 * utente
 * 
 * Esta função permite processar uma determinada aplicação de uma vacina a um 
 * utente. Primeiramente a função verifica se há stock suficiente para uma 
 * aplicação. De seguida verifica se o utente já foi vacinado nesse dia com a 
 * mesma vacina. Caso essas 2 condições sejam favoráveis, procede a atualizar
 * o número de doses aplicadas desse lote e insere o novo registo de inoculação
 * na lista de inoculações. A função imprime o lote que foi aplicado caso a 
 * aplicação tenha sido bem sucedida e retorna 1. Caso alguma das verificações 
 * tenham sido desfavoráveis a função imprime uma mensagem de erro e retorna 0.
 * 
 * @see existeStock
 * @see jaFoiVacinado
 * @see vacina
 * @see insereAplicacaoOrdenado
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
    char *nome_utente, Inoculacoes *aplicacao) {

    char nome_vacina[MAX_BYTES_VACINAS], lote[MAX_CHAR_LOTE+1] = {0};
    // Lê o nome da vacina
    if((scanf("%s", nome_vacina)) != 1) return 0;
    // Verifica se existe stock da vacina
    if(!existeStock(nome_vacina, lista_vacinas, lote, lingua, data_sistema))
        return 0;
    // Verifica se o utente já foi vacinado com esta vacina
    if(jaFoiVacinado(nome_vacina, nome_utente, data_sistema, lista_inoculacoes,
        lingua))
        return 0;
    // Atualiza o stock da vacina
    vacina(lote, lista_vacinas);
    // Preenche os campos da estrutura de inoculação
    strcpy(aplicacao->nome_utente, nome_utente);
    strcpy(aplicacao->lote, lote);
    strcpy(aplicacao->nome_vacina, nome_vacina);
    aplicacao->data_aplicacao = *data_sistema;
    // Insere a nova inoculação na lista ordenada
    insereAplicacaoOrdenado(lista_inoculacoes, aplicacao);
    // Imprime o lote da vacina aplicada
    printf("%s\n", lote);
    return 1;
}

/**
 * @brief Verifica se um utente tem registo
 * 
 * Esta função permite verificar se um utente está na lista de inoculações.
 * A função vai percorrendo a lista de inoculações até encontrar um utente com
 * o mesmo nome que foi passado em argumento. Caso não encontre nenhum registo
 * imprime uma mensagem de erro e retorna 0. No caso de existir um utente com
 * esse nome na lista de inoculações retorna 1.
 * 
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] nome_utente nome do utente que se pretende verificar
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se o utente já tem registo, 0 caso contrário
 */  
int utenteTemRegisto(Inoculacoes **lista_utentes, char nome_utente[],
    char lingua[]){
    
    Inoculacoes *atual = *lista_utentes;
    //Percorre a lista à procura do primeiro registo com o nome do utente igual
    //ao que foi passado em argumento
    while(atual != NULL){
        if(strcmp(atual->nome_utente, nome_utente) == 0){
           return 1;
        }
        atual = atual->next;
    }
    printf(!strcmp(lingua, "pt") ? "%s: utente inexistente\n" : 
    "%s: no such user\n", nome_utente);
    //devolve 0 caso não encontre nenhum registo com o nome de utente passado
    return 0; 
    
}

/**
 * @brief Verificação da data e do lote de um registo de inoculação a apagar
 * 
 * Esta função verifica se a data e o lote de uma inoculação que se pretende 
 * apagar são válidas. Em primeiro lugar a função verifica se a data que é 
 * passada é válida e se a data atual do sistema é mais antiga que a data 
 * passada em argumento. Em caso afirmativo imprime uma mensagem de erro e 
 * retorna 0. Seguidamente verifica se o lote que foi passado em argumento
 * existe. Em caso de não existir imprime uma mensagem de erro e retorna 0.
 * Caso as verificações sejam favoráveis ao que se pertende retorna 1.
 * 
 * @see dataValida
 * @see dataMaisAntiga
 * @see loteExiste
 * @param[in] data data a verificar
 * @param[in] data_sistema data atual da simulação
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @return 1 se os parâmetros são válidos, 0 caso contrário
 */  
int validaDataLote(Data *data, char *lote, Data *data_sistema,
    Vacinas **lista_vacinas, char lingua[]) {
    
    //Verifica se a data passada é válida e se é mais antiga que a data atual de
    //sistema
    if (!dataValida(*data) || dataMaisAntiga(*data_sistema, *data)) {
        printf(!strcmp(lingua, "pt") ? "data inválida\n" : "invalid date\n");
        //Retorna 0 caso a data não seja válida ou caso a data passada não
        //seja mais antiga que a data atual de sistema
        return 0;
    }
    //Verifica se o lote existe
    if (!loteExiste(lote, lista_vacinas)) {
        printf(!strcmp(lingua, "pt") ? "%s: lote inexistente\n" : 
        "%s no such batch\n", lote);
        //Retorna 0 caso o lote não exista
        return 0;
    }
    //Retorna 1 caso a data seja válida e caso o lote exista
    return 1;
}