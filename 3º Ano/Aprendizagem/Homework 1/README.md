# Homework 1 — Árvores de Decisão e Ganho de Informação

Trabalho da cadeira de **Aprendizagem** (Machine Learning, IST, 2025/26). Tema: aprendizagem de **árvores de decisão** por **ganho de informação** (entropia de Shannon) e avaliação de modelos.

**Nota:** 16,43 / 20 — pen-and-paper 8,23/11 · programming 8,2/9

## O problema

**Parte I — Pen & paper:** completar uma árvore de decisão por ganho de informação, desenhar a matriz de confusão de treino, identificar a classe de menor F1, traçar histogramas condicionais e detetar *outliers* pelo critério do IQR.

**Parte II — Programação:** sobre o `hungarian_heart_diseases.csv` (284 pacientes, 9 atributos, *normal* vs *doença cardíaca*): varrimento de `min_samples_leaf` com análise de *over/underfitting*, e procura de um modelo ótimo (split estratificado 60/20/20, *grid search* em `max_depth` e `min_samples_split`) com extração das **associações condicionais** e respetivas **probabilidades posteriores**.

## Como executar

```sh
pip install scikit-learn pandas matplotlib
jupyter notebook G48_notebook.ipynb
```

O `hungarian_heart_diseases.csv` deve estar na mesma pasta.

## Correções 2026

- **Reprodutibilidade** — a árvore do varrimento passou a fixar `random_state=1` (o *split* já era semeado, a árvore não).
- **Regras + posteriores** — acrescentado um bloco que percorre a árvore e imprime cada caminho (regra) com a sua probabilidade posterior, respondendo por inteiro à Q3.ii.
- **Leitura da árvore** — corrigida a interpretação de dois *splits* que estavam descritos ao contrário (`chest_pain` e `sex`), alinhando o texto com os dados.

## Autores

Grupo 48 — João Carvalho (IST 57175)
         — Rodrigo Santos (IST 107032)