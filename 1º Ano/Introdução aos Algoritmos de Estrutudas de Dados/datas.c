/**
 * @file datas.c
 * @brief Funções que efectuam calculos sobre datas
 * @author ist157175 (João Carvalho)
 */

#include "datas.h"
/**
 * @brief Função que verifica se a data é válida
 *
 * Esta função recebe uma data e verifica se, primeiramente, o ano é igual ou
 * superior a 2025 (visto que o inicio da simulação é o primeiro dia de 2025),
 * depois verifica se o ano é bissexto, se fôr altera o número de dias que o mês
 * de fevereiro pode ter, verifica se o número que representa o mês está entre 1
 * e 12 e finalmente verifica se o número que representa o dia é maior ou igual
 * a 1 e se é menor ou igual ao número máximo de dias correspondente ao mês em 
 * que essa data está.
 * 
 * @see anoBisexto
 * @param[in] data Data a verificar
 * @return 1 caso a data seja valida, 0 caso contrário
 */

int dataValida(Data data){
    int dias_por_mes[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    //Verificação do ano 2025, pois a data inicial do sistema é o primeiro dia 
    //de 2025, caso seja anterior sai logo da função
    if(data.ano >= 2025){ 
        // Verifica se o ano é bissexto e se sim, muda o número de dias do mês 
        // de fevereiro para 29
        if(anoBisexto(data.ano)){ 
            dias_por_mes[1] = 29;
        }
        //Verifica se o mês e o dia estão dentro dos valores válidos
        if(data.mes >= 1 && data.mes <= 12){
            if(data.dia >=1 && data.dia <= dias_por_mes[data.mes - 1])
                return 1;
        }            
    }   
    return 0;

}
/**
 * @brief Função que lê e imprime uma data
 *
 * Esta função recebe uma data e imprime para o standard output na forma
 * dd-mm-aaaa
 *
 * @param[in] data Data a imprimir
 * @return void (a função não retorna valores)
 */

void escreveData(Data data){
    printf("%02d-%02d-%d\n", data.dia, data.mes, data.ano);
}
/**
 * @brief Função que verifica se um ano é bissexto
 *
 * Esta função recebe um ano e verifica se é um ano bissexto, ou seja, vamos
 * utilizar a regra do calendário gregoriano: o ano tem que ser divisivel por 4 
 * mas não por 100 ou tem que ser divisivel por 400 para ser considerado um ano
 * bissexto.
 * 
 * @param[in] ano ano a verificar
 * @return 1 se o ano for bissexto, 0 caso contrário
 */

int anoBisexto(int ano){
    if((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0){
        return 1;
    }
    else{
        return 0;
    }
}

/**
 * @brief Função que verifica se a data passada no primeiro argumento é mais 
 * antiga que a que é passada no segundo argumento
 *
 * Esta função recebe duas datas e verifica se a data que é passada no primeiro
 * argumento é mais antiga que que é passada no segundo argumento, para isso, 
 * verifica se o ano da data1 é menor que o ano da data2, em caso de igualdade,
 * verifica se o mês da data1 é menor que o mês da data2, e em caso de
 * igualdade, verifica se o dia da data1 é menor que o dia da data2.
 *
 * @param[in] data1 data que se quer verificar se é mais antiga
 * @param[in] data2 data que de comparação
 * @return 1 se data1 fôr mais antiga que a data2, 0 caso contrário
 */


int dataMaisAntiga(Data data1, Data data2) {
    if (data1.ano < data2.ano) {
        return 1; 
    }
    else if (data1.ano == data2.ano && data1.mes < data2.mes) {
        return 1; 
    }
    else if (data1.ano == data2.ano && data1.mes == data2.mes &&
             data1.dia < data2.dia) {
        return 1; 
    }
    
    else
        return 0; 
}