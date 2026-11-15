Here is a thorough, comprehensive architectural audit of your complete **Lexica** repository structure and codebase based on the provided project archive.

---

## ## 1. Repository Structure & Layout

Your project is organized into a clean, modular repository layout that separates low-level data structures, core search engine logic, front-end interfaces, and documentation:

```text
Lexica/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── TODO.md
├── bench/
├── data/
├── docs/
│   ├── 1_projectIntro.md
│   ├── 2_projectSpecification.md
│   ├── 3_programFlow&Usage.md
│   └── 4_codeBaseAudit.md
└── src/
    ├── 1_ds/                  # Custom STL-free data structures
    ├── 2_core/                # Headless engine backend & modules
    ├── 3_frontEnd/            # CLI and Web GUI interfaces
    └── main.cpp               # Command-line argument dispatcher

```

---

## ## 2. Core Data Structures (`src/1_ds/`) Audit

All fundamental computer science data structures have been custom-built from scratch with strict adherence to memory safety and zero external dependencies:

* **`MyVector` (`1_vector.hpp`)**: Dynamic array featuring geometric capacity expansion ($\times 2$), robust pointer-based iterators (`begin()`, `end()`), and safe bounds access.
* **`MinHeap` (`2_minHeap.hpp`)**: Priority queue implementation utilizing sift-up and sift-down mechanics to maintain $O(K)$ space efficiency for top-$K$ ranking.
* **`Trie` (`3_trie.hpp`)**: Prefix tree optimized for lowercase alphabetic keys ($a-z$), driving fast prefix-based autocomplete searches via recursive DFS collection.
* **`MyHashMap` (`4_hashMap.hpp`)**: Chaining hash map utilizing the DJB2 hash algorithm and a dynamic load-factor threshold (>70%) with automatic rehashing.
* **`LRUCache` (`5_lruCache.hpp`)**: Hybrid cache combining a hash map for $O(1)$ key lookups with a doubly-linked list for $O(1)$ most-recently-used (MRU) promotion and least-recently-used (LRU) tail eviction.

---

## ## 3. Core Engine & Modules (`src/2_core/`) Audit

The headless backend orchestrator binds all modular components together cleanly:

* **`FileLoader` (`1_fileLoader.hpp`)**: Leverages C++17 `<filesystem>` to scan directories for regular `.txt` files, loading stream buffers into memory and assigning sequential 1-based `docID`s.
* **`Tokenizer` (`2_tokenizer.hpp`)**: Strips punctuation, lowercases characters, filters out tokens shorter than 2 characters, and extracts clean word tokens.
* **`DocumentStore` (`3_documentStore.hpp`)**: Manages document metadata and content storage sequentially, providing secure 1-based ID lookups.
* **`InvertedIndex` (`4_invertedIndex.hpp`)**: Maps unique terms to posting lists (`MyVector<Posting>`), suppressing duplicate token entries per document.
* **`Ranker` (`5_ranker.hpp`)**: Computes TF-IDF relevance scores ($\text{TF} \times \text{IDF}$) across matching documents, sorts candidate results descending using insertion sort, and truncates to the requested `topK`.
* **`EditDistance` (`6_editDistance.hpp`)**: Implements space-optimized $O(N)$ Levenshtein distance using rolling rows (`prev` and `curr` arrays) to support fuzzy spell correction.
* **`Engine` (`engine.cpp`)**: Orchestrates initialization timing via `std::chrono::high_resolution_clock`, manages the inverted index, autocomplete trie, and integrates the query `LRUCache`.

---

## ## 4. Frontend & Entrypoint Audit (`src/3_frontEnd/` & `src/main.cpp`)

* **CLI Frontend (`1_cli/cli.hpp`)**: Provides an interactive terminal command loop supporting `index`, `suggest`, `search` (with `-k` flag parsing), `fuzzy`, `stats`, `help`, and `quit`.
* **Web GUI (`2_webGUI/`)**: Houses server code, HTML/CSS assets, and JavaScript handlers for a browser-based interface.
* **Argument Dispatcher (`src/main.cpp`)**: Safely parses command-line arguments using string-wrapped `argv` comparisons to launch either the CLI or Web GUI seamlessly.

---

## ## 5. Documentation (`docs/`) Audit

Your documentation suite is exceptionally thorough, covering project introduction, technical specifications, program flow, usage instructions, and codebase audits.

### Summary of Audit Status

* **STL-Free Compliance**: Core data structures and backend logic rely entirely on custom implementations.
* **Memory Management**: Destructors across hash maps, tries, and LRU caches perform exhaustive node sweeps preventing memory leaks.
* **Code Style**: Uniform brace initialization `{}` and explicit `camelCase` conventions are consistently applied.