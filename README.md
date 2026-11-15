# Lexica

A high-performance, header-only C++17 search engine and data structures lab built completely from scratch with manual memory management and custom-crafted data structures.

Lexica indexes a folder of text files, builds an inverted index, and provides both an interactive terminal CLI and a Web GUI server to autocomplete words, run TF-IDF ranked searches, and perform fuzzy spell-checking.

> **Status:** Stable core memory management implemented. Supports CLI REPL and Web GUI interfaces.

## Features & Commands

| Feature | Command / Flag | Description |
|---|---|---|
| Index corpus | `index <folder>` | Scans directory, tokenizes documents, builds inverted index & trie |
| Autocomplete | `suggest <prefix>` | Returns prefix-matching suggestions with frequencies from the Trie |
| Ranked search | `search <query> [-k <num>]` | Evaluates queries via TF-IDF ranking with LRU query caching |
| Spell check / Fuzzy | `fuzzy <word>` | Finds closest vocabulary match using Levenshtein edit distance |
| Corpus statistics | `stats` | Displays total document count and unique term count |
| Help menu | `help` | Lists all available CLI commands |
| Exit CLI | `quit` or `exit` | Closes the Lexica terminal session |
| Web GUI Server | `--gui <port>` | Launches the HTTP server interface (e.g., `./build/lexica --gui 8080`) |

## Example Session

```
Lexica> index data/sample
[INFO] Reading files from: data/sample...
Indexing completed successfully!
--------------------------------------------------
  Files Indexed : 3
  Total Tokens  : 1,420
  Unique Terms  : 312
  Time Elapsed  : 0.01042 s
--------------------------------------------------

Lexica> suggest neu
[INFO] Autocomplete Suggestions for prefix: "neu"
--------------------------------------------------
  1. neural    (freq: 4)
  2. network   (freq: 3)
--------------------------------------------------

Lexica> search neural network -k 3
[INFO] Search Results for: "neural network"
--------------------------------------------------
  Cache Status : MISS
--------------------------------------------------
  1. ai_notes.txt  [0.1425]
  2. deep_learning.txt  [0.0891]
--------------------------------------------------

Lexica> fuzzy nueral
[INFO] Spell Check / Fuzzy Search for: "nueral"
--------------------------------------------------
  Did you mean: neural (distance 2)
--------------------------------------------------
```

## Custom Data Structures (`src/1_ds/`)

All core structures are built from scratch with manual memory management (Rule-of-Five copy/move semantics). STL containers (`std::vector`, `std::unordered_map`) are avoided in the core engine components.

| Structure | Purpose in Lexica |
|---|---|
| `MyVector` | Custom dynamic array with contiguous heap allocation and safe iterators |
| `MyHashMap` | Custom hash map used for vocabulary and posting lists |
| `Trie` | Prefix tree used for fast autocomplete suggestions |
| `MinHeap` | Priority queue structure used for ranking and top-k retrieval |
| `LRUCache` | Fast query cache to store and retrieve recent search results |

## Architecture & Modules (`src/2_core/`)

* **FileLoader (`1_fileLoader.hpp`)**: Scans directories recursively for `.txt` files and loads documents into memory.
* **Tokenizer (`2_tokenizer.hpp`)**: Normalizes text to lowercase, strips non-alphabetic characters, and extracts clean tokens.
* **DocumentStore (`3_documentStore.hpp`)**: Manages document metadata and raw text lookups by docID.
* **InvertedIndex (`4_invertedIndex.hpp`)**: Maps terms to document postings lists.
* **Ranker / TF-IDF (`5_ranker.hpp`)**: Computes term frequency and inverse document frequency scores.
* **EditDistance (`6_editDistance.hpp`)**: Implements Levenshtein distance algorithms for fuzzy matching.
* **Engine (`engine.hpp`)**: Central orchestration class binding the index, cache, and search subsystems together.

## Build Instructions

Requires a C++17 compatible compiler and CMake.

```bash
# Configure and build the project
cmake -S . -B build
cmake --build build --config Release
```

## Running the Application

* **Run the Interactive CLI REPL:**
  ```bash
  ./build/lexica
  ```

* **Run the Web GUI Server:**
  ```bash
  ./build/lexica --gui 8080
  ```
  Then open your browser at `http://localhost:8080`.

## Project Structure

```
src/
  1_ds/            Custom data structures (vector, hashmap, trie, heap, lru)
  2_core/          Engine core modules (file loader, tokenizer, index, ranker, etc.)
  3_frontEnd/
    1_cli/         Command-line interface (cli.hpp)
    2_webGUI/      Web GUI server (gui.hpp using cpp-httplib)
  main.cpp         CLI / GUI entry point & flag parsing
data/sample/       Sample plain-text corpus for local testing
```

## Corpus

Place your plain-text (`.txt`) files inside the `data/sample/` directory, or pass any custom directory path directly to the `index` command in the CLI.