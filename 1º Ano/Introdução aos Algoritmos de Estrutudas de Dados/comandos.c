/**
 * @file comandos.c
 * @brief Funções que executam os comandos dados pelo utilizador
 * @author ist157175 (João Carvalho)
 * 
 */
#include "comandos.h"
#include "func_aux.h"
#include "funcs_lista.h"
#include "funcs_delete.h"
#include "funcs_insert.h"
#include "funcs_verificacao.h"
 /**número máximo de bytes que o nome da vacina pode ter*/
#define MAX_BYTES_VACINAS 51
/**número máximo de caracteres que o lote pode ter*/
#define MAX_CHAR_LOTE 20 
/**número médio de bytes que o nome do utente pode ter*/
#define MED_BYTES_UTENTE 201 
/**número máximo de caractéres que o input pode ter*/
#define MAX_CHARS 65535 

/**
 * @brief Adiciona uma vacina à lista de vacinas
 * 
 * Esta função permite adicionar uma nova vacina à lista de vacinas. Em primeiro
 * lugar, a função verifica se os dados da vacina a adicionar são válidos. Caso
 * os dados passados sejam válidos a função insere de maneira ordenada a nova
 * vacina na lista das vacinas, imprime o lote da vacina e retorna o número 
 * atualizado de vacinas guardadas em sistema.
 * 
 * @see vacinaValida
 * @see inserirOrdenado
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] num_vacinas número atual de vacinas guardadas em sistema
 * @return O número atual de vacinas guardadas em sistema
 */
int adicionaLote(Vacinas **lista_vacinas, Data *data_sistema, char lingua[],
    int num_vacinas) {

    char lote[MAX_CHARS], nome[MAX_CHARS];
    Data data;
    int doses;
    //Leitura dos dados
    if((scanf("%s %d-%d-%d %d %s", lote, &data.dia, &data.mes, &data.ano,
        &doses, nome)) != 6) return num_vacinas;
    //Verifica se os parametros da vacina são válidos
    if(!vacinaValida(lista_vacinas, num_vacinas, doses, lote, nome, data,
        data_sistema, lingua)) return num_vacinas;
    //Alocação e inicialização da estrutura principal
    Vacinas *vacina = malloc(sizeof(Vacinas));
    if (!vacina) {
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        exit(1);
    }
    //Inserção dos dados da nova vacina na estrutura para poder ser introduzida 
    //na lista das vacinas
    vacina->nome_vacina = strdup(nome);
    vacina->lote = strdup(lote);
    if (!vacina->nome_vacina || !vacina->lote) {
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        free(vacina);
        exit(1);
    }
    vacina->validade = data;
    vacina->doses = doses;
    vacina->doses_aplicadas = 0;
    vacina->next = NULL;
    // Inserção na lista
    inserirOrdenado(lista_vacinas, vacina);
    printf("%s\n", vacina->lote);
    return num_vacinas += 1;
}

