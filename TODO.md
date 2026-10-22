# Lexicon v2: Modular To-Do

How to use: finish one module at a time, in order. A module is done only when every box is ticked, including its tests. Then merge its branch to `main` and tag it.

Rules: no STL maps, sets or priority queues in the core. `std::string` and `std::vector` are fine as plain storage. Time cap per module is a guide, not a target.

---

## M0: Setup (day 1)
Branch: `feat/setup` | Tag: `v0.0-setup`

- [ ] Create GitHub repo, clone or init locally
- [ ] Folder structure: `include/lexicon/{ds,core,cli}` (headers), `src/{core,cli}` (sources), `tests/support`, `bench/support`, `data/sample`, `docs`
- [ ] `CMakeLists.txt` builds a `lexicon_lib` static library, the `lexicon` executable, and one executable per test and benchmark file
- [ ] `.gitignore` (build folder, binaries, large corpora)
- [ ] `README.md` stub and `NOTES.md` (three lines per topic as you learn)
- [ ] Tiny test macro that prints PASS/FAIL per case
- [ ] Small sample corpus (5 to 10 `.txt` files) in `data/sample/`
- [ ] **Done when:** project builds and the empty test runner prints a result

## M1: Tokenizer and file loader (day 1 to 2)
Branch: `feat/tokenizer` | Tag: `v0.1-tokenizer`

- [ ] List all `.txt` files in a folder, assign docIDs
- [ ] Read a file into a string
- [ ] Tokenize: lowercase, split on non-letters, drop tokens under 2 chars
- [ ] Tests: empty file, punctuation only, mixed case, unicode bytes do not crash
- [ ] **Done when:** sample folder becomes a token list per document

## M2: Hash map, chaining (day 2)
Branch: `feat/hashmap` | Tag: `v0.2-hashmap`

- [ ] Hash function for strings
- [ ] `put`, `get`, `contains`, resize and rehash at a load-factor threshold
- [ ] Counters: entries, buckets, collisions, longest chain
- [ ] Tests: insert and look up 10,000 terms, overwrite existing key, missing key
- [ ] **Done when:** all tests pass and counters print correctly

## M3: Hash map, open addressing (day 3)
Branch: `feat/hashmap-open` | Tag: `v0.3-hashmap-open`

- [ ] Linear probing with the same interface as M2
- [ ] Resize before the table gets too full
- [ ] No delete (state this in the report)
- [ ] Tests: identical-behaviour test against the chaining map on 10,000 terms
- [ ] **Done when:** both maps return identical results for every lookup

## M4: Trie (day 4)
Branch: `feat/trie` | Tag: `v0.4-trie`

- [ ] Node with children and a frequency at word end
- [ ] `insert`, `contains`, `walkPrefix`
- [ ] `suggest(prefix)`: DFS to collect words under the prefix
- [ ] Count nodes for the stats command
- [ ] Tests: unknown prefix, prefix equals a full word, empty prefix
- [ ] **Done when:** autocomplete works on 20 hardcoded words

## M5: Min-heap (day 5)
Branch: `feat/heap` | Tag: `v0.5-heap`

- [ ] Array-backed heap, `push`, `pop`, `top`, sift up and down
- [ ] Bounded top-k helper (min-heap of size k)
- [ ] Wire the heap into `suggest` (top 5 by frequency)
- [ ] Tests: random inputs vs full sort, ties, k larger than n, k = 0
- [ ] **Done when:** heap top-k equals full-sort top-k on every test

## M6: Radix trie (day 5 to 6)
Branch: `feat/radixtrie` | Tag: `v0.6-radixtrie`

- [ ] Edge labels instead of single characters
- [ ] Insert with node splitting
- [ ] Same `suggest` interface as the standard trie
- [ ] Node count for comparison
- [ ] Tests: compare outputs with the standard trie on 100 random prefixes
- [ ] **Done when:** identical autocomplete results and a smaller node count

## M7: Inverted index and ranking (day 6 to 8)
Branch: `feat/index` | Tag: `v0.7-index`

