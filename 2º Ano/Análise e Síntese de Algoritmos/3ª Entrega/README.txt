Projeto Análise e Síntese de Algoritmos — P3 (2024/2025)

Terceiro projeto da cadeira de Análise e Síntese de Algoritmos (ASA). O objetivo foi
resolver, com Programação Linear Inteira, o problema de distribuição ótima de brinquedos
de Natal da empresa do Pai Natal (UbiquityInc): dadas várias fábricas (cada uma com stock
de um tipo de brinquedo), países (com um mínimo de presentes a entregar e um limite de
exportação) e crianças (que pedem brinquedos e recebem no máximo um), maximizar o número
de crianças cujos pedidos são satisfeitos.

Implementação em Python com a biblioteca PuLP (solver GLPK). Lê a instância do standard
input e escreve o número máximo de crianças satisfeitas, ou -1 se for impossível respeitar
as restrições.

Nota Histórica: A submissão original obteve 19,5 valores. A funcionalidade estava totalmente
correta, mas faltou 0,5 num único teste de grande dimensão com limite de tempo apertado — que
só passava quem construísse o modelo LP em tempo linear. A versão original construía duas das
restrições com ciclos aninhados (O(n^2)), o que rebentava o tempo nessa instância.

📦 Versão Submetida em 2025 (Nota: 19,5/20)

Modelação LP correta e robusta, a passar todos os testes exceto o maior.
✅ O que estava BEM:

    Modelação correta: variáveis binárias x[criança, fábrica], função objetivo (maximizar
    atribuições) e as 4 restrições do problema (1 presente por criança, stock por fábrica,
    mínimo de presentes por país, máximo de exportação por país).

    Pré-processamento inteligente: filtragem de fábricas sem stock e de pedidos inválidos
    logo na leitura, e deteção antecipada de inviabilidade (mínimos por país).

    Uso de um índice auxiliar (pares_por_crianca) para a restrição "1 presente por criança".

🛠️ O que foi Melhorado (correções 2026)

Esta revisão focou-se exatamente no ponto que custou os 0,5 valores: o desempenho da
construção do modelo no teste grande.

1. Eliminação dos ciclos aninhados nas Restrições 2 e 4 (O(n^2) -> O(n))

    O Erro Original: as restrições de stock por fábrica (R2) e de exportação por país (R4)
    eram construídas percorrendo TODA a lista de pares (criança, fábrica) para CADA fábrica
    e para CADA país. Cada par era revisitado N vezes -> complexidade quadrática.

    A Correção: pré-cálculo dos índices numa única passagem (pares_por_fabrica e
    export_por_pais), passando a construir cada restrição com um único ciclo linear. A
    construção do modelo passou de ~O(n^2) para O(n).

    Impacto medido: num benchmark de escala (n = 500 a 2000), a construção antiga crescia de
    forma quadrática (0,2s -> 7,9s) enquanto a nova cresce linearmente (0,07s -> 1,4s) — um
    ganho que chega a 5,6x e continua a aumentar com o tamanho. É precisamente esta margem de
    tempo que faltava para passar o teste grande.

2. Limpeza da Restrição 3

    Ajuste Técnico: remoção de uma condição redundante (if (c,f) in x) que era sempre
    verdadeira, tornando o código mais limpo sem alterar o comportamento.