/**
 * @brief Faz a listagem das vacinas
 * 
 * Esta função permite fazer a listagem das vacinas que estão guardadas em
 * sistema. A função em primeiro lugar verifica se é passado algum nome de uma
 * vacina. Caso não seja passado nenhum nome a função imprime a lista toda de
 * vacinas. Caso seja passado um nome, a função faz a listagem apenas das 
 * vacinas com esse nome.
 * 
 * @see listaTodasVacinas
 * @see listaVacinasNome
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void listaVacinas(Vacinas **lista_vacinas, char lingua[]) {
    char nome[MAX_BYTES_VACINAS];
    //Procura o primeiro carácter não branco
    char c = getchar();
    if (c == '\n' || c == EOF) {
        //Se não tiver argumentos, imprime a lista toda
        listaTodasVacinas(lista_vacinas);
        return;
    }
    ungetc(c, stdin); //repõe os caracteres para o scanf
    
    //lê o nome das vacinas até encontrar um '\n' ou um EOF
    while (scanf("%s", nome) == 1) {
        listaVacinasNome(lista_vacinas, nome, lingua);
        c = getchar();
        if (c == '\n' || c == EOF) {
            break;
        }
        ungetc(c, stdin); //repõe o carácter para o scanf
    }
}

/**
 * @brief Aplica uma vacina a um utente
 * 
 * Esta função permite aplicar uma vacina a um utente. A função começa por ler o
 * nome do utente, seguidamente verifica se a vacina tem stock suficiente e se
 * o utente já foi vacinado nesse dia pela mesma vacina. Caso corra tudo bem, a 
 * função atualiza o stock da vacina e imprime o lote que foi aplicado.
 *
 * @see leNomeUtente
 * @see processaAplicacaoVacina 
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void aplicaVacina(Vacinas **lista_vacinas, Inoculacoes **lista_inoculacoes,
    Data *data_sistema, char lingua[]) {

    // Variáveis para armazenar os dados da vacina e do utente
    int size_med = MED_BYTES_UTENTE, sucesso;
    char *nome_utente = leNomeUtente(&size_med);
    // Aloca memória para uma nova inoculação
    Inoculacoes *aplicacao = malloc(sizeof(Inoculacoes));
    if (!aplicacao || !nome_utente) {
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        free(nome_utente);
        exit(1);
    }
    // Aloca memória para os campos da estrutura de inoculação
    aplicacao->nome_utente = malloc(strlen(nome_utente) + 1);
    aplicacao->lote = malloc(MAX_CHAR_LOTE + 1);
    aplicacao->nome_vacina = malloc(MAX_BYTES_VACINAS);
    // Verifica se todas as alocações foram bem-sucedidas
    if(!aplicacao->nome_utente || !aplicacao->nome_vacina || !aplicacao->lote) {
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        free(nome_utente);
        free(aplicacao);
        exit(1);
    }
    // Verificação e validação da vacina e do utente
    sucesso = processaAplicacaoVacina(lista_vacinas, lista_inoculacoes,
            data_sistema, lingua, nome_utente, aplicacao);
    // Liberta memória com base no resultado
    free(nome_utente);
    if (!sucesso) {
        free(aplicacao->nome_utente);
        free(aplicacao->nome_vacina);
        free(aplicacao->lote);
        free(aplicacao);
    }
}

/**
 * @brief Retira uma vacina à lista de vacinas
 * 
 * Esta função permir apagar um determinado lote da lista de vacinas. Em 
 * primeiro lugar a função verifica se o lote existe. Em caso afirmativo a 
 * função retira a disponibilidade desse lote, atualiza o número de vacinas
 * guardadas em sistema e imprime o número de aplicações que foram 
 * realizadas com esse lote.
 * 
 * @see loteExiste
 * @see apagaLote
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] lingua linguagem das mensagens de erro
 * @param[in] num_vacinas número atual de vacinas guardadas em sistema
 * @return O número atual de vacinas guardadas em sistema
 */
int retiraLote(Vacinas **lista_vacinas, char lingua[], int num_vacinas){
    //Variável para armazenar o código do lote a ser removido
    char lote[MAX_CHAR_LOTE + 1];
    
    //Variável para armazenar o número de aplicações realizadas com o lote
    int num_aplicacoes;
    
    //Lê o código do lote a ser removido
    scanf("%s", lote);
    
    //Verifica se o lote existe no sistema
    if(!loteExiste(lote, lista_vacinas)){
        //Se o lote não existir, exibe mensagem de erro na língua apropriada
        printf(!strcmp(lingua, "pt") ? "%s: lote inexistente\n" : 
        "%s: no such batch\n", lote);
        //Retorna o número de vacinas sem alteração
        return num_vacinas;
    }
    
    //Remove o lote e obtém o número de aplicações realizadas com este lote
    num_aplicacoes = apagaLote(lista_vacinas, lote);
    
    //Reduz o contador total de vacinas
    num_vacinas--;
    
    //Exibe o número de aplicações que foram realizadas com este lote
    printf("%d\n", num_aplicacoes);
    
    //Retorna o número atualizado de vacinas
    return num_vacinas;
}

