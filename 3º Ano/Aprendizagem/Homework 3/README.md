# Homework 3 — Regressão Linear/Ridge e Perceptrão Multicamada

Trabalho da cadeira de **Aprendizagem** (Machine Learning, IST, 2025/26). Tema: **regressão** (solução fechada dos mínimos quadrados e regularização Ridge) e **redes neuronais** (MLP e retropropagação).

**Nota:** 20 / 20 — pen-and-paper 12/12 · programming 8/8

## O problema

**Parte I — Pen & paper:** aprender uma regressão por OLS num espaço transformado por função de base polinomial; repetir com Ridge (λ=1) e discutir o efeito da regularização; comparar MAE de treino e teste; e executar um passo de gradiente descendente estocástico num MLP com *softmax* e entropia cruzada.

**Parte II — Programação:** sobre o `rent.csv`: comparar Regressão Linear com MLP (sem ativação e com *ReLU*) por *cross-validation*, com *boxplots* do MAE; explicar o papel das funções de ativação; e traçar as curvas de aprendizagem (treino vs validação) para diagnosticar *over/underfitting*.

## Como executar

```sh
pip install scikit-learn pandas matplotlib seaborn
jupyter notebook HW3_G48_notebook.ipynb
```

O `rent.csv` deve estar na mesma pasta.

## Notas

Trabalho com **cotação máxima** em ambas as partes; incluído tal como entregue, sem alterações.

## Autores

Grupo 48 — João Carvalho (IST 57175)
         — Rodrigo Santos (IST 107032)