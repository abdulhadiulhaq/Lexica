# Lexica Search Engine — Program Flow & Usage Guide

**Lexica** is a custom, minimalist C++17 search and autocomplete engine built around zero-dependency data structures (`MyVector`, `MyHashMap`, `Trie`, `MinHeap`, and `LRUCache`).

---

## 1. System Architecture & Directory Layout

All source code is centralized under `src/` and cleanly divided into custom data structures (`src/ds/`) and core information-retrieval logic (`src/core/`):

```text
Lexica/
├── CMakeLists.txt
├── data/
│   └── sample/              # Corpus directory containing .txt documents
└── src/
    ├── ds/                  # Custom Data Structures
    │   ├── my_vector.hpp    # Dynamic array with 2x capacity scaling
    │   ├── my_hashMap.hpp   # Separate-chaining hash table with auto-rehashing
    │   ├── my_trie.hpp      # 26-way prefix tree for autocomplete
    │   ├── my_minHeap.hpp   # Binary min-heap for Top-K extraction
    │   └── my_lruCache.hpp  # O(1) Hash Map + Doubly Linked List query cache
    ├── core/                # Search Engine Pipeline
    │   ├── fileLoader.hpp   # Directory scanner & file reader
    │   ├── tokenizer.hpp    # Lowercasing, sanitization & word splitting
    │   ├── index.hpp        # Inverted Index & document postings
    │   ├── ranker.hpp       # TF-IDF relevance scoring & sorting
    │   └── editDistance.hpp # Space-optimized DP Levenshtein distance
    ├── engine.cpp           # Engine orchestration & state management
    ├── cli.cpp              # Interactive REPL command parser
    └── main.cpp             # Application entry point
```

---

## 2. Program Execution Flow

Lexica operates in two distinct phases: **Startup Indexing** and the **Interactive Query Loop**.

### Phase 1: Startup & Indexing Pipeline
When the executable starts, it automatically ingests the document corpus before accepting user commands:

```text
[data/sample/*.txt]
        │
        ▼
  1. FileLoader::loadDirectory()   ──► Assigns sequential doc_id (1..N) & reads text
        │
        ▼
  2. Tokenizer::tokenize()         ──► Converts to lowercase, strips punctuation, drops tokens < 2 chars
        │
        ├──► 3a. InvertedIndex::addDocument() ──► Maps each term to [ (doc_id, term_freq), ... ] in MyHashMap
        │
        ├──► 3b. Trie::insert()               ──► Inserts word into prefix tree & increments global frequency
        │
        └──► 3c. Vocabulary Registry          ──► Stores unique words in MyVector for fuzzy spell-checking
```

### Phase 2: Query Execution Pipeline
Once indexing completes, the engine enters an interactive Read-Eval-Print Loop (REPL):

1. **Search Query (`search <query>`):**
   * **Step 1 (Cache Check):** Checks `LRUCache` for the exact normalized query string.
     * *Cache Hit ($O(1)$):* Moves the node to the head of the doubly linked list and returns cached results immediately.
     * *Cache Miss:* Proceeds to the ranking pipeline.
   * **Step 2 (Tokenization):** Splits the query into clean tokens via `Tokenizer`.
   * **Step 3 (Retrieval & Scoring):** Looks up postings lists in `InvertedIndex` and accumulates **TF-IDF** scores for each matching document:
     $$\text{IDF}(t) = \ln\left(1 + \frac{N}{\text{df}(t)}\right), \quad \text{Score}(q, d) = \sum_{t \in q} \text{tf}(t, d) \times \text{IDF}(t)$$
   * **Step 4 (Top-K Extraction & Caching):** Extracts the top $K$ highest-scoring documents, stores the result in `LRUCache` (evicting the least recently used entry if at capacity), and prints the ranked filenames and scores.

2. **Autocomplete (`suggest <prefix>`):**
   * Traverses down the `Trie` character-by-character ($O(L)$ where $L$ is prefix length).
   * Runs a Depth-First Search (DFS) on the subtree to collect matching completions along with their corpus frequencies.

3. **Fuzzy Spell-Check (`fuzzy <word>`):**
   * Iterates over the indexed vocabulary and computes the **Levenshtein Edit Distance** using a two-row dynamic programming matrix.
   * Surfaces candidate corrections within an edit distance of $\le 2$.

---

## 3. Building & Running

### Compilation (CMake)
Ensure you have a C++17 compatible compiler (`g++`, `clang++`, or `MSVC`) and CMake installed:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Direct Compilation (`g++`)
Alternatively, compile directly from the project root:

```bash
g++ -std=c++17 -O2 -Isrc src/main.cpp -o lexica
```

### Running the Engine
Make sure your `.txt` corpus files are placed inside `data/sample/`, then run:

```bash
./lexica
```

---

## 4. CLI Usage & Command Reference

Once launched, you will be greeted by the `Lexica>` prompt.

| Command | Syntax | Description | Example |
| :--- | :--- | :--- | :--- |
| **Search** | `search <query>` | Tokenizes the query, checks the LRU cache, and returns the top 5 ranked documents using TF-IDF scoring. | `search binary search tree` |
| **Suggest** | `suggest <prefix>` | Queries the prefix `Trie` to return up to 5 autocomplete suggestions and their frequencies. | `suggest alg` |
| **Fuzzy** | `fuzzy <word>` | Finds vocabulary words within a Levenshtein edit distance of $\le 2$ to correct typos. | `fuzzy algorthm` |
| **Exit** | `exit` | Cleanly frees allocated memory across all custom data structures and exits the program. | `exit` |

### Sample Session
```text
========================================
        LEXICA SEARCH ENGINE (C++17)    
========================================
[INFO] Loaded 12 documents from data/sample
[INFO] Indexing complete. Ready for queries!

Lexica> suggest dat
Suggestions for 'dat':
  - data (freq: 42)
  - database (freq: 15)
  - dataset (freq: 8)

Lexica> fuzzy strcture
Closest matches for 'strcture':
  - Did you mean 'structure'? (Distance: 1)

Lexica> search hash table collision
Found 2 matching document(s):
  [1] Doc ID: 4 (hashing_notes.txt) - Score: 6.8421
  [2] Doc ID: 9 (dsa_overview.txt) - Score: 2.1972

Lexica> exit
```