# Homework 2 — Aprendizagem Bayesiana e k-Vizinhos (Naïve Bayes vs kNN)

Trabalho da cadeira de **Aprendizagem** (Machine Learning, IST, 2025/26). Tema: **classificadores Bayesianos** e **k-vizinhos-mais-próximos**, e a sua comparação.

**Nota:** 19,25 / 20 — pen-and-paper 12,5/13 · programming 6,75/7

## O problema

**Parte I — Pen & paper:** aprender um classificador Bayesiano (com variável normalmente distribuída) e classificar sob MAP; calcular a *accuracy* de um kNN com distância de Hamming por *leave-one-out*; e provar o limite estrito do erro do 1-NN face ao erro de Bayes (`E₁ₙₙ = 2·E_Bayes·(1 − E_Bayes)`).

**Parte II — Programação:** sobre o *Wisconsin Diagnostic Breast Cancer*: comparar kNN (k=5) e Gaussian Naïve Bayes por *cross-validation* estratificada, avaliar o impacto do *Min-Max scaling*, testar com *scipy* se o kNN é estatisticamente superior, e variar o número de vizinhos com pesos *uniform* e *distance*.

## Como executar

```sh
pip install scikit-learn scipy pandas matplotlib
jupyter notebook HW2_notebook_G48.ipynb
```

Requer o `Breast_cancer_dataset.csv` (Wisconsin Diagnostic Breast Cancer) na mesma pasta.

## Correções 2026

- **Teste estatístico (1c)** — o teste-t pareado passou a ser **unilateral** (`alternative='greater'`), por a questão ser direcional (*superior*, não apenas *diferente*); as hipóteses e a justificação foram alinhadas em conformidade. A conclusão mantém-se (kNN superior), mas o método passa a corresponder à pergunta.

## Autores

Grupo 48 — João Carvalho (IST 57175)
         — Rodrigo Santos (IST 107032)