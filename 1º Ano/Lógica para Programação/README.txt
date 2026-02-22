Projeto Lógica para Programação - Horários e Ocupação de Mesa (2022/2023)

Este repositório contém o código do projeto da cadeira de Lógica para Programação (LP) do ano letivo 2022/2023. O objetivo foi desenvolver um sistema de gestão de horários do IST-Tagus e um motor de resolução de restrições para um problema de etiqueta de mesa.

Nota Histórica: A submissão original obteve 18,7 valores. Embora a funcionalidade estivesse correta, a nota máxima não foi atingida devido a penalizações na componente de Comentários e por não se ter utilizado o potencial de Simplificação do Prolog (especialmente nas negações).
📦 Versão Submetida em 2022 (Nota: 18,7/20)

A versão original foi muito forte na parte funcional, resolvendo com sucesso os problemas de pesquisa de horários e a complexidade da ocupação da mesa.
✅ O que estava BEM:

    Funcionalidade Total: O programa passava nos testes e encontrava as soluções corretas para a mesa, provando que o motor lógico estava bem montado.

    Módulo de Horários: Uso eficaz de predicados como findall/3 e sort/2 para organizar os dados do IST-Tagus.

    Dinâmica de Restrições: Implementação robusta do chamaRestricoes usando o operador univ (=..).

🛠️ O que foi Melhorado (correções 2026)

Esta revisão focou-se precisamente nos pontos onde a nota foi penalizada na entrega original:
1. Simplificação Radical das Negações (naoLado e naoFrente)

    O Erro Original: Na entrega de 2022, estas restrições foram feitas de forma "manual" e extensiva, com centenas de linhas a tentar prever onde as pessoas não podiam estar.

    A Correção: Aplicação do conceito de Negação por Falha. Agora, o código apenas define o que é estar em frente ou ao lado, e usa o operador \+ para negar tudo o resto. Isto reduziu o ficheiro em centenas de linhas e tornou-o muito mais elegante.


2. Geometria e Simetria

    Ajuste Técnico: Refinação do predicado lado/3 para garantir que a simetria (A ao lado de B = B ao lado de A) está embutida na regra base, eliminando falhas em interrogações que invertiam a ordem dos nomes.
---

---

