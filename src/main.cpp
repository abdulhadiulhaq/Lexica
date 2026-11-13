#include <iostream>
#include <string>
#include "my_vector.hpp"
#include "my_hashmap.hpp"
#include "trie.hpp"
#include "min_heap.hpp"
#include "tokenizer.hpp"
#include "file_loader.hpp"
#include "edit_distance.hpp"
#include "index.hpp"
#include "ranker.hpp"

int main(int argc, char* argv[]) 
{
    std::cout << "========================================\n";
    std::cout << "        LEXICA SEARCH ENGINE (C++17)    \n";
    std::cout << "========================================\n";

    InvertedIndex index;
    Trie autocomplete_trie;
    MyHashMap<int, std::string> doc_id_to_filename;
    MyVector<std::string> vocabulary;
    MyHashMap<std::string, bool> vocab_set;

    // 1. Load sample documents from data/sample/ directory
    std::string data_dir = "data/sample";
    MyVector<Document> docs = FileLoader::loadDirectory(data_dir);
    
    std::cout << "[INFO] Loaded " << docs.size() << " documents from " << data_dir << "\n";

    for (auto i {0}; i < docs.size(); ++i) 
    {
        const Document& doc = docs[i];
        doc_id_to_filename.insert(doc.doc_id, doc.filename);

        MyVector<std::string> tokens = Tokenizer::tokenize(doc.content);
        index.addDocument(doc.doc_id, tokens);

        for (auto j {0}; j < tokens.size(); ++j) 
        {
            autocomplete_trie.insert(tokens[j], 1);
            bool exists = false;
            if (!vocab_set.get(tokens[j], exists)) 
            {
                vocab_set.insert(tokens[j], true);
                vocabulary.push_back(tokens[j]);
            }
        }
    }

    std::cout << "[INFO] Indexing complete. Ready for queries!\n\n";
    std::cout << "Commands:\n";
    std::cout << "  search <query>   - Search ranked documents\n";
    std::cout << "  suggest <prefix> - Autocomplete prefix\n";
    std::cout << "  fuzzy <word>     - Spell correction / Did you mean\n";
    std::cout << "  exit             - Quit program\n\n";

    std::string command;
    while (true) 
    {
        std::cout << "Lexica> ";
        std::cin >> command;

        if (command == "exit") 
        {
            break;
        }
        else if (command == "search") 
        {
            std::string query_str;
            std::getline(std::cin, query_str);
            
            MyVector<std::string> q_tokens = Tokenizer::tokenize(query_str);
            MyVector<SearchResult> ranked = Ranker::rankQuery(q_tokens, index, index.getTotalDocuments(), 5);

            std::cout << "Found " << ranked.size() << " matching document(s):\n";
            for (auto i {0}; i < ranked.size(); ++i) 
            {
                std::string fname = "";
                doc_id_to_filename.get(ranked[i].doc_id, fname);
                std::cout << "  [" << (i + 1) << "] Doc ID: " << ranked[i].doc_id 
                          << " (" << fname << ") - Score: " << ranked[i].score << "\n";
            }
        }
        else if (command == "suggest") 
        {
            std::string prefix;
            std::cin >> prefix;

            MyVector<std::pair<std::string, int>> suggestions = autocomplete_trie.suggest(prefix);
            std::cout << "Suggestions for '" << prefix << "':\n";
            size_t limit = suggestions.size() < 5 ? suggestions.size() : 5;
            for (auto i {0}; i < limit; ++i) 
            {
                std::cout << "  - " << suggestions[i].first << " (freq: " << suggestions[i].second << ")\n";
            }
        }
        else if (command == "fuzzy") 
        {
            std::string target;
            std::cin >> target;

            std::cout << "Closest matches for '" << target << "':\n";
            int match_count = 0;
            for (auto i {0}; i < vocabulary.size() && match_count < 5; ++i) 
            {
                int dist = EditDistance::levenshtein(target, vocabulary[i]);
                if (dist <= 2) 
                {
                    std::cout << "  - Did you mean '" << vocabulary[i] << "'? (Distance: " << dist << ")\n";
                    match_count++;
                }
            }
        }
        else 
        {
            std::string dummy;
            std::getline(std::cin, dummy);
            std::cout << "Unknown command. Use search, suggest, fuzzy, or exit.\n";
        }
    }

    return 0;
}