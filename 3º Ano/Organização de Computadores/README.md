# Computer Organization — Memory Hierarchy & Pipelines

Three-part systems project for **Organização de Computadores** (Computer Organization) at Instituto Superior Técnico — **2025/26**, group **38**. A hands-on study of the memory hierarchy and the CPU pipeline, spanning hardware performance counters, a virtual-memory simulator, and RISC-V pipeline analysis.

**Project grade: 20/20.**

## Team
- João Carvalho — ist 57175
- Duarte Mendonça - ist 105882
- Rodrigo Santos - ist 107032

## The three assignments

### Part 1 — System Modeling & Profiling (`Parte 1/`)
Characterizing a real machine's caches and using that knowledge to optimize code, with the **PAPI** hardware-performance-counter API.
- C micro-benchmarks sweep array sizes and access **strides** to infer the **cache size, block size and L1 miss penalty** from the timing / miss-rate curves.
- A **matrix-multiply** kernel is then optimized against the measured **L2 cache** characteristics (access order / blocking) and profiled with PAPI.

### Part 2 — TLB Cache Simulator (`Parte 2/`)
A two-level **Translation Lookaside Buffer** implemented in C on top of a provided virtual-memory simulator (page table + DRAM + backing storage).
- Both TLB levels are **fully associative**, with **LRU** eviction and a **write-back** policy.
- Implements address translation (`tlb_translate`) and invalidation (`tlb_invalidate`), tracking per-level hit / miss / invalidation counters, and handling page faults and page eviction to disk.
- Built with `make` (gcc `-O3`) and validated against the provided memory-trace inputs and expected outputs (L1-only and full L1+L2 versions).

### Part 3 — Instruction-Level Parallelism (`Parte 3/`)
Pipeline analysis and optimization of a RISC-V program (`prog.s`, computing `C[i] = A[i] + B[i]·A[i]`) in the **Ripes** simulator.
- Traces the 5-stage pipeline **without forwarding** — pipeline diagram (F/D/X/M/W), CPI, and the simulator's branch-prediction policy.
- Repeats **with data forwarding** and measures the resulting speedup.
- Applies **instruction re-ordering** to minimize the remaining data and structural hazards, keeping the output unchanged.

## Building & running

- **Part 1** — build PAPI from `lab1_kit` (`./configure && make`), then compile and run the C benchmarks. Hardware-dependent (meant to run on the lab machines).
- **Part 2** — `make` in `oc-25-26-tlb-simulator-v3/`, then `./build/tlbsim inputs/<trace>.txt`; `run_tlbsim_l1_tests.sh` / `run_tlbsim_l2_tests.sh` diff against the expected outputs.
- **Part 3** — open `prog.s` in [Ripes](https://github.com/mortbopet/Ripes) and select the 5-stage processor (with / without forwarding).

## Stack

C (gcc) · PAPI (hardware performance counters) · RISC-V assembly · Ripes simulator.
