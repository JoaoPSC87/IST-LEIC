# k-means em RISC-V Assembly

Projeto de **Introdução à Arquitetura de Computadores** (IST, 2023/2024). Implementação do algoritmo de agrupamento **k-means** em Assembly **RISC-V** (simulador Ripes), com visualização em tempo real numa **LED Matrix 32×32**. O foco está na eficiência de execução (ciclos de relógio) e na manipulação direta de memória e I/O.

- **Distância:** Manhattan — `d = |x1 − x2| + |y1 − y2|`
- **Nota do projeto:** 7,5 / 8,0

## Otimização original: comparação de estados na Stack

Para decidir quando parar, o algoritmo compara os centroides da iteração atual com os da anterior. A maioria dos grupos reserva espaço extra em `.data` (RAM) para guardar os centroides anteriores, o que implica acessos `lw`/`sw` à memória.

A nossa abordagem foi **empurrar os centroides anteriores para a Stack** em vez da RAM — menos memória estática e dados mais próximos do processador.

## Otimização 2026: passagem única no cálculo dos centroides

Ao rever o projeto, identifiquei o **verdadeiro gargalo** de desempenho.

**O problema:** a rotina `calculateCentroids` recalculava, para *cada* cluster, o cluster mais próximo de *todos* os pontos:

```
para cada cluster c (k vezes):
    para cada ponto p (n vezes):
        nearestCluster(p)      # recalculado k vezes por ponto!
        se p pertence a c, somar as coordenadas
```

São `k × n_points` chamadas a `nearestCluster` por iteração — e cada chamada é, ela própria, O(k). Complexidade **O(n·k²)** por iteração.

**A correção:** uma **passagem única** sobre os pontos — calcula-se o cluster de cada ponto **uma só vez** e acumula-se diretamente no cluster respetivo, dando **O(n·k)** (um ganho de ~k×). Fiel à filosofia original, os acumuladores (`soma_x`, `soma_y` e `contagem` por cluster) ficam na **Stack**, sem nada estático. Medido no Ripes com a mesma inicialização, a versão otimizada gasta consistentemente **menos ciclos** e chega aos mesmos centroides.

### Otimizações consideradas e descartadas

- **Substituir o `div`** (cálculo das médias): pouco compensa. O divisor é o *número de pontos do cluster* — um valor de **runtime**, não constante — pelo que não dá para *bit-shifts* nem multiplicação por inverso fixo; e são apenas ~6 divisões por iteração (negligenciável).
- **Redesenho seletivo (dirty rectangles):** apagar só os pixels que mudam de cluster em vez dos 1024. Ganho real, mas secundário, e exigiria guardar o cluster de cada ponto entre iterações.

## Destaques técnicos

- **Valor absoluto sem saltos complexos** no cálculo da distância de Manhattan.
- **Convenção de chamada** rigorosa (registos `s`/`t`), garantindo que sub-rotinas como `nearestCluster` não corrompem o estado do programa.
- **Tabela de cores** que mapeia IDs de cluster para RGB, para renderização direta na LED Matrix.

## Como executar

Abrir no simulador **Ripes** com a seguinte configuração:

- **Processador:** de **32 bits** e, dentro desses, o mais simples — o **Single-cycle processor**.
- **LED Matrix** de **32×32** (periférico de I/O).

O input é escolhido na secção `.data` (descomentar um dos `Input A–E`). Depois é só carregar o programa (`.s`) e executar.

## Autores (Grupo 14)

- João Carvalho (IST 57175)
- Miguel Blanco (IST 106893)
- Rodrigo Santos (IST 107032)
