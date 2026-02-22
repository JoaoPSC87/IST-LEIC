# Projeto Fundamentos da Programação - Jogo das Minas (2022/2023)


Este repositório contém o código do segundo projeto da cadeira de Fundamentos da Programação (FP) do ano letivo 2022/2023. O objetivo do projeto foi recriar o clássico jogo **Minesweeper (Minas)** em Python no terminal, implementando vários Tipos Abstratos de Dados (TADs) e uma lógica de jogo robusta.

**Nota Histórica:** Devido a motivos de força maior na altura da entrega original (2022), o projeto foi submetido com alguns *bugs*. Em 2026, o código foi totalmente revisto e corrigido, passando agora a 100% nos testes públicos e privados!

---

## 📦 Versão Inicial (O que foi entregue em 2022) (Nota: 12,94/20)

A submissão original continha uma excelente base de trabalho, demonstrando uma boa compreensão da estrutura exigida para o jogo e dos conceitos de Tipos Abstratos de Dados.

### ✅ O que estava BEM:
* **Estrutura Base:** A divisão do código nos vários TADs (Gerador, Coordenada, Parcela, Campo) estava bem delineada.
* **Lógica Core:** A interface gráfica (`MinasGUI.py`) e a interação principal do jogo já comunicavam com as funções base.
* **Geração de Aleatoriedade:** O núcleo matemático do algoritmo `xorshift32` e `xorshift64` para gerar minas aleatórias estava na sua maioria bem raciocinado.
* **Criatividade:** Esforço notório para ter o projeto jogável, com a maioria das funções auxiliares de navegação no tabuleiro já criadas.

### ❌ O que estava MAL (Bugs Conhecidos da 1ª Entrega):
* **Violação de Abstração:** A função `campo_para_str` modificava as parcelas destrutivamente (guardava o número de minas vizinhas lá dentro), corrompendo o estado do tabuleiro.
* **Erros de Sintaxe e Tipos:** Falta de validações rigorosas (ex: tentar fazer `ord()` num número inteiro, e falhas no uso de `()` em vez de `[]` em dicionários/listas).
* **Lógica do Jogo:**
  * O jogo não protegia o jogador no primeiro clique (as minas eram colocadas antes da primeira jogada, permitindo perder logo no início).
  * A limpeza em cadeia (`limpa_campo`) esquecia-se de limpar a coordenada inicial, originando *loops* inconsistentes.
  * O ciclo principal terminava após uma única ação em vez de continuar a correr até à vitória ou derrota.
* **Detalhes de Formatação:** Falta de *returns* em alguns `if/else`, espaços em falta nas strings (`[Bandeiras...]` em vez de `   [Bandeiras...]`), e um "O" em falta na mensagem de `BOOOOOOOM!!!`.

---

## 🛠️ Patch Notes (Correções e Melhorias - 2026)

O código foi sujeito a uma sessão rigorosa de *debugging*, resolvendo todos os entraves aos testes automáticos do Mooshak/Pytest.

### TAD Gerador
* **Corrigido:** A lógica de cálculo *xorshift* foi movida de `obtem_estado` (que deve ser puramente funcional) para `atualiza_estado`.
* **Corrigido:** Sintaxe do `eh_gerador` corrigida para aceder corretamente aos elementos da lista (`arg[0]` em vez de `arg(0)`).
* **Adicionado:** Validação rigorosa de tipos (`type(x) == int`) antes de testar valores, impedindo que os testes quebrassem a função com tuplos ou *floats*.

### TAD Coordenada
* **Melhorado:** `str_para_coordenada` limpo para usar a conversão nativa do Python `int()`, ignorando automaticamente zeros à esquerda (e corrigido um `return` em falta).
* **Corrigido:** `obtem_coordenadas_vizinhas` ajustado para devolver exatamente as coordenadas no sentido horário a começar pela diagonal superior-esquerda, como exigido pelo enunciado.

### TAD Parcela e TAD Campo
* **Refatorização:** `eh_parcela_limpa` limpa de verificações desnecessárias de pistas. O TAD Parcela passou a conter apenas o seu estado bruto.
* **Corrigido:** `alterna_bandeira` modificado para devolver corretamente os valores Boleanos `True` (se a ação teve efeito) ou `False` (se falhou), sem devolver o objeto da parcela.
* **Corrigido Crítico:** A cópia profunda do campo (`cria_copia_campo`) foi reescrita para instanciar novas listas na memória (`id()` diferentes), impedindo que alterações numa cópia afetassem o tabuleiro original.
* **Corrigido:** `campo_para_str` deixou de ser destrutivo e passou a calcular as minas vizinhas "on the fly" apenas para impressão.
* **Corrigido:** Condição de vitória no `jogo_ganho` alterada para comparar o número de células seguras limpas com a (Área Total - Minas).

### Funções Principais (`turno_jogador` e `minas`)
* **First-Click Safety:** Adicionada a mecânica oficial do Minesweeper onde a primeira ação do jogo é forçosamente um clique de `[L]impar`, e a função `coloca_minas` só é chamada **depois** de escolhida essa coordenada, garantindo que existem sempre pelo menos 9 casas seguras à volta do primeiro clique.
* **Correção de Boleanos:** O `turno_jogador` passou a devolver `False` em caso de explosão (derrota) e `True` em caso de continuação do jogo (anteriormente estava invertido).
* **Validação à Prova de Bala:** Adicionada proteção contra coordenadas inválidas (ex: letras duplas, números gigantes) *antes* de estas entrarem no motor lógico, impedindo *IndexErrors* e *ValueErrors*.
* **Polimento Cosmético:** Adicionados 3 espaços (`'   '`) antes do contador de bandeiras e retificado o tamanho da *string* de derrota para passar nos testes visuais estritos.

---

