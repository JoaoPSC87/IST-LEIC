# Event Reservation System - Projeto RC 2025/2026

**Grupo:** 88  
**Alunos:**
- Nome: João Carvalho - Número: 57175
- Nome: Rodrigo Santos - Número: 107032

---

## Descrição do Projeto

Sistema de reserva de eventos implementado em C, utilizando sockets UDP e TCP para comunicação cliente-servidor. O sistema permite aos utilizadores criar eventos, fazer reservas, gerir as suas reservas e transferir ficheiros de descrição dos eventos.

---

## Estrutura do Projeto

```
projeto/
├── EUser/              # Aplicação Cliente
│   ├── user.c          # Main do cliente
│   ├── funcs.c         # Funções de comandos
│   ├── protocols.c     # Implementação UDP/TCP
│   └── mensagens.c     # Mensagens de resposta
├── EServer/            # Aplicação Servidor
│   ├── ES.c            # Main do servidor
│   ├── funcs.c         # Lógica de negócio
│   ├── requests.c      # Handlers de requests
│   └── mensagens.c     # Mensagens de protocolo
├── func_aux/           # Funções auxiliares
│   └── validations.c   # Validações de input
├── Makefile            # Compilação
└── README.txt          # Este ficheiro
```

---

## Compilação

Para compilar o projeto:

```bash
make
```

Para limpar ficheiros objeto:

```bash
make clean
```

Executáveis gerados:
- `user` - Aplicação cliente
- `ES` - Aplicação servidor

---

## Execução

### Servidor

```bash
./ES [-p ESport] [-v]
```

Opções:
- `-p ESport`: Porta do servidor (default: 58088)
- `-v`: Modo verbose (mostra mensagens de debug)

Exemplo:
```bash
./ES -v -p 58088
```

### Cliente

```bash
./user [-n ESIP] [-p ESport]
```

Opções:
- `-n ESIP`: IP do servidor (default: 127.0.0.1)
- `-p ESport`: Porta do servidor (default: 58088)

Exemplo:
```bash
./user -n 127.0.0.1 -p 58088
```

---

## Comandos Disponíveis

### Comandos UDP

| Comando | Sintaxe | Descrição |
|---------|---------|-----------|
| login | `login [UID] [password]` | Fazer login ou registar novo utilizador |
| logout | `logout` | Terminar sessão |
| unregister | `unregister` | Apagar conta |
| myevents | `myevents` ou `mye` | Listar eventos criados |
| myreservations | `myreservations` ou `myr` | Listar reservas feitas |

### Comandos TCP

| Comando | Sintaxe | Descrição |
|---------|---------|-----------|
| create | `create [name] [fname] [dd-mm-yyyy hh:mm] [seats]` | Criar evento |
| close | `close [EID]` | Fechar evento |
| list | `list` | Listar todos os eventos |
| show | `show [EID]` | Ver detalhes e descarregar ficheiro |
| reserve | `reserve [EID] [num_seats]` | Fazer reserva |
| changePass | `changePass [old] [new]` | Mudar password |

### Comandos Locais

| Comando | Sintaxe | Descrição |
|---------|---------|-----------|
| exit | `exit` | Sair da aplicação |

---

## Protocolo de Comunicação

### UDP

Usado para operações simples que não requerem transferência de ficheiros:
- Login/Logout/Unregister
- Listar eventos do utilizador
- Listar reservas do utilizador

### TCP

Usado para operações que requerem transferência de ficheiros ou múltiplas trocas:
- Criar eventos (upload de ficheiro)
- Mostrar evento (download de ficheiro)
- Fazer reservas
- Fechar eventos
- Mudar password

---

## Estrutura de Dados no Servidor

```
USERS/
└── [UID]/
    ├── [UID]_pass.txt       # Password do utilizador
    ├── [UID]_login.txt      # Estado de login
    ├── CREATED/             # Eventos criados
    │   └── [EID].txt
    └── RESERVED/            # Reservas feitas
        └── R-[UID]-[timestamp].txt

EVENTS/
└── [EID]/
    ├── START_[EID].txt      # Info do evento
    ├── RES_[EID].txt        # Total de lugares reservados
    ├── END_[EID].txt        # Marca evento fechado (opcional)
    ├── DESCRIPTION/         # Ficheiros de descrição
    │   └── [filename]
    └── RESERVATIONS/        # Reservas individuais
        └── R-[UID]-[timestamp].txt
```

---

## Formato dos Ficheiros

### START_[EID].txt
```
[UID] [name] [fname] [attendance] [date] [time]
```
Exemplo: `157175 Concerto poster.jpg 300 12-12-2026 19:00`

### RES_[EID].txt
```
[num_reserved]
```
Exemplo: `150`

### Ficheiro de Reserva
```
[UID] [num_seats] [dd-mm-yyyy hh:mm:ss]
```
Exemplo: `157175 100 19-12-2025 17:40:09`

---

## Validações Implementadas

- **UID**: 6 dígitos numéricos
- **Password**: 8 caracteres alfanuméricos
- **EID**: 3 dígitos numéricos (001-999)
- **Nome do evento**: Máximo 10 caracteres alfanuméricos
- **Número de lugares**: 10-999
- **Tamanho de ficheiro**: Máximo 10 MB
- **Data**: Formato dd-mm-yyyy hh:mm

---

## Funcionalidades Especiais

1. **Sistema de Timestamps**: Cada reserva tem timestamp único
2. **Gestão de Estado de Eventos**: 
   - 0 = Past (data passou)
   - 1 = Open (ativo)
   - 2 = Sold Out (esgotado)
   - 3 = Closed (fechado pelo criador)
3. **Transferência de Ficheiros**: Suporta upload/download via TCP
4. **Validação de Permissões**: Apenas o criador pode fechar eventos
5. **Controlo de Capacidade**: Rejeita reservas se exceder lugares disponíveis

---

## Tratamento de Erros

O sistema valida e trata:
- Utilizadores não registados
- Passwords incorretas
- Utilizadores não autenticados
- Eventos inexistentes
- Eventos já fechados ou esgotados
- Reservas que excedem capacidade
- Ficheiros inexistentes ou muito grandes
- Formatos de data inválidos
- Inputs malformados

---

## Autores

Este projeto foi desenvolvido no âmbito da cadeira de Redes de Computadores (RC) do 3º ano de LEIC, IST, 2025/2026.
