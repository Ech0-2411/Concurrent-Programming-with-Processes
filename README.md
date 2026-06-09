# CS 270 – Project 5: Concurrent Word Counter (Map-Reduce) (UNCLOMPLETE - TO BE ADDED LATER)

> **Course:** CS 270: Systems Programming – Spring 2025, University of Kentucky
> **Author:** Hamblet Arroyo

---

## Overview

A concurrent word-counting framework modeled after the Map-Reduce paradigm. Multiple **scanner** processes read files/directories in parallel, count word occurrences, and send results through pipes to **reducer** processes that aggregate the totals. A **driver** process orchestrates everything and prints the final sorted output.

---

## Files

| File | Description |
|---|---|
| `scanner.c` | Scanner process — walks files/dirs, counts words, sends to reducers |
| `reduce.c` | Reducer process — aggregates word counts from scanners, sends to driver |
| `driver.c` | Driver process — spawns processes, collects output (provided, do not modify) |
| `amap.h` / `amap.c` | Sorted counter map implementation (provided, do not modify) |
| `aux.h` / `aux.c` | Pipe read/write helpers (provided, do not modify) |
| `Makefile` | Builds the `driver` executable |

---

## Build & Run

```bash
make
./driver <file_or_dir> [file_or_dir ...]
```

**Example:**

```bash
./driver td1 td2
```

Output is a sorted list of `word count` pairs printed to stdout.

---

## Architecture

```
argv[1] --> [ scanner 0 ] --\
                             +--> [ reducer 0 (a-i) ] --\
argv[2] --> [ scanner 1 ] --X                            +--> [ driver ] --> stdout
                             +--> [ reducer 1 (j-r) ] --/
argv[3] --> [ scanner 2 ] --/    [ reducer 2 (s-z) ] --/
```

One scanner and one reducer process are forked per command-line argument. Scanners distribute word counts to reducers based on the first letter of each word (via the `whichpipe` map). Reducers aggregate and forward to the driver in alphabetical order.

---

## Process Roles

**Scanner** (`scanner.c`)
- Recursively walks the given file or directory
- Skips binaries, images, and PDFs — processes ASCII text files only
- Tokenizes input into alphabetic-only, lowercase words
- Sends `(word, count)` pairs to the appropriate reducer pipe based on first letter
- Closes all reducer pipe write ends when done

**Reducer** (`reduce.c`)
- Reads `(word, count)` pairs from its dedicated reducer pipe
- Accumulates totals in an `amap_t` counter map
- Dumps all entries to its driver pipe when the scanner pipes reach EOF
- Closes all unneeded pipe ends to prevent deadlock

---

## IPC Design

- **Reducer pipes** — one per reducer; all scanners write to all reducer pipes; each reducer reads only from its own
- **Driver pipes** — one per reducer; each reducer writes its final output; the driver reads sequentially
- Pipe EOF signals process completion — no other synchronization needed
- Words starting with letters `a–i` → reducer 0, `j–r` → reducer 1, `s–z` → reducer 2 (with N processes, ranges are divided evenly)

---

## References

- CS 270 Project 5 handout — University of Kentucky
- `man 2 pipe`, `man 2 fork`, `man 3 opendir`
