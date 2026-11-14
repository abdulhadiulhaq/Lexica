# Lexica

A command-line search engine and data structures lab, written in C++ from scratch.

Lexica indexes a folder of text files and lets you autocomplete words, run ranked searches, and get spelling suggestions. Every feature is powered by a data structure built by hand, and each one is benchmarked against an alternative.

> **Status:** in development. See [TODO.md](TODO.md) for module progress. Sample outputs below are illustrative until benchmarks are finalised.

## Features

| Feature | Command | Powered by |
|---|---|---|
| Index a folder | `index <folder>` | Tokenizer, hash map, trie, BK-tree |
| Autocomplete | `suggest <prefix>` | Trie or radix trie, min-heap for top 5 |
| Ranked search | `search <terms> [-k N] [--any]` | Inverted index, postings intersection, TF-IDF, min-heap top-k |
| Spelling suggestions | `fuzzy <word>` | Edit distance (DP) and BK-tree |
| Query caching | `set cache <N>` | LRU cache (hash map + doubly linked list) |
| Swap internals live | `set map chain\|open`, `set trie std\|radix` | Two hash maps, two tries behind one interface |
| Statistics | `stats` | Counters kept by every structure |
| Benchmarks | `bench <name>` | Fixed-seed timing harness |

## Example session

```
lexica> index data/books
Reading files ........ 1,204 files
Tokenizing ........... 8,412,330 tokens
Unique terms ......... 71,904
Indexed in 2.41 s

lexica> suggest neu
  neural    (412)
  neuron    (210)
  neutral   (198)
  neutron   (133)
  neurology (87)

lexica> search neural network -k 3
Cache: MISS
  1. deep_learning.txt     0.0814
  2. ai_history.txt        0.0577
  3. brain_notes.txt       0.0426

lexica> fuzzy nueral
Did you mean: neural (distance 2)
[BK-tree visited 214 of 71,904 words]
```

## Data structures

All built from scratch. `std::string` and `std::vector` are used only as plain storage.

| Structure | Used for | Compared against |
|---|---|---|
| Hash map (chaining) | Inverted index | Open addressing, sorted array |
| Hash map (open addressing) | Inverted index | Chaining |
| Trie | Autocomplete | Radix trie |
| Radix trie | Autocomplete with fewer nodes | Standard trie |
| Min-heap | Top-k ranking and suggestions | Full sort |
| BK-tree | Fuzzy lookup | Brute-force scan |
| LRU cache | Repeated queries | No cache |
| Sorted postings intersection | AND queries | Plain merge vs skip pointers (stretch) |

## How search works

1. Query terms are normalised with the same tokenizer used at index time.
2. The LRU cache is checked first. A hit returns immediately.
3. On a miss, each term's postings list is fetched from the hash map.
4. Lists are intersected (AND) or merged (OR).
5. Each candidate document is scored with TF-IDF: the sum of `tf x idf` over query terms, where `tf = count / document length` and `idf = ln(N / df)`.
6. A min-heap of size k keeps the best results.
7. The result is stored in the cache and printed.

## Build

Requires a C++17 compiler and CMake.

```bash
cmake -S . -B build
cmake --build build
```

Run it:

```bash
./build/lexica
```

Run the tests:

```bash
./build/test_hashmap
./build/test_trie
./build/test_heap
```

Run a benchmark (built with `-O2`):

```bash
./build/bench_topk
```

## Project structure

```
src/
  ds/       hashmap_chain.h  hashmap_open.h  trie.h  radix_trie.h
            min_heap.h  bk_tree.h  lru_cache.h
  core/     tokenizer  file_loader  index  ranker  edit_distance  engine
  cli/      commands  main
tests/      one test executable per structure
bench/      one benchmark executable per comparison
data/sample/  small corpus for tests and demos
docs/       report and benchmark tables
```

## Corpus

A small sample corpus is included in `data/sample/`. For benchmarks, use a larger collection of plain-text files such as books from Project Gutenberg. The large corpus is not committed to this repository.

## Benchmarks

Fixed random seeds, `-O2`, warm-up run discarded, average of three runs. Full tables are in `docs/`.

| Experiment | Compares | Metric |
|---|---|---|
| Hash map | Chaining vs open addressing vs sorted array | Lookup time, collisions, load factor |
| Trie | Standard vs radix | Node count, lookup time |
| Top-k | Heap vs full sort | Time, crossover point |
| Fuzzy | BK-tree vs brute force | Words compared, time |
| Cache | On vs off | Hit rate, speedup |

## Limitations

- No phrase search (would need positional postings).
- No live file watching. Re-run `index` after files change.
- Command line only, no GUI.
- Plain TF-IDF ranking, no BM25 and no stemming.
- Plain-text files only.
- The open-addressing hash map has no delete, because the index is built once and never edited.
- In-memory only. The index is rebuilt on each run.

## Testing

Each structure has tests that check key invariants:

- Both hash maps return identical results.
- Both tries return identical autocomplete.
- Heap top-k equals full-sort top-k.
- BK-tree results equal brute force.
- A cache hit returns exactly what the miss returned.