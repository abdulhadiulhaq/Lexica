# Lexica: Custom C++17 Search Engine (Streamlined ~2,000 LOC Edition)

A command-line search engine and data structures lab built from scratch in modern C++17. Lexica indexes a directory of plain-text files and supports prefix autocomplete, TF-IDF ranked multi-word search, and fuzzy spelling suggestions—all powered by hand-rolled data structures without relying on STL maps, sets, or priority queues.

---

## 1. Core Engineering Rules & Constraints
* **No Core STL Containers:** `std::unordered_map`, `std::map`, `std::unordered_set`, `std::set`, and `std::priority_queue` are strictly prohibited in the engine and data structures.
* **Permitted Storage:** `std::string` and `std::vector` (or custom `MyVector`) are used purely as contiguous underlying storage.
* **Target Scope:** Streamlined to **~1,600 – 2,100 LOC** total (including unit tests) by focusing on the four essential search engine pillars while cutting redundant structure variants.

---

## 2. Four Core Features & Architecture

| Feature | CLI Command | Custom Data Structures & Algorithms |
| :--- | :--- | :--- |
| **1. Corpus Indexing** | `index <folder>` | File Loader (`<filesystem>`), Tokenizer, **Chaining Hash Map** |
| **2. Autocomplete** | `suggest <prefix>` | **Standard Trie** (DFS traversal) + **Min-Heap** (top-5 frequency) |
| **3. Ranked Search** | `search <terms> [-k N] [--any]` | **Inverted Index**, Postings Intersection/Merge, **TF-IDF**, **Min-Heap** |
| **4. Fuzzy Matching** | `fuzzy <word>` | **Levenshtein Edit Distance** (DP) + Dictionary Scan / **Min-Heap** |

---

## 3. Data Structures & Algorithms Breakdown

### A. Data Structures (`include/ds/`)
1. **Hash Map with Separate Chaining (`hashmap_chain.hpp`)**
   * **Purpose:** Backs the Inverted Index (mapping terms to postings lists) and stop-word filtering.
   * **Details:** Custom string hash function (e.g., DJB2 or FNV-1a), linked-list buckets for collision resolution, dynamic resizing/rehashing at a load-factor threshold, and metric counters (entries, buckets, collisions, longest chain).
2. **Standard Trie (`trie.hpp`)**
   * **Purpose:** Powers instant prefix lookups for autocomplete (`suggest`).
   * **Details:** Prefix tree nodes storing child pointers, end-of-word flags, and word occurrence frequencies. Uses DFS traversal from the prefix node to gather candidates.
3. **Array-Backed Min-Heap (`min_heap.hpp`)**
   * **Purpose:** Efficiently extracts the Top-$K$ documents in ranked search and Top-5 words in autocomplete/fuzzy matching in $O(N \log K)$ time.
   * **Details:** Supports `push`, `pop`, `top`, `siftUp`, and `siftDown` with bounded capacity $K$.

### B. Core Engine Logic (`include/core/` & `src/core/`)
1. **File Loader & Tokenizer (`file_loader.cpp`, `tokenizer.cpp`)**
   * Recursively scans a target folder for `.txt` files and assigns unique numeric `docID`s.
   * Normalizes text: converts characters to lowercase, strips punctuation/non-letters, drops tokens shorter than 2 characters, and filters stop-words.
2. **Inverted Index (`index.cpp`, `posting.hpp`)**
   * Maps each unique term to a sorted list of `(docID, termFrequency)` postings, alongside a document-length table and document frequency (`df`) counts.
3. **Ranker & Query Engine (`ranker.cpp`, `engine.cpp`)**
   * **Boolean Filtering:** Two-pointer sorted intersection for `AND` queries (default) and union merge for `OR` queries (`--any`).
   * **TF-IDF Scoring:** Ranks matching documents using:
     $$\text{TF}(t, d) = \frac{\text{count of } t \text{ in } d}{\text{total words in } d}, \quad \text{IDF}(t) = \ln\left(\frac{N}{\text{df}(t)}\right)$$
     $$\text{Score}(q, d) = \sum_{t \in q} \text{TF}(t, d) \times \text{IDF}(t)$$
4. **Levenshtein Edit Distance (`edit_distance.cpp`)**
   * Computes minimum single-character insertions, deletions, and substitutions between two strings via Dynamic Programming to suggest close matches ($\text{distance} \le 2$) when a query term is misspelled.

---

## 4. Streamlined Project Directory Structure

```text
Lexica/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── ds/
│   │   ├── hashmap_chain.hpp
│   │   ├── trie.hpp
│   │   └── min_heap.hpp
│   ├── core/
│   │   ├── tokenizer.hpp
│   │   ├── file_loader.hpp
│   │   ├── posting.hpp
│   │   ├── index.hpp
│   │   ├── ranker.hpp
│   │   ├── edit_distance.hpp
│   │   └── engine.hpp
│   └── cli/
│       └── commands.hpp
├── src/
│   ├── core/
│   │   ├── tokenizer.cpp
│   │   ├── file_loader.cpp
│   │   ├── index.cpp
│   │   ├── ranker.cpp
│   │   ├── edit_distance.cpp
│   │   └── engine.cpp
│   ├── cli/
│   │   └── commands.cpp
│   └── main.cpp
├── tests/
│   ├── support/
│   │   └── test_macro.hpp
│   ├── test_tokenizer.cpp
│   ├── test_hashmap.cpp
│   ├── test_trie.cpp
│   ├── test_heap.cpp
│   ├── test_index.cpp
│   └── test_edit_distance.cpp
└── data/
    └── sample/
```

---

## 5. Estimated LOC Budget (~2,000 LOC)

| Component | Target LOC | Notes |
| :--- | :--- | :--- |
| **Data Structures (`include/ds/`)** | ~350 – 450 LOC | Chaining Hash Map, Standard Trie, Min-Heap |
| **Core Engine (`include/core/`, `src/core/`)** | ~450 – 550 LOC | Tokenizer, Loader, Inverted Index, TF-IDF, Levenshtein DP |
| **CLI & Entrypoint (`src/cli/`, `src/main.cpp`)** | ~150 – 200 LOC | Interactive REPL (`index`, `suggest`, `search`, `fuzzy`, `stats`, `help`, `quit`) |
| **Unit Tests (`tests/`)** | ~600 – 800 LOC | Invariant checks (Heap vs. `std::sort`, Map lookups, Index math) |
| **Total Projected Codebase** | **~1,550 – 2,000 LOC** | Clean, modular, and completely self-contained |

---

## 6. Streamlined Implementation Roadmap

1. **Phase 1: Setup & Ingestion (`feat/setup`, `feat/tokenizer`)**
   * Configure CMake, test macros, sample `.txt` corpus, file loader, and text tokenizer.
2. **Phase 2: Core Data Structures (`feat/hashmap`, `feat/trie`, `feat/heap`)**
   * Implement and unit-test the Chaining Hash Map, Standard Trie, and Min-Heap independently.
3. **Phase 3: Search, Ranking & Fuzzy Logic (`feat/index`, `feat/fuzzy`)**
   * Wire the Inverted Index, two-pointer postings intersection, TF-IDF ranker, and Levenshtein spell-checker.
4. **Phase 4: Interactive CLI & Final Verification (`feat/cli`, `v1.0`)**
   * Build the command loop (`index`, `suggest`, `search`, `fuzzy`, `stats`), run full end-to-end tests, and finalize documentation.