Projeto Análise e Síntese de Algoritmos — P2 (2024/2025)

Segundo projeto da cadeira de Análise e Síntese de Algoritmos (ASA). O problema, ambientado no
estudo da rede de metro de Caracolândia: dada a rede (um multi-grafo não-dirigido em que cada
aresta pertence a uma linha de metro), calcular o índice de conectividade mc — o número MÁXIMO,
sobre todos os pares de estações, do número MÍNIMO de mudanças de linha necessárias para viajar
entre elas.

Casos especiais: o output é 0 se nunca for necessária qualquer mudança de linha, e -1 se
existirem duas estações sem ligação entre si (rede desconexa).

A ideia central é modelar o custo como "mudança de linha" (mover-se dentro da mesma linha não
custa; trocar de linha custa 1) e calcular de forma eficiente o "diâmetro" da rede nesse custo —
um problema de grafos.

Implementação em C++ (algoritmo iterativo). Lê do standard input e escreve para o standard
output.

Nota Histórica: A submissão original obteve 19,7 valores. O código estava perfeito (passou todos
os testes de execução); os 0,3 em falta foram apenas na componente de Relatório, não no código.

📦 Versão Submetida em 2024 (Nota: 19,7/20 — execução perfeita; 0,3 no relatório)

✅ O que estava BEM:

    Modelação do custo por mudança de linha: transformação do problema de forma a que o custo de
    um percurso seja o número de vezes que se troca de linha (0 dentro da mesma linha).

    Cálculo eficiente do índice de conectividade (o máximo dos mínimos), com tratamento correto
    dos casos-limite: 0 quando não há mudanças, -1 quando a rede é desconexa.

    Implementação iterativa em C++, dentro dos limites de tempo e memória.