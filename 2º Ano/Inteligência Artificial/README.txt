Projeto Inteligência Artificial — Nuruomino (2024/2025)

Projeto da cadeira de Inteligência Artificial (IA). O problema é o puzzle Nuruomino
(variante do LITS): dado um tabuleiro dividido em regiões, preencher cada região com
UM tetraminó - uma de quatro formas: L, I, T ou S - respeitando três regras:

  - dois tetraminós do MESMO tipo não podem tocar-se ortogonalmente (mesmo entre
    regiões diferentes);
  - todas as células preenchidas têm de formar UM único bloco ligado;
  - não pode existir nenhum quadrado 2x2 totalmente preenchido.

Está modelado como um problema de PROCURA, sobre o framework aima (search.py), resolvido
com depth_first_graph_search. Lê do standard input e escreve para o standard output.


Nota Histórica: A nota do projeto foi 13,9/20 (componente de código automática: 10/15; o
restante em relatório). No dia da entrega, de manhã, não passava um único teste - só
começou a passar quando a "ação" deixou de ser tratada como um tuplo e passou a ser uma
CLASSE (Action) com hash próprio. A partir daí passou os tabuleiros mais pequenos (10/15),
mas os maiores (ex.: test-05, test-09) nunca terminavam — sem tempo, na altura, para
descobrir porquê.


Versão Submetida em 2024 (Nota do projeto: 13,9/20 — código 10/15)

O que estava BEM:

    Modelação como problema de procura sobre o framework aima (search.py), usando
    depth_first_graph_search (procura em profundidade com controlo de estados repetidos).

    Classe Action hashable — foi o refactor-chave que destrancou tudo: a máscara da peça
    é guardada como tuplo-de-tuplos (hashable, ao contrário de um np.array) e a classe dá
    __hash__/__eq__ próprios. Assim o estado pode ser um frozenset de ações que a procura
    consegue deduplicar. O __iter__ permitiu ainda que o código antigo (que desempacotava a
    ação como um tuplo) continuasse a funcionar sem reescrita.

    Domínios por região pré-calculados: para cada região, todas as colocações de tetraminó
    válidas dentro dela, calculadas uma só vez.

    Heurísticas: MRV (atribuir primeiro a região com menos colocações possíveis) e
    least-constraining (preferir as colocações que afetam menos regiões vizinhas).

    Verificação incremental das regras locais (quadrado 2x2 e peças iguais adjacentes) a
    cada colocação.


O que foi Melhorado (correções 2026)

O Problema:
    A submissão passava 10/15; os tabuleiros maiores nunca terminavam - pareciam "entrar
    em loop" e nunca devolviam solução.

O Diagnóstico (com evidência):
    Instrumentando a procura, descobriu-se que ela CHEGAVA a tabuleiros completos, mas
    rejeitava-os no fim: no test-05, 36 745 tabuleiros completos rejeitados - 100% por
    falta de contiguidade, ZERO por quadrado 2x2. A regra "tudo ligado num só bloco" só
    era testada no goal_test (na folha da árvore), por isso a procura preenchia o
    tabuleiro de dezenas de milhares de maneiras "cheias mas desligadas" antes de
    descobrir que não serviam. Agravado por dois fatores: (a) regiões enormes (20+
    células) com ramificação alta e (b) custo por nó elevado (reconstruía o conjunto de
    células ocupadas e convertia np.array a cada verificação, ~3600 nós/s).

    O "experimento natural" que isolou a causa: test-05 e test-13 são ambos 12x12 com 16
    regiões, mas o test-13 (maior região = 12 células) resolvia num instante e o test-05
    (maior região = 23 células) nunca terminava - a diferença estava no tamanho das
    regiões, não do tabuleiro.

A Correção (três ideias, todas no metodo actions()):
    1. Crescimento ligado por construção - depois de semear a partir da região mais
       constrangida, só se consideram colocações que TOCAM no bloco já montado (de todas
       as regiões adjacentes, não de uma só, para não perder completude). Assim os becos
       "cheios mas desligados" nunca chegam a ser explorados.

    2. Propagação de unitárias - uma região com uma única colocação válida é forçada de
       imediato, sem ramificar. Colapsa os tabuleiros muito repartidos (ex.: regiões de
       tamanho 4, que só admitem uma peça).

    3. Velocidade - as células de cada peça passam a ser pré-calculadas na Action e a
       validação é feita com conjuntos já prontos, sem np.array nos ciclos quentes (~10x
       mais rápido por nó).

O Resultado:
    Os 13 testes públicos passam (test_script.py), o mais lento em ~3,3s (limite de 6s por
    teste). Aproveitou-se para limpar métodos que deixaram de ser usados.


Como executar:

    Um teste:    python nuruomino.py < tests/test-01.txt
    Suite toda:  python test_script.py


Ficheiros:

    nuruomino.py           A nossa solução (o ficheiro alterado).
    search.py, utils.py    Framework de procura fornecido com o projeto (AIMA) - não é nosso.
    test_script.py, tests/ Script e testes públicos fornecidos com o projeto.


Autores (Grupo 53):
    João Pedro dos Santos Carvalho (IST 57175)
    Rodrigo Alexandre Almeida Fonseca dos Santos (IST 107032)