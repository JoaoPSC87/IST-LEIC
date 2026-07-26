# IPM — HCI Bake-offs

Two design "bake-offs" for **Interação Pessoa-Máquina** (Human-Computer Interaction) at Instituto Superior Técnico — **2025/26**, group **L03G57**. Each is an iterative design challenge — ideate, prototype, and test with real users — evaluated in a blind cross-class competition.

## Team
- João Carvalho — ist 57175
- Caio Aguiar - ist 110794
- João Vaz - ist 113809
- Paulo Brito - ist 114642

## Bake-off 1 — Recipe-video social network (Figma). Grade - 13.7/20

A high-fidelity **Figma** prototype (`L03G57.fig`) for a mobile app to share and discover recipe videos, reimagining a video social network around cooking. The prototype supports three user tasks:
- **defining the user's kitchen utensils** (e.g. adding an electric mixer, a wok, a thermometer);
- **searching recipe videos** with rich filters (cuisine, required utensils, ingredients, dietary restrictions, verified creators) and subscribing to a creator;
- **seasonal-ingredient inspiration** — from a push notification, to a filtered recipe, to finding the nearest market with the ingredient in stock.

Built following the course's design cycle — a paper low-fidelity prototype iterated into the Figma high-fidelity prototype, informed by formative evaluations (Wizard-of-Oz, think-aloud) and designed for the iPhone 16 frame (393×852). Tested blind by students from other classes and scored with the **UEQ-S** user-experience questionnaire.

## Bake-off 2 — Selection in dense UIs (p5.js) · Grade - 20/20

A functional **p5.js** prototype that minimizes the time to select a target in a dense **8×10 grid of 80 targets** (city names). Starting from a provided sketch — which labels the grid randomly, prompts a target, and logs performance metrics to Firebase — the interface was redesigned so users hit the requested target as fast as possible, applying **Fitts's law**, UX laws and human-factors principles, without ever reading the target variable.

The design was iterated over two rounds with **A/B testing** on ≥10 users each, and the gains were backed by **quantitative analysis** — mean selection times with confidence intervals, t-tests / ANOVA, and bar charts. Judged on both the design justification and the actual mean selection time in the blind bake-off, it achieved **top marks**. The group's work is in `sketch.js`, `target.js`, `support.js` and `style.css`.

## Process & stack

Iterative HCI design (ideate → prototype → evaluate) · low- and high-fidelity prototyping · formative & summative user testing · UEQ-S, A/B tests, inferential statistics. **Figma** (Bake-off 1) · **p5.js** + Firebase (Bake-off 2).
