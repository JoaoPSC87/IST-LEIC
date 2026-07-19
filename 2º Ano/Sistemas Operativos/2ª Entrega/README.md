# IST-KVS — Cliente-servidor com named pipes (Parte 2)

Segunda parte do projeto de **Sistemas Operativos** (IST, 2024/25). Estende o IST-KVS para um **processo servidor** a que processos **clientes** se ligam por *named pipes* (FIFOs), podendo **subscrever** chaves e receber **notificações** sempre que o valor muda.

**Nota:** 9,89 / 20

## O que faz

- O servidor cria um **FIFO de registo** onde os clientes pedem o início de sessão; cada cliente cria três FIFOs próprios (pedidos, respostas, notificações).
- Durante a sessão o cliente pode `SUBSCRIBE`/`UNSUBSCRIBE` chaves; sempre que uma chave subscrita é escrita ou apagada por um `.job`, o cliente é notificado no seu FIFO.
- **Exercício 1** — sessões e subscrições, com suporte a **S sessões concorrentes**.
- **Exercício 2** — `SIGUSR1`: o servidor apaga todas as subscrições e fecha as ligações de todos os clientes (que terminam), **sem se desligar**.

## Arquitetura

- **Tarefa anfitriã** — recebe os pedidos de registo e coloca-os num **buffer produtor-consumidor**.
- **Pool de S tarefas gestoras** — consomem do buffer e servem cada sessão (uma por cliente). O buffer sincroniza-se com **semáforos + mutex**.
- **Notificações** — é a tarefa que escreve na tabela que notifica os subscritores dessa chave (cada chave tem a sua lista de subscritores).
- Protocolo cliente-servidor **binário** (opcode + campos de tamanho fixo).

## Como compilar e executar

```sh
make

# servidor
./src/server/kvs <diretoria_jobs> <max_threads> <max_backups> <fifo_registo>

# cliente (lê SUBSCRIBE/UNSUBSCRIBE/DELAY/DISCONNECT do stdin)
./src/client/client <id_cliente> <fifo_registo> < teste.txt
```

## Reconstrução 2026

A versão entregue estava **despojada** — para garantir os pontos de compilação, tinha-se removido tudo o que rebentava a *build*, pelo que faltavam peças inteiras da arquitetura. Esta foi **reconstruída de raiz** sobre o núcleo (já corrigido) da Parte 1:

- **Protocolo binário** — opcodes como *byte* e campos de tamanho fixo com *padding* `\0` (antes era texto com `atoi`, frágil), com leitura/escrita que tratam operações parciais.
- **Subscrições multi-subscritor** — cada chave passou a ter uma **lista** de subscritores (antes só suportava um), notificando todos no `WRITE`/`DELETE`.
- **Multi-sessão** — a anfitriã + o **buffer produtor-consumidor com semáforos** + o pool de **S gestoras** (a arquitetura que o enunciado pede e que faltava; antes atendia-se um só cliente).
- **`SIGUSR1`** — só a anfitriã escuta o sinal (as outras tarefas bloqueiam-no com `pthread_sigmask`); ao recebê-lo, apaga as subscrições e fecha os pipes de todos os clientes, mantendo o servidor vivo.

## Autores

Grupo - João Carvalho (IST 57175)
      - Frederico Vardasca (IST 78621)
