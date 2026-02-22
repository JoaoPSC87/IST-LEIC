Projeto: Introdução à Arquitetura de Computadores (Assembly RISC-V) - 2023/24

Aluno: João Carvalho (57175)
1. Resumo do Projeto

Implementação do algoritmo de agrupamento k-means em Assembly RISC-V para um processador de 32 bits, com visualização em tempo real numa LED Matrix (32x32). O projeto foca-se na eficiência de execução (ciclos de relógio) e na manipulação direta de memória e I/O.

    Arquitetura: RISC-V (Ripes Simulator)

    Algoritmo: k-means com Distância de Manhattan: 
    d=∣x1​−x2​∣+∣y1​−y2​∣

    Nota do Projeto: 7.5 / 8.0

2. A Otimização "Única" (Feedback do Docente)

O grande diferencial deste trabalho foi a gestão dinâmica da Stack para comparação de estados.
O Problema:

Para saber se o algoritmo deve parar, é necessário comparar os centroides da iteração atual com os da anterior. A maioria dos grupos reserva espaço extra na memória RAM (.data) para isto, o que implica instruções de acesso à memória (lw/sw) lentas.
A Nossa Solução:

    Uso da Stack como Buffer Volátil: Em vez de usarmos a RAM, "empurrámos" os centroides anteriores diretamente para a Stack.

    Vantagem: Reduzimos a pegada de memória do programa e otimizámos o uso da cache (simulada), mantendo os dados mais próximos do processador. Fomos o único grupo a evitar a redundância de vetores estáticos para esta validação.

3. Análise de Performance: O que faltou para o 8.0

Embora a nossa lógica de comparação tenha sido superior, a nota máxima exigia uma otimização matemática que não implementámos:
❌ O "Gargalo" da Instrução div

No cálculo dos novos centroides, utilizámos a instrução de divisão nativa do RISC-V:
Code snippet

div t0, t1, t2  # Média das coordenadas

    Impacto: A instrução div é a mais pesada de qualquer arquitetura, demorando múltiplos ciclos de relógio.

    A Otimização em falta: Substituír a divisão por multiplicação pelo inverso fixo ou por bit-shifts (caso o número de pontos fosse potência de 2), o que tornaria o algoritmo significativamente mais rápido em grandes conjuntos de dados.

❌ Redesenho Seletivo (Dirty Rectangles)

O nosso comando cleanScreen apaga todos os 1024 pixels da matriz em cada iteração. Uma otimização de topo passaria por apagar apenas os pixels que mudaram de cluster, reduzindo drasticamente as operações de escrita em I/O.
4. Destaques Técnicos do Código (proj_14_entrega.s)

    Aritmética de Valor Absoluto: Implementação eficiente da distância de Manhattan sem saltos condicionais complexos, minimizando pipeline stalls.

    Convenção de Chamada: Respeito rigoroso pelos registos s0-s11 e t0-t6, garantindo que sub-rotinas como findNearestCentroid não corrompam o estado do programa principal.

    Tabela de Cores: Mapeamento direto de IDs de clusters para cores RGB em memória, permitindo uma renderização instantânea na LED Matrix.

🚀 Conclusão

Este projeto provou que a eficiência em sistemas embebidos não vem apenas de "escrever menos código", mas de entender como a memória (Stack vs RAM) e o CPU interagem. A nota de 7,5 reflete uma implementação sólida e criativa, com uma maturidade algorítmica acima da média.
