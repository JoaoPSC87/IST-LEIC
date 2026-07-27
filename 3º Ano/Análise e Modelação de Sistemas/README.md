# AMS — AgriRetail Enterprise Modeling

Systems-modeling project for **Análise e Modelação de Sistemas** (Systems Analysis and Modeling) at Instituto Superior Técnico — **2025**, group **62**. A conceptual specification of the **AgriRetail** domain — an alliance of farmers and retailers coordinating agricultural supply and demand — modeled across enterprise architecture, business processes, software, and physical systems. No code: the deliverables are models and analysis.

**Grade: 15/20**

## Team
- João Carvalho - ist 57175
- Rodrigo Santos - ist 107032
- Gabriel Amaral - ist 116424

## The scenario

The **AgriRetail Alliance** runs an "Open Competitive Exploration" to define an open domain-architecture specification. The solution spans a logical information system (**AgriCore**), a physical **Transport Unit** (and its semi-trailers), and the collection/delivery processes connecting farmers, retailers and logistics — deliberately scoped to the domain (payments, security, long-term planning and the internals of external services are treated as black boxes).

## The two phases

**Phase 1 (PRJ1)** — framing and high-level models:
- explicit **assumptions** and **open questions** about the domain;
- a high-level **ArchiMate** layered view of the enterprise architecture;
- the collection/delivery process as a **BPMN** collaboration diagram.

**Phase 2 (PRJ2)** — the full specification (all of Phase 1, refined, plus):
- **AgriCore** — a **UML use-case** diagram and a **domain (class) model**;
- the *"Register order"* use case, both as a **textual description** (all steps and scenarios) and a **UML sequence diagram**;
- a **UML state machine** for the lifecycle of each semi-trailer;
- the **Transport Unit** — a use-case diagram plus **SysML** block and internal-block diagrams of its structure.

Models were judged on technical rigor, coverage of the domain (UoD), the quality of the critical analysis (assumptions and gaps), and clarity of communication; Phase 2's stronger solutions frame the result as a reusable **reference architecture**.

## Contents & tools

Modeled with **Archi** and **Camunda** (Phase 1) and **Sparx Enterprise Architect** (Phase 2). The repository holds the model sources (`.archimate`, `.bpmn`, `.qea`, plus XML exports) and the group reports — the report PDFs embed the rendered diagrams, so they are viewable without any tool.

## Modeling languages

ArchiMate (enterprise architecture) · BPMN (business processes) · UML (use cases, domain model, sequence, state machine) · SysML (block diagrams).
