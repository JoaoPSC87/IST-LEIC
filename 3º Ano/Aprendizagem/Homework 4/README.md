# Homework 4 — Clustering (k-means), PCA e LDA

Trabalho da cadeira de **Aprendizagem** (Machine Learning, IST, 2025/26). Tema: aprendizagem **não-supervisionada** (*k-means*, PCA) e **redução de dimensionalidade supervisionada** (LDA).

**Nota:** 19 / 20 — pen-and-paper 8,5/9 · programming 10,5/11

## O problema

**Parte I — Pen & paper:** executar uma época de *k-means* com distância euclidiana e discutir o efeito da inicialização dos centroides; calcular a matriz de covariância de três pontos 3D, determinar o plano de projeção que minimiza o erro e verificar se separa as classes.

**Parte II — Programação:** sobre o `diabetes.csv` (normalizado com *Min-Max*): *k-means* com curva do SSE (*elbow*), usar os *clusters* como classificador pela regra da maioria (matriz de confusão, *precision*, *recall*, F1) e discutir as suas limitações; PCA com variância explicada acumulada; e comparar a separação das classes ao longo de PC1 (PCA) vs a componente LDA.

## Como executar

```sh
pip install scikit-learn pandas matplotlib seaborn
jupyter notebook G48.ipynb
```

O `diabetes.csv` deve estar na mesma pasta.

## Correções 2026

- **Comentário da LDA (B-3)** — o comentário passou a **quantificar** a separação (medianas por classe, distância entre médias em desvios-padrão, *accuracy* de ~76,8% ao usar a componente como regra) e a explicitar a **limitação**: mesmo o melhor eixo linear deixa ~23% de erro, ou seja, as classes não são linearmente separáveis com estas *features*.

## Autores

Grupo 48 — João Carvalho (IST 57175)
         — Rodrigo Santos (IST 107032)