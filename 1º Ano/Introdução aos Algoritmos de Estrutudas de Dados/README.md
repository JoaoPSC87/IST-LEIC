# IAED — Sistema de Gestão de Vacinas e Inoculações

Projeto de **Introdução aos Algoritmos e Estruturas de Dados** (IST, 2024/25), em **C**.
Sistema de gestão de lotes de vacinas e do registo das suas aplicações a utentes:
controlo de stocks, validades, aplicações cronológicas e remoções seletivas.

**Aluno:** João Pedro Carvalho (ist157175)
**Nota:** projeto 18,5 / 20 · final após teste prático **19,37 / 20**

## Funcionalidades

Interação por comandos lidos do *stdin*:

| Comando | Ação |
|---|---|
| `c` | introduz um novo lote de uma vacina |
| `l` | lista os lotes disponíveis (todos ou por nome) |
| `a` | aplica uma dose a um utente |
| `r` | retira a disponibilidade de um lote |
| `d` | apaga registos de aplicações de um utente |
| `u` | lista aplicações (todas ou de um utente) |
| `t` | avança o tempo simulado |
| `q` | termina |

Suporta mensagens de erro em **português** (`./proj pt`) ou inglês (por omissão).

## Compilar e executar

```sh
gcc -O3 -Wall -Wextra -Werror -Wno-unused-result -o proj *.c
./proj < teste.in > teste.out
```

## Arquitetura

Projeto **modular**, dividido por responsabilidades em vez de um ficheiro único:

| Ficheiro | Responsabilidade |
|---|---|
| `project.c` | ciclo principal e despacho de comandos |
| `comandos.c` | implementação de cada comando |
| `funcs_verificacao.c` | validações e regras de negócio |
| `funcs_insert.c` · `funcs_delete.c` · `funcs_lista.c` | inserção, remoção e listagem |
| `hash.c` | tabela de dispersão que indexa registos por utente |
| `datas.c` · `func_aux.c` | datas (anos bissextos) e leitura de nomes |

Destaques da implementação original: **gestão de memória sem fugas** (validado com
Valgrind), *parsing* robusto de nomes entre aspas com expansão dinâmica do *buffer*,
e inserção ordenada dos lotes (mantendo a listagem imediata).

## Otimização 2026 — de O(N²) para O(N)

A submissão original passou **todos** os testes de correção, mas falhou **3 testes por
*time limit exceeded*** (0,5 valores cada). O diagnóstico revelou **dois** percursos
lineares no comando mais frequente (`a` — aplicar vacina), que juntos tornavam o
programa **quadrático**:

**1. Inserção do registo.** As inoculações são sempre aplicadas na data atual, e essa
data nunca recua — o novo registo pertence *sempre* ao fim da lista. Mesmo assim, o
código percorria a lista inteira para lá chegar. Passou a manter-se um apontador para
a **cauda**, tornando a inserção **O(1)**.

**2. Verificação de "já vacinado".** A cada aplicação percorria-se a lista global à
procura de registos daquele utente. Introduziu-se uma **tabela de dispersão** (djb2,
encadeamento externo) que associa a cada utente a sua própria lista de registos,
em ordem cronológica. A verificação — e também a listagem (`u <utente>`) e a
validação de existência (`d`) — passou a consultar **apenas os registos desse utente**.

Para respeitar as restrições do enunciado (sem variáveis globais e sem `extern`), o
estado foi encapsulado numa abstração `Registo`, passada por parâmetro, que agrega a
lista cronológica, a sua cauda e a tabela de dispersão.

### Resultados medidos

| Aplicações (N) | Original | Cauda O(1) | **+ Hash** |
|---|---|---|---|
| 10 000 | 0,31s | 0,11s | **0,01s** |
| 40 000 | 4,75s | 2,45s | **0,02s** |
| 100 000 | **33,37s** | — | **0,08s** |
| 400 000 | — | — | **0,42s** |

A **~420× mais rápido** com 100 000 aplicações, e a curva passou a **linear**
(duplicar N duplica o tempo, em vez de o quadruplicar).

Todas as alterações foram validadas por **regressão**: o output do programa mantém-se
**byte a byte idêntico** ao da versão submetida (em ambas as línguas), numa bateria que
cobre todos os comandos, erros e casos-limite.

## Lição

A escolha da estrutura de dados deve seguir as operações que **dominam** a execução.
A lista ligada tornava a listagem trivial, mas penalizava as operações mais frequentes.
E o gargalo mais caro nem sempre é o mais óbvio — a inserção "ordenada" custava tanto
como a procura, e passava despercebida.
