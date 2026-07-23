# BlockchainIST

Distributed Systems Project 2026

**Group A49**

**Difficulty level: I am Death incarnate!**

**Grade:** 17.3 / 20 — A.1 4.22/5 · A.2 1.33/2 · B.1 3.87/4 · B.2 2.65/3 · C.1 2.70/3 · C.2 2.5/3

<!-- ### Code Identification

In all source files (namely in the *groupId*s of the POMs), replace __GXX__ with your group identifier. The group
identifier consists of either A or T followed by the group number - always two digits. This change is important for 
code dependency management, to ensure your code runs using the correct components and not someone else's. -->

### Team Members


| Number | Name              | User                             | Email                                               |
|--------|-------------------|----------------------------------|-----------------------------------------------------| 
| 57175  | João Carvalho     | <https://github.com/JoaoPSC1987> | <mailto:joao.p.santos.carvalho@tecnico.ulisboa.pt>  |
| 109822 | Pedro Brito       | <https://github.com/p-b-b>       | <mailto:pedro.d.brito@tecnico.ulisboa.pt>           |
| 116424 | Gabriel Amaral    | <https://github.com/amaral220x>  | <mailto:gabriel.amaral@tecnico.ulisboa.pt>          |

## Getting Started

The overall system is made up of several modules.
The definition of messages and services is in _Contract_.

See the [Project Statement](https://github.com/tecnico-distsys/BlockchainIST-2026) for a complete domain and system description.

### Prerequisites

The Project is configured with Java 17 (which is only compatible with Maven >= 3.8), but if you want to use Java 11 you
can too -- just downgrade the version in the POMs.

To confirm that you have them installed and which versions they are, run in the terminal:

```s
javac -version
mvn -version
```

### Installation

To compile and install all modules:

```s
mvn clean install
```

And generate the keys (public and private) for the nodes:

```s
chmod +x generate_keys.sh
./generate_keys.sh
```
## Fixes 2026

After the final discussion, the code was audited against the feedback received and several correctness and quality issues were fixed. The distributed logic is unchanged in spirit — these are corrections to how it was implemented.

### Security — the transaction key was outside the signature

The idempotency key that identifies each transaction (used to deduplicate retries in the fault-tolerance mechanism) was travelling as a gRPC **header**, *outside* the user's signed request. Since the node↔sequencer channel is explicitly **not** a secure channel, that key could be tampered with without breaking the signature — which would let the same transaction be applied twice, or a legitimate one be dropped as a duplicate.

The key is now a field **inside** the signed request, and each node checks, when applying a block, that the transaction key still matches the one the user signed. The key now travels *inside* the vault, not next to it.

### Concurrency

- **Single lock for the wallet state.** The wallet map was being mutated through two paths guarded by *different* locks (a `ReadWriteLock` on one side, `synchronized` on the other, and one path with no lock at all). Every access now goes through the same `ReadWriteLock`, removing a real data race.
- **Per-wallet locks.** Creating a lock with `put` let two concurrent requests for the same wallet end up with *different* locks (no mutual exclusion). Replaced with an atomic `computeIfAbsent`.

### Causal order (C.1)

For optimistic transfers, the node was replying to the client **before** broadcasting to the sequencer, which could let a later operation be ordered ahead of an earlier one. The broadcast now happens **before** the reply — the client only gets its "OK" once the sequencer has accepted the operation's order. The optimization is preserved: the node still does not wait for the *block*.

### Chain integrity

Blocks are now applied strictly in order: the node tracks the expected sequence number and refuses out-of-order blocks. A block that fails signature verification therefore halts the chain (safe) instead of being silently skipped and leaving a gap in the state.

### Miscellaneous

Fixed an inverted `canDeleteWallet` condition, removed a stray IDE import, dead code and unused variables, and corrected several typos and misleading comments.


## Built With

* [Maven](https://maven.apache.org/) - Build and dependency management tool;
* [gRPC](https://grpc.io/) - RPC framework.
