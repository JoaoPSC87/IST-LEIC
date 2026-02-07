/* iaed25 - ist157175 - project */
/**
 * @file project.c
 * @brief Funções principais do programa
 * @author ist157175 (João Carvalho)
 * 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include "comandos.h"
 #include "func_aux.h"
 #include "funcs_lista.h"
 #include "funcs_delete.h"
 #include "funcs_insert.h"
 #include "funcs_verificacao.h"
 
 /**
  * @brief Processa os comandos introduzidos pelo utilizador
  *
  * Função que executa as operações conforme o comando inserido pelo utilizador.
  * Os comandos disponíveis são:
  * - c: introduz novo lote de vacinas
  * - l: lista vacinas disponíveis
  * - a: aplica vacina a utente
  * - r: retira lote de vacinas
  * - d: apaga registo de vacina
  * - u: lista aplicações de utente
  * - t: avança tempo do sistema
  *
  * @see adicionaLote
  * @see listaVacinas
  * @see aplicaVacina
  * @see retiraLote
  * @see apagaRegisto
  * @see listaInoculacoes
  * @see avancarTempo
  * @param lista_vacinas Ponteiro para a lista de vacinas
  * @param lista_inoculacoes Ponteiro para a lista de inoculações
  * @param data_sistema Ponteiro para a data atual do sistema
  * @param num_vacinas Número atual de vacinas registadas
  * @param comando Comando a executar
  * @param lingua linguagem a utilizar nas mensagens de erro
  * @return Número atualizado de vacinas após a execução do comando
  */
 
 int comandos(Vacinas **lista_vacinas, Inoculacoes **lista_inoculacoes,
             Data *data_sistema, int num_vacinas,char comando, char lingua[]) {
     switch (comando) {
         // introduz novo lote
         case 'c': num_vacinas = adicionaLote(lista_vacinas, data_sistema,
                                 lingua, num_vacinas);
             break;
         // lista vacinas
         case 'l': listaVacinas(lista_vacinas, lingua);
             break;
         // aplica vacina a utente
         case 'a': aplicaVacina(lista_vacinas, lista_inoculacoes, data_sistema,
                     lingua);
             break;
         // retira lote
         case 'r': num_vacinas = retiraLote(lista_vacinas, lingua, num_vacinas);
             break;
         // apaga registo de vacina
         case 'd': apagaRegisto(lista_inoculacoes, lista_vacinas, data_sistema,
                     lingua);
             break;
         // lista aplicações de utente
         case 'u': listaInoculacoes(lista_inoculacoes, lingua);
             break;
         // avança tempo
         case 't': Data *nova_data = avancarTempo(data_sistema, lingua);
             if (nova_data) {
                 *data_sistema = *nova_data;
                 free(nova_data);
             }
             break;
         }
     return num_vacinas;
 }
 
 /**
  * @brief Função principal do programa
  *
  * Inicializa as estruturas de dados, processa os comandos introduzidos pelo 
  * utilizador e liberta a memória no final da execução. O programa termina 
  * quando recebe o comando 'q' ou EOF.
  *
  * @see comandos
  * @see destroyVacinas
  * @see destroyInoculacoes
  * @param argc Número de argumentos da linha de comando
  * @param argv Array de strings com os argumentos da linha de comando
  * @return 0 em caso de sucesso, diferente de 0 em caso de erro
  */
 int main(int argc, char *argv[]) {
     int num_vacinas = 0;
     char comando, *lingua = "en";
     Inoculacoes *lista_inoculacoes = NULL;
     Vacinas *lista_vacinas = NULL;
     Data *data_sistema = malloc(sizeof(Data));
 
     // Verificação do número de argumentos do programa
     if (argc > 2)
         return 0;
     else {
         // Caso o segundo argumento exista, a linguagem das mensagens de erro
         // passa para português, senão mantem a linguagem default (inglês)
         if (argc == 2 && strcmp(argv[1], "pt") == 0)
             lingua = "pt";
 
         if (!data_sistema) {
             printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");
             return 0;
         }
 
         // Inicialização da data do sistema
         data_sistema->dia = 1;
         data_sistema->mes = 1;
         data_sistema->ano = 2025;
 
         // chamada da função que executa os comandos
         while ((comando = getchar()) != EOF && comando != 'q') {
             num_vacinas = comandos(&lista_vacinas, &lista_inoculacoes,
                             data_sistema, num_vacinas, comando, lingua);
         }
         // Libertação da memória alocada
         destroyVacinas(&lista_vacinas);
         destroyInoculacoes(&lista_inoculacoes);
         free(data_sistema);
         return 0;
     }
 }
 