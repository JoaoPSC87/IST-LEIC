# HVA — Gestão de Hotel/Habitat de Animais

Projeto da cadeira de **Programação com Objetos** (IST, 2024/2025). Aplicação em **Java** para a gestão de um jardim zoológico, com uma arquitetura em duas camadas claramente separadas (domínio e interface).

## O domínio

O HVA gere:

- **Habitats** — com uma área, uma população de animais e um conjunto de árvores.
- **Árvores** de dois tipos, **caducas** (perdem a folha) e **perenes**, cujo **ciclo biológico** e **esforço de limpeza** variam com a **estação** do ano (Primavera, Verão, Outono, Inverno) e com a idade.
- **Animais** de várias **espécies**, com uma satisfação que depende do habitat, da população da própria espécie e das restantes, e da influência do habitat sobre a espécie.
- **Funcionários** — **tratadores** (responsáveis por habitats) e **veterinários** (responsáveis por espécies), cada um com a sua fórmula de satisfação.
- **Vacinas** e o registo de **vacinações**, incluindo a deteção de vacinações erradas (com diferentes níveis de dano) e o seu efeito na saúde do animal.
- **Avanço das estações**, que envelhece as árvores e altera o seu ciclo biológico.
- **Persistência** do estado (guardar/abrir ficheiros serializados) e **importação** de dados a partir de ficheiros de texto.

## Arquitetura

- **`hva-core`** — o domínio: as classes de negócio (`Hotel`, `Habitat`, `Animal`, `Tree`, `Species`, `Employee`, `Vaccine`, as estações e os cálculos de satisfação). Não conhece a interface.
- **`hva-app`** — a interface (menus e comandos), construída sobre a framework **`po-uilib`**. Comunica com o domínio **exclusivamente através da fachada `Hotel`**.

## Como compilar e executar

Requisitos: **Java 21** e a framework **`po-uilib`**.

```sh
# 1. Compilar a framework po-uilib (a partir do seu source)
cd po-uilib && make

# 2. Compilar o projeto (a partir da raiz), apontando para o po-uilib
make PO_UILIB_DIR=<caminho/para/po-uilib>
```

Executar a aplicação:

```sh
java -cp "<po-uilib.jar>:hva-core/hva-core.jar:hva-app/hva-app.jar" hva.app.App
```

Para arrancar com dados pré-carregados, indica um ficheiro de importação com `-Dimport=<ficheiro>`.

> Nota: no Windows o separador do classpath é `;` (em vez de `:`).

## Correções 2026

Depois da entrega, o professor disponibilizou a **pauta descritiva por grupo** e os testes de avaliação. Aproveitei para corrigir os pontos apontados na pauta e vários bugs que custaram avaliação, deixando o projeto **acima da versão entregue**.

### Ciclo biológico das árvores (o maior corte da pauta)

A satisfação dos tratadores depende do **esforço de limpeza** de cada árvore, definido como o **produto** de três fatores: `dificuldade_base × esforço_sazonal × ln(idade + 1)`. O código **somava** o termo `ln(idade + 1)` em vez de o multiplicar, produzindo satisfações erradas em todas as combinações de estação e tipo de árvore. Corrigido para o produto.

### Privacidade do core (encapsulamento)

Os *getters* de coleções já devolviam vistas imutáveis, mas os **métodos mutadores** das classes de domínio (adicionar, mudar, incrementar…) estavam declarados `public`. Isso permitiria, em teoria, à camada de interface obter um objeto do domínio e modificá-lo diretamente, contornando a fachada. Passaram a **package-private**, de forma a que seja o **próprio compilador** a impor a separação app/core — a interface só pode operar através do `Hotel`.

### Outros bugs corrigidos

- **Argumentos trocados numa exceção** — a mensagem de "veterinário não autorizado a vacinar" trocava o identificador do veterinário com o da espécie.
- **Divisões inteiras** — dois cálculos de satisfação (do animal e do veterinário) usavam divisão inteira, truncando a parte decimal e arredondando incorretamente.
- **Sensibilidade a maiúsculas/minúsculas** — os identificadores de espécies e de árvores não eram comparados de forma *case-insensitive* (ao contrário dos restantes), o que permitia duplicados.
- **Limite do dano de vacinação** — o limiar entre os efeitos "acidente" e "erro" tinha um *off-by-one*.
- **Histórico de saúde** — os registos de vacinação de um animal eram separados por `|` em vez de `,`.
- **Persistência** — abrir um ficheiro não registava o nome do ficheiro atual, pelo que uma gravação seguinte voltava a pedir o nome.

## Autores

Grupo 35:
- João Carvalho (IST 57175)
- Rodrigo Santos (IST 107032)
