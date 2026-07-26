# Aviação — Airline Flight Database

Database project for **Bases de Dados** (Databases) at Instituto Superior Técnico — **Group 73**. A relational database for an airline — airports, planes, seats, flights, sales and tickets — with trigger-based integrity constraints, a REST API, and OLAP analytics over a materialized view.

**Grades: E1 (conceptual model) 16.3/20 · E2 (implementation) 17.3/20.**

## Team
- João Carvalho — ist157175
- Miguel Blanco — ist1106893
- Rodrigo Santos — ist1107032

## The two deliveries

**E1 — Conceptual design** (`E1/`). Entity-Association model of the airline domain and its mapping to a relational schema.

**E2 — Implementation & analysis** (`E2/`):
- **Schema** (`aviacao.sql`) — 6 tables: `aeroporto`, `aviao`, `assento`, `voo`, `venda`, `bilhete`.
- **Integrity constraints** via triggers:
  - **RI-1** — at check-in, the ticket's class must match the seat's class and the seat's plane must be the flight's plane;
  - **RI-2** — tickets sold per class per flight cannot exceed the plane's seats of that class;
  - **RI-3** — a sale's time must precede the departure of every flight it buys tickets for.
- **Population** (`populate.sql`, loaded via `\copy` from text files) produced by a Python generator.
- **REST API** (`app/app.py`, Flask + psycopg) — list airports; upcoming departures from an airport (next 12h); next three flights between two airports with tickets available; buy tickets; check in.
- **Materialized view** `estatisticas_voos` — per-flight statistics (passengers, seats and revenue per class, plus date parts).
- **OLAP analytics** (`E2-report.ipynb`) — most in-demand routes, fleet coverage, revenue cubes with `GROUPING SETS`, and weekday class-ratio patterns.
- **Index analysis** with `EXPLAIN (ANALYZE, BUFFERS)`.

## Running it

Runs on the course's [BDist workspace](https://github.com/bdist/bdist-workspace) (PostgreSQL 17 + Jupyter in Docker). In a Jupyter terminal:

```sql
psql -h postgres -U postgres          -- password: postgres
CREATE USER aviacao WITH PASSWORD 'aviacao';
CREATE DATABASE aviacao WITH OWNER = aviacao ENCODING = 'UTF8';
GRANT ALL ON DATABASE aviacao TO aviacao;
\q
```
```sql
psql -h postgres -U aviacao aviacao   -- password: aviacao
\i aviacao.sql
\i populate.sql
```
Then open `E2-report.ipynb` in Jupyter (it connects to `aviacao@postgres/aviacao`) and run all cells. To regenerate the data, run `python corrected_script.py`, which writes the `.txt` files that `populate.sql` loads.

## 2026 corrections

- **Data generator rewritten** (`corrected_script.py`). The original had several defects: it used only 5 of the 10 planes, compressed every flight into a three-month window, never generated tickets in its main routine, and ran in O(n³). It was rewritten to schedule flights day-by-day across the whole period using all planes (each departs from its previous arrival), generate ~70k tickets over ~12k sales with a `bisect`-based O(n log n) cut, and satisfy every integrity constraint (zero violations). It also emits the `.txt` files consumed by `populate.sql`.
- **Honest index analysis (section 6).** The submission claimed a "−22%" gain from indexes on the base tables. Re-measuring with `EXPLAIN (ANALYZE, BUFFERS)` showed those indexes are **not used** when building the materialized view (a full aggregation of ~71k tickets — a sequential scan is optimal) and are **redundant** for selective lookups (`voo_id` is already indexed by the `UNIQUE(voo_id, codigo_reserva, nome_passegeiro)` constraint). Section 6 was rewritten around the real findings: the **materialized view** is the optimization for the analytical workload, while a genuinely useful index targets a selective, uncovered column — `bilhete(codigo_reserva)`, which turns a sequential scan (3.84 ms) into an index scan (0.07 ms, ~55×) for booking lookups.

## Stack

PostgreSQL 17 · Python (Jupyter, ipython-sql) · Flask + psycopg · Docker (BDist workspace).
