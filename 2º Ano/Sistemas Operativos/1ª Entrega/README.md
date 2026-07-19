# IST-KVS — Key-Value Store concorrente (Parte 1)

Projeto de **Sistemas Operativos** (IST, 2024/25). O IST-KVS armazena pares chave-valor numa tabela de dispersão, processa ficheiros de comandos em lote e explora **concorrência** (múltiplas tarefas) e **processos** (cópias de segurança não bloqueantes).

**Nota:** 14,17 / 20

## O que faz

Lê ficheiros `.job` de uma diretoria e, para cada um, produz um `.out` com o resultado dos comandos `WRITE`, `READ`, `DELETE`, `SHOW`, `WAIT` e `BACKUP`. Os pares ficam numa hashtable (colisões por listas ligadas). O acesso a ficheiros usa a interface POSIX (descritores), não a `stdio`.

- **Exercício 1** — processamento de ficheiros de comandos em lote.
- **Exercício 2** — `BACKUP` não bloqueante: um `fork` grava um snapshot enquanto o pai continua; no máximo N backups em simultâneo.
- **Exercício 3** — paralelização: cada `.job` é processado por uma tarefa, com sincronização no acesso à tabela.

## Arquitetura

- `main` — descobre os ficheiros e distribui-os por um **pool de tarefas**.
- `operations` — a camada de serviço (init/write/read/delete/show/backup) com a sincronização.
- `kvs` — a tabela de dispersão. · `parser` — o analisador de comandos.

## Como compilar e executar

```sh
make
./kvs <diretoria_jobs> <max_backups> <max_threads>
```

## Correções 2026

A versão entregue compilava e corria, mas a concorrência (Exercício 3) tinha problemas graves que só não apareciam com uma tarefa. Revisão:

- **Sincronização real da tabela** — o `rwlock` era passado *por valor*, ou seja, cada operação trancava uma **cópia** do lock: a tabela não tinha sincronização nenhuma. Passou a lock por endereço e, depois, a **um lock por *bucket*** (aquisição por ordem de índice → sem interblocagem), maximizando o paralelismo.
- **Estado por tarefa** — o descritor e o *buffer* de saída eram **globais** e partilhados por todas as tarefas (outputs misturados / escritos no ficheiro errado). Passaram a locais.
- **Pool de tarefas** — a versão original degenerava para execução **serial** (reutilizava sempre o mesmo *slot*). Passou a uma **fila de trabalho** com N tarefas a consumir até esvaziar.
- **Semântica do `.out`** — erros para `stderr` (o `.out` só leva resultados), e `READ`/`SHOW` passaram a sair **ordenados alfabeticamente**.
- **Cópia de segurança** — o filho termina com `_exit` e a espera por vaga saiu de dentro do *lock*.
- **Limpeza** — removido código morto (um módulo só com código comentado, declarações órfãs).

## Autores

Grupo - João Carvalho (IST 57175)
      - Frederico Vardasca (IST 78621) 
