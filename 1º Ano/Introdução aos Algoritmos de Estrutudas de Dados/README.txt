Projeto 1: Introdução aos Algoritmos e Estruturas de Dados (C) - 2024/25

Aluno: João Pedro Carvalho (57175)
1. Resumo da Submissão

Desenvolvimento de um sistema de gestão de vacinas e inoculações para o sistema de saúde. O programa processa lotes de vacinas, gere stocks, controla datas de validade e regista aplicações a utentes, suportando comandos complexos de remoção e listagem cronológica.

    Linguagem: C (C99)

    Estrutura: Modular (12+ ficheiros entre headers e código fonte)

    Nota do Projeto: 18.5 / 20.0

    Nota Final (após Teste Prático): 19.37 / 20.0

2. Análise de Performance e Estruturas de Dados

Estrutura	Implementação	Eficiência (Busca)
Lotes de Vacinas	Lista Ligada Simples (Ordenada)	O(N)
Inoculações	Lista Ligada Simples (Cronológica)	O(N)
Nomes/Strings	Alocação Dinâmica (malloc/realloc)	O(1) Memória

3. O que foi melhorado e Pontos de Falha
✅ Destaques Positivos

    Gestão de Memória Impecável: O projeto cumpre o requisito estrito de libertar toda a memória dinâmica antes de terminar (destroyVacinas, destroyInoculacoes, etc.), garantindo 0 memory leaks no Valgrind.

    Parsing de Input Robusto: A função leNomeUtente lida com nomes entre aspas e espaços de forma dinâmica, utilizando uma estratégia de expansão de buffer (realloc ao dobro) para garantir eficiência.

    Modularidade: Ao contrário da abordagem comum de um ficheiro único, este projeto foi dividido por responsabilidades (datas.c, funcs_delete.c, funcs_verificacao.c), facilitando a manutenção e o debug.

⚠️ O Ponto Crítico: A Falta de Hash Tables

A nota de 18.5 deveu-se à não implementação de Tabelas de Dispersão (Hash Tables).

    Consequência: Em testes com volumes massivos de dados (milhares de utentes ou inoculações), a procura linear em listas ligadas torna-se lenta (O(N)).

    O Teste Falhado: O sistema falhou no teste de eficiência temporal. Para obter os 20 valores, seria necessária uma Hash Table para indexar os utentes pelo nome, permitindo o acesso direto O(1) em vez de percorrer a lista inteira.

4. Destaques Técnicos (Revisão de Código)

A. Avanço do Tempo e Validações

O sistema de datas foi implementado com rigor, incluindo a verificação de anos bissextos (regra gregoriana) e a comparação de datas para impedir a aplicação de vacinas fora da validade.

B. Internacionalização (Flag pt)

O programa suporta mensagens de erro em duas línguas, selecionadas no arranque através dos argumentos da main:
C

// Exemplo de tratamento de erro bilíngue em project.c
printf(!strcmp(lingua, "pt") ? "sem memória\n" : "no memory\n");

C. Inserção Ordenada

Em vez de ordenar a lista apenas quando necessário (o que seria O(N log N)), o programa mantém a integridade da ordenação no momento da inserção (inserirOrdenado), garantindo que o comando de listagem (l) seja imediato.

Conclusão

Este projeto demonstra uma maturidade elevada na gestão de recursos de baixo nível em C. A transição de 18.5 para 19.37 no teste prático confirmou que o domínio dos conceitos de algoritmos e estruturas de dados foi plenamente atingido, sendo a falta da Hash Table apenas uma decisão de compromisso entre complexidade e tempo de entrega.
