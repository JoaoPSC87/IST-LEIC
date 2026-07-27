# HumanaEthica — Shifts (ES 2025/26 · Group AL-32)

Group project for **Engenharia de Software** (Software Engineering) at Instituto Superior Técnico, 2025/26. It extends the open-source **HumanaEthica** volunteering platform with a full-stack **Shift** feature — domain model and invariants, REST services, a Vue.js UI, and an automated test suite across every layer.

**Grade: 19.7 / 20**

## Team
- **João Carvalho** — [ist157175](https://gitlab.rnl.tecnico.ulisboa.pt/ist157175)
- **João Alves** — [ist1106439](https://gitlab.rnl.tecnico.ulisboa.pt/ist1106439)

## Context

HumanaEthica is a broker that connects charities and non-profit organizations with volunteers. Institutions publish **activities**; volunteers **enroll** and take part as **participants**. Our delivery introduces **Shifts** — time slots within an activity, each with a location and a capacity — and threads them through enrollment and participation.

## What we built

**Backend (Spring Boot / Java 21)**
- `Shift` domain entity and its relationship to `Activity`, with DTOs and a repository (`shift/` package), plus the enrollment/participation changes needed to select shifts.
- Service + REST web layer to **create a shift** and to **list the shifts of an activity**.
- Domain invariants (each raising a specific error), verified by unit tests:
  - location is 20–200 characters;
  - the participant limit is present and greater than zero;
  - start and end dates are required, and the start is before the end;
  - shift dates fall within the activity's dates;
  - the activity must be **approved** before shifts can be added;
  - the sum of the shifts' participant limits cannot exceed the activity's participant limit;
  - a participant cannot be added beyond a shift's participant limit;
  - shifts selected in a single enrollment cannot have overlapping periods.

**Frontend (Vue.js + TypeScript)**
- `Shift.ts` model and `Activity.ts` updated to carry a list of shifts.
- A **Manage Shifts** page reachable from the institution's activity table, with a **shift-creation dialog** whose client-side validation mirrors the domain rules (e.g. the Save button stays disabled until the location length is valid, and creation is blocked unless the activity is approved).
- A **Shifts Capacity** column showing the v/t ratio (sum of shift capacities over the activity's participant limit).
- Enrollment UI with **multi-select of shifts**, and member UI to choose shifts per enrollment.

**Tests — the full pyramid**
- **Spock** unit tests for the domain invariants.
- **JUnit / Spring** integration tests for the web services (`*IT`).
- **Cypress** end-to-end tests for the UI flows (shift creation, validation, enrollment with shifts).

## Engineering process

The work was split into 16 tasks (T2.1–T2.16), each tracked as a **GitLab issue** with its own **merge request** and a **peer review** by the other member. The full task → commit → MR → review mapping is in [`delivery-reports/P2.md`](delivery-reports/P2.md). Continuous integration built the project and ran the tests on every push (`.gitlab-ci.yml`).

## Running it

The quickest path is Docker Compose, from the repository root:

```sh
cp data/access.log.example data/access.log
cp data/error.log.example data/error.log
cp frontend/example.env frontend/.env      # then uncomment for docker compose
docker compose build
docker compose up -d frontend              # app on http://localhost:8081, API on :8080
```

Run the test suites:

```sh
docker compose up be-unit-tests            # Spock unit tests + JaCoCo coverage
docker compose up integration-tests        # web-service integration tests
docker compose up e2e-run                  # Cypress end-to-end tests
```

A single test class can be targeted, e.g. `UNIT=CreateShiftMethodTest docker compose up be-unit-tests`. Full bare-metal setup (Postgres 14+, Java 21, Maven, Node 21) is documented in the base project linked below.

## Project layout

```
backend/            Spring Boot API (Java 21, Maven) — domain, services, REST, tests
frontend/           Vue.js + TypeScript client, Cypress e2e tests
data/               runtime logs (git-ignored)
delivery-reports/   the P2 submission report (tasks, MRs, reviews)
docker-compose.yml  one-command build / run / test
```

## Base project & license

Built on **[HumanaEthica](https://github.com/socialsoftware/humanaethica)** by IST/INESC-ID, distributed under the **MIT License** (© 2022 IST/INESC-ID) — see [`LICENSE`](LICENSE). The Shift feature and its tests are the contribution of Group AL-32.