- [ ] Postings lists: term to sorted `(docID, tf)` entries, stored in the hash map
- [ ] Document length table and df per term
- [ ] Precompute idf = ln(N / df)
- [ ] `search` with AND mode: two-pointer postings intersection
- [ ] `--any` mode (OR)
- [ ] TF-IDF scoring: sum of tf x idf, tf = count / document length
- [ ] Top-k with the heap, plus a full-sort baseline
- [ ] Tests: AND returns only docs containing every term, hand-computed score matches
- [ ] **Done when:** ranked search runs on the sample corpus and matches the baseline

## M8: Edit distance and BK-tree (day 8 to 9)
Branch: `feat/fuzzy` | Tag: `v0.8-fuzzy`

- [ ] Levenshtein distance with the DP table
- [ ] Known cases: kitten/sitting = 3, nueral/neural = 2, same word = 0
- [ ] BK-tree insert
- [ ] BK-tree search with tolerance 2 and triangle-inequality pruning
- [ ] Brute-force baseline over the whole dictionary
- [ ] Hook into `search` when a term is missing
- [ ] **Done when:** BK-tree results equal brute force on 100 misspellings, with far fewer comparisons

## M9: LRU cache (day 9 to 10)
Branch: `feat/cache` | Tag: `v0.9-cache`

- [ ] Doubly linked list plus hash map
- [ ] `get` moves entry to front, `put` evicts the tail when full
- [ ] Cache key = sorted terms + mode + k
- [ ] Hit and miss counters
- [ ] `set cache N`, with 0 disabling it
- [ ] Tests: eviction order, capacity 1, repeated query returns same result
- [ ] **Done when:** cache hit output equals cache miss output

## M10: CLI (day 10 to 11)
Branch: `feat/cli` | Tag: `v0.10-cli`

- [ ] Prompt loop and command parser
- [ ] `index`, `suggest`, `search`, `fuzzy`, `set`, `stats`, `bench`, `help`, `quit`
- [ ] Error messages: no index loaded, empty query, unknown command, missing folder
- [ ] `stats` prints counters from every structure
- [ ] **Done when:** the full sample session from the guide runs

## M11: Benchmarks (day 11 to 12)
Branch: `feat/bench` | Tag: `v0.11-bench`

- [ ] Fixed random seeds, `-O2`, discard warm-up, average of 3 runs
- [ ] Hash map: chaining vs open addressing vs sorted array
- [ ] Trie: standard vs radix (nodes, time)
- [ ] Top-k: heap vs full sort at k = 5, 10, 50, 100
- [ ] Fuzzy: BK-tree vs brute force
- [ ] Cache: on vs off
- [ ] Results saved as tables in `docs/`
- [ ] **Done when:** every table is complete and has one sentence of interpretation

## M12: Stretch (only if ahead)
Branch: `feat/stretch` | Tag: `v0.12-stretch`

- [ ] Skip pointers for postings intersection
- [ ] Delta + variable-byte encoding of postings
- [ ] **Cut first if time is short**

## M13: Report, README, release (day 13 to 14)
Branch: `docs/report` | Tag: `v1.0`

- [ ] README: build steps, command list, sample session
- [ ] Report: structure choices with justification
- [ ] Report: complexity table with a one-line reason each
- [ ] Report: benchmark tables and discussion
- [ ] Report: honest limitations (no phrase search, no delete in open addressing, TF-IDF not BM25)
- [ ] Clean build from scratch on a fresh clone
- [ ] All tests pass
- [ ] Five-minute demo rehearsed: autocomplete, ranked search, misspelling, cache hit, benchmark table
- [ ] **Done when:** `v1.0` tag is pushed

---

## Git routine for every module

1. `git switch -c feat/<module>`
2. Commit each time a test passes: `git commit -m "trie: add prefix suggestion via DFS"`
3. `git push -u origin feat/<module>`
4. When the module's checklist is complete: `git switch main`, `git merge feat/<module>`, `git push`
5. Tag it: `git tag v0.x-<name>` then `git push --tags`

## Cut order if time runs short

1. Skip pointers and compression (M12)
2. Radix trie (M6)
3. Open-addressing map (M3)

Never cut tests or benchmarks.