/**
 * @brief Apaga registos de inoculações de um utente
 * 
 * Esta função permite apagar registos de inoculação de um utente. Caso seja
 * passado em argumento uma data e um lote, apaga os registos desse utente para
 * esse dia com esse lote, caso só seja passado uma data apaga os registos desse
 * dia, e caso só seja passado o nome do utente apaga todos os registos desse 
 * utente. Em primeiro lugar a função lê o nome do utente e verifica se esse
 * utente já tem registo. De seguida faz a verificação dos argumentos que são
 * passados, valida os dados de entrada e caso os dados sejam válidos procede à
 * remoção dos registos. A função imprime o número de registos que foram 
 * apagados.
 * 
 * @see leNomeUtente
 * @see utenteTemRegisto
 * @see validaDataLote
 * @see apagaInoculacoesUtenteLote
 * @see dataValida
 * @see dataMaisAntiga
 * @see apagaInoculacoesUtenteData
 * @see apagaInoculacoesUtente
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */
void apagaRegisto(Inoculacoes **lista_inoculacoes, Vacinas **lista_vacinas,
    Data *data_sistema, char lingua[]){
    // Variáveis para armazenar os dados da pesquisa
    int size_med = MED_BYTES_UTENTE,apagados = 0;
    char input[MAX_CHARS], lote[MAX_CHAR_LOTE + 1], 
    *nome_utente = leNomeUtente(&size_med);
    Data data;
    if(!nome_utente){
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        exit(1);
    }
    while(1){
        // Verifica se o utente tem registos de inoculação
        if(!utenteTemRegisto(lista_inoculacoes, nome_utente, lingua)) break;
        // Lê a linha de entrada para processar parâmetros adicionais
        // (data e/ou lote)
        fgets(input, sizeof(input), stdin);
        // Tenta interpretar a entrada como "data lote"
        if(sscanf(input,"%d-%d-%d %s",&data.dia,&data.mes, &data.ano, lote)==4){    
            if(validaDataLote(&data, lote, data_sistema, lista_vacinas, lingua))
                apagados=apagaInoculacoesUtenteLote(lista_inoculacoes,nome_utente,&data,lote);
            else break;
        }else if(sscanf(input, "%d-%d-%d", &data.dia, &data.mes, &data.ano)==3){
            if (!dataValida(data) || dataMaisAntiga(*data_sistema, data)) {
                printf(!strcmp(lingua,"pt")?"data inválida\n":"invalid date\n");
                break;
            }
            apagados=apagaInoculacoesUtenteData(lista_inoculacoes,nome_utente,&data);
        } else apagados = apagaInoculacoesUtente(lista_inoculacoes,nome_utente);
        printf("%d\n", apagados);
        break;
    }
    free(nome_utente);
}

/**
 * @brief Faz a listagem dos registos de inoculações
 * 
 * Esta função permite listar os registos de inoculações. Caso seja passado um 
 * nome de utente a função lista todos os registos desse utente, caso não seja
 * passado nenhum nome a função lista todos os registos guardados em sistema.
 * A função em primeiro lugar verifica o número de argumentos passados pelo
 * utilizador e de seguida procede de acordo com o número de argumentos. 
 * 
 * @see listaTodosUtentes
 * @see leNomeUtente
 * @see listaUtenteNome
 * @param[in,out] lista_inoculacoes Ponteiro para a lista de inoculações
 * @param[in] lingua linguagem das mensagens de erro
 * @return void (a função não retorna valores)
 */  
void listaInoculacoes(Inoculacoes **lista_inoculacoes, char lingua[]){
    
    int size_med = MED_BYTES_UTENTE;
    
    //Procura o primeiro carácter não branco
    char c = getchar();
    if (c == '\n' || c == EOF) {
        // Se não tiver argumentos, imprime a lista toda
        listaTodosUtentes(lista_inoculacoes);
        return;
    }
    //lê o nome do utente
    else{
        char *nome = leNomeUtente(&size_med);
        if(!nome){
            printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
            exit(1);
        }
        //lista os registos de inoculação do utente
        listaUtenteNome(lista_inoculacoes, nome, lingua);
        free(nome);
    }  
}

/**
 * @brief Avança a data da simulação
 * 
 * Esta função permite mostrar a data atual da simulação, caso não seja passado 
 * nenhum argumento, ou avançar a data do sistema, caso a data passada seja
 * válida.
 * 
 * @see dataValida
 * @see dataMaisAntiga
 * @see escreveData
 * @param[in] data_sistema data atual da simulação
 * @param[in] lingua linguagem das mensagens de erro
 * @return ou retorna NULL ou retorna a nova data para atualizar o sistema
 */
Data *avancarTempo(Data *data_sistema, char lingua[]){
    //Aloca memória para a nova data
    Data *data = malloc(sizeof(Data));
    if(!data){
        printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
        exit(1);
    }
    
    //Variável para armazenar a entrada do utilizador
    char input[MAX_CHARS];
    
    //Lê a linha de entrada contendo a nova data
    fgets(input, sizeof(input), stdin);
    
    //Tenta interpretar a entrada como uma data no formato "dia-mês-ano"
    if(sscanf(input, "%d-%d-%d", &data->dia, &data->mes, &data->ano) == 3){
        //Verifica se a data é válida e não é anterior à data atual do sistema
        if(dataValida(*data) && !dataMaisAntiga(*data, *data_sistema)){
            //Imprime a nova data
            escreveData(*data);
            //Retorna a nova data para atualizar o sistema
            return data;
        }
        else {
            //Se a data for inválida, exibe mensagem de erro
            printf(!strcmp(lingua,"pt") ? "data inválida\n" : "invalid date\n");
            //Liberta a memória alocada
            free(data);
        }
    }
    else{
        //Se o formato da entrada for inválido, mantém a data atual
        //Imprime a data atual do sistema
        escreveData(*data_sistema);
        //Liberta a memória alocada
        free(data);
    }
    
    //Retorna NULL para indicar que a data do sistema não foi alterada
    return NULL;
}