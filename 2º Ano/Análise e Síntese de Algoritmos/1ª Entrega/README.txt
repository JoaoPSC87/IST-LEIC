Projeto Análise e Síntese de Algoritmos — P1 (2024/2025)

Primeiro projeto da cadeira de Análise e Síntese de Algoritmos (ASA). O problema, ambientado
na descoberta do arqueólogo João Caracol no Templo Perdido da civilização TuttiFrutti: dado um
operador binário ⊕ definido por uma tabela n×n sobre inteiros (não associativo nem comutativo),
uma sequência de m inteiros e um resultado desejado, decidir se é possível parentetizar a
sequência de forma a que a expressão dê esse resultado — e, em caso afirmativo, apresentar a
parentização mais à esquerda.

Como o operador é definido por uma tabela arbitrária, a ordem das operações importa, o que
torna este num caso clássico de Programação Dinâmica sobre intervalos.

Implementação em C++ (algoritmo iterativo, para não esgotar a pilha nos testes de maior
dimensão). Lê do standard input e escreve para o standard output.

Nota Histórica: A submissão original obteve a nota máxima (20/20). O código passou todos os
testes de execução dentro dos limites de tempo e memória.

📦 Versão Submetida em 2024 (Nota: 20/20)

✅ O que estava BEM:

    Modelação por Programação Dinâmica: para cada subsequência [i,j], cálculo do conjunto de
    valores atingíveis ao parentetizá-la, combinando os resultados das duas metades através da
    tabela do operador.

    Reconstrução da solução: a partir da tabela de DP, obtenção da parentização mais à esquerda
    que atinge o resultado desejado.

    Implementação iterativa e eficiente em C++, respeitando os limites de tempo/memória e o
    formato de saída exigido.