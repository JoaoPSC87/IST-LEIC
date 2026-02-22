# Projeto 1: Fundamentos da Programação (2022/2023)

Este repositório contém a minha resolução do primeiro projeto da cadeira de Fundamentos da Programação (FP) do ano letivo de 2022/2023. Este projeto foi dividido em três tarefas independentes que testaram diferentes paradigmas da programação em Python: manipulação de *strings*, algoritmos de alocação e cálculo numérico iterativo.

> **Avaliação Final:** 18,44 / 20 
> **Testes Automáticos (Mooshak):** 100% de aprovação (Públicos e Privados).

---

## ⚙️ O que foi implementado (As 3 Tarefas)

### Tarefa 1: Justificação de Textos
Um formatador de texto automático que, dada uma *string* e uma largura de coluna, divide o texto em várias linhas e distribui espaços em branco de forma uniforme entre as palavras, simulando o comportamento de justificação de texto de processadores como o Microsoft Word.
* **Funções-chave:** `limpa_texto`, `corta_texto`, `insere_espacos`, `justifica_texto`.

### Tarefa 2: Método de D'Hondt

Implementação do algoritmo matemático utilizado em Portugal (e noutros países) para alocar mandatos (deputados) em eleições proporcionais. O programa calcula os quocientes sucessivos e lida com as regras de desempate (prioridade aos partidos com menos votos globais).
* **Funções-chave:** `calcula_quocientes`, `atribui_mandatos`, `obtem_candidatos_aprovados`.

### Tarefa 3: Sistemas de Equações (Método Iterativo)
Um solucionador de sistemas de equações lineares utilizando métodos iterativos. O código verifica se a matriz do sistema é de diagonal estritamente dominante (condição de convergência), reordena as linhas se necessário para remover zeros da diagonal, e calcula o vetor solução iterativamente até atingir a precisão desejada.
* **Funções-chave:** `retira_zeros_diagonal`, `eh_diagonal_dominante`, `resolve_sistema`.

---

## 📊 Autoavaliação e Lições Aprendidas

### ✅ O que correu muito bem (A base da nota)
* **Lógica à Prova de Bala:** O facto de ter passado a 100% nos testes do Mooshak (que testa dezenas de *edge-cases* ocultos) demonstra que as validações de input (`raise ValueError`) e os algoritmos principais foram implementados de forma robusta e matematicamente correta.
* **Cálculo Numérico em Python:** O sucesso na Tarefa 3 mostra um bom domínio da manipulação de tuplos aninhados (matrizes) e da gestão de erros de arredondamento em *floats*.

### 📈 Onde perdi pontos (O que faltou para o 20)
Embora a máquina tenha dado nota máxima à lógica, a avaliação manual do docente apontou algumas falhas nas boas práticas de código (*Clean Code*):
* **Comentários Insuficientes:** Faltaram explicações em linguagem natural nas lógicas mais complexas (como os *loops* do D'Hondt ou as iterações do sistema de equações), o que dificulta a leitura do código por terceiros.
* **Estilo e *Docstrings*:** Algumas *docstrings* (as explicações no início de cada função) poderiam ter sido mais descritivas quanto aos tipos de dados recebidos e devolvidos, e faltou aplicar algumas convenções normativas de PEP-8 (espaçamentos, quebras de linha).

---

---

