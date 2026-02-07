/**
 * @file func_aux.c
 * @brief Funções auxiliares para o fucionamento do programa principal
 * @author ist157175 (João Carvalho)
 */
#include "func_aux.h"
/**
 * @brief Lê o nome de um utente a partir do standard input
 *
 * Esta função lê caracteres do standard input para obter o nome de um utente.
 * O nome pode estar entre aspas (") ou não. No caso de estar entre aspas,
 * o conteúdo entre aspas é considerado o nome com espaços em branco, caso haja. 
 * Caso contrário, apenas a primeira palavra (até encontrar espaço, tabulação ou 
 * fim de linha) é considerada como nome.
 *
 * @param[in,out] size_med Apontador para o tamanho inicial do buffer.
 * Após a execução, contém o tamanho final alocado.
 * @return Apontador para a string ou NULL em caso de falha na alocação
 */
char* leNomeUtente(int *size_med) {
	char *nome = malloc(*size_med);
	if (nome == NULL)
		return NULL;
	
	int i = 0, c;
	nome[0] = getchar();
	
	// Ignorar espaços iniciais
	while (nome[0] == ' ' || nome[0] == '\t')
		nome[0] = getchar();
	
	// Define índice inicial e delimitador baseado no primeiro caráctere
	char aspas = (nome[0] == '"') ? '"' : ' ';
	i = (aspas == '"') ? 0 : 1;
	
	// Lê caracteres até encontrar o delimitador
	while ((c = getchar()) != aspas && c != '\n' && c != '\t' && c != EOF) {
		if (i >= *size_med - 1) {
			*size_med *= 2;
			char *temp = realloc(nome, *size_med);
			if (temp == NULL) {
				free(nome);
				return NULL;
			}
			nome = temp;
		}
		nome[i++] = c;
	}
	
	// Se não for aspas, devolve o caráctere ao buffer
	if (aspas != '"' && c != EOF)
		ungetc(c, stdin);
	
	nome[i] = '\0';
	return nome;
}
/**
 * @brief Atualiza o número de doses aplicadas de um lote de vacina
 *
 * Esta função procura um lote específico de vacina na lista ligada de vacinas
 * e incrementa o contador de doses aplicadas quando o lote é encontrado.
 * A função percorre a lista ligada até encontrar o lote correspondente ou
 * até chegar ao fim da lista.
 *
 * @param[in] lote Código identificador do lote de vacina a procurar
 * @param[in,out] lista_vacinas Ponteiro para a lista de vacinas
 * @return void (a função não retorna valores)
 */
void vacina(char lote[], Vacinas **lista_vacinas) {
	Vacinas *atual = *lista_vacinas;
	while (atual != NULL) {
		if (strcmp(lote, atual->lote) == 0) {
			//quando encontra o lote, atualiza as doses aplicadas e sai
			atual->doses_aplicadas++;
			return;
		}
		atual = atual->next;
	}
}