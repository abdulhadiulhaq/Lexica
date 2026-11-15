#pragma once


#include <string>                                       // for string
#include <chrono>                                       // for high-resolution timing
#include "1_fileLoader.hpp"                             // for FileLoader and Document
#include "2_tokenizer.hpp"                              // for Tokenizer
#include "3_documentStore.hpp"                          // for DocumentStore
#include "4_invertedIndex.hpp"                          // for InvertedIndex
#include "5_ranker.hpp"                                 // for Ranker and SearchResult
#include "6_editDistance.hpp"                           // for EditDistance
#include "../1_ds/1_vector.hpp"                         // for MyVector
#include "../1_ds/3_trie.hpp"                           // for Trie (Autocomplete)
#include "../1_ds/5_lruCache.hpp"                       // for LRUCache (Query Caching)
using std::string;


// struct representing indexing statistics
struct IndexStats 
{
    size_t fileCount;
    size_t totalTokens;
    size_t uniqueTerms;
    double elapsedSeconds;

    IndexStats() 
        : fileCount(0)
        , totalTokens(0)
        , uniqueTerms(0)
        , elapsedSeconds(0.0) 
        {}

    IndexStats(size_t files, size_t tokens, size_t unique, double seconds) 
        : fileCount(files)
        , totalTokens(tokens)
        , uniqueTerms(unique)
        , elapsedSeconds(seconds) 
        {}
};


// struct representing a fully resolved search response containing document metadata and score
struct SearchResponse 
{
    int    docID;
    string fileName;
    double score;

    SearchResponse() 
        : docID(0)
        , fileName("")
        , score(0.0) 
        {}

    SearchResponse(int id, const string& name, double s) 
        : docID(id)
        , fileName(name)
        , score(s) 
        {}
};


// the central Lexica search engine orchestrator class (headless backend supporting index, suggest, search, and fuzzy)
class Engine 
{
    private:
        DocumentStore               documentStore;
        InvertedIndex               invertedIndex;
        Trie                        autocompleteTrie;
        mutable LRUCache<string, MyVector<SearchResponse>> queryCache; // LRU cache for query results
        size_t                      uniqueTermCount;
        size_t                      totalTokenCount;
        bool                        isInitialized;

    public:
        Engine(size_t cacheCapacity = 100) 
            : queryCache(cacheCapacity)
            , uniqueTermCount(0)
            , totalTokenCount(0)
            , isInitialized(false) 
            {}

        // 1. INDEXING: scans directory, tokenizes documents, builds inverted index & trie, and computes stats
        inline IndexStats initialize(const string& dirPath) 
        {
            auto startTime {std::chrono::high_resolution_clock::now()};

            auto docs {FileLoader::loadDirectory(dirPath)};
            if (docs.empty()) 
            {
                return IndexStats();
            }

            size_t fileCount {0};
            size_t totalTokens {0};

            // Custom hash set to track unique vocabulary across the entire corpus
            MyHashMap<string, bool> vocabulary;

            for (const auto& doc : docs) 
            {
                documentStore.addDocument(doc);
                ++fileCount;

                auto tokens {Tokenizer::tokenize(doc.content)};
                for (const auto& token : tokens) 
                {
                    ++totalTokens;
                    auto dummy {false};
                    if (!vocabulary.get(token, dummy)) 
                    {
                        vocabulary.insert(token, true);
                        autocompleteTrie.insert(token, 1); // populate trie for autocomplete
                    }
                }

                invertedIndex.addDocument(doc.docID, tokens);
            }

            auto endTime {std::chrono::high_resolution_clock::now()};
            std::chrono::duration<double> elapsed {endTime - startTime};

            uniqueTermCount = vocabulary.get_size();
            totalTokenCount = totalTokens;
            isInitialized = true;

            return IndexStats(fileCount, totalTokenCount, uniqueTermCount, elapsed.count());
        }

        // 2. AUTOCOMPLETE: returns prefix-matching suggestions with frequencies from the Trie
        inline MyVector<std::pair<string, int>> suggest(const string& prefix, size_t topK = 5) const 
        {
            MyVector<std::pair<string, int>> results;
            if (!isInitialized || prefix.empty()) 
            {
                return results;
            }

            auto rawSuggestions {autocompleteTrie.suggest(prefix)};

            // Sort suggestions descending by frequency using insertion sort
            for (auto i {1}; i < rawSuggestions.size(); ++i) 
            {
                auto key {rawSuggestions[i]};
                long long j = static_cast<long long>(i) - 1;

                while (j >= 0 && rawSuggestions[j].second < key.second) 
                {
                    rawSuggestions[j + 1] = rawSuggestions[j];
                    --j;
                }
                rawSuggestions[j + 1] = key;
            }

            // Truncate to topK
            auto limit {(rawSuggestions.size() < topK) ? rawSuggestions.size() : topK};
            for (auto i {0}; i < limit; ++i) 
            {
                results.push_back(rawSuggestions[i]);
            }

            return results;
        }

        // 3. RANKED SEARCH WITH LRU CACHING: evaluates queries via TF-IDF and checks cache hit/miss status
        inline MyVector<SearchResponse> search(const string& queryString, size_t topK, bool& cacheHitOut) const 
        {
            MyVector<SearchResponse> responses;
            if (!isInitialized || queryString.empty()) 
            {
                cacheHitOut = false;
                return responses;
            }

            // Construct cache key combining query string and topK limit
            string cacheKey {queryString + "_k" + std::to_string(topK)};

            // Check LRU Cache first
            if (queryCache.get(cacheKey, responses)) 
            {
                cacheHitOut = true;
                return responses;
            }

            cacheHitOut = false;
            auto queryTokens {Tokenizer::tokenize(queryString)};
            if (queryTokens.empty()) 
            {
                return responses;
            }

            // Rank via TF-IDF Ranker
            auto rawResults {Ranker::rankQuery(queryTokens, invertedIndex, documentStore.size(), topK)};

            // Resolve docIDs into filenames and scores
            for (const auto& res : rawResults) 
            {
                Document doc(0, "", "");
                if (documentStore.getDocument(res.docID, doc)) 
                {
                    responses.push_back(SearchResponse(res.docID, doc.fileName, res.score));
                }
            }

            // Store result in LRU Cache for future queries
            queryCache.put(cacheKey, responses);

            return responses;
        }

        // 4. FUZZY SEARCH / SPELL CORRECTION: finds the closest vocabulary term using Levenshtein distance
        inline bool fuzzyMatch(const string& queryTerm, string& bestMatchOut, int& minDistanceOut, int maxTolerance = 2) const 
        {
            if (!isInitialized || queryTerm.empty()) 
            {
                return false;
            }

            // If exact term exists, distance is 0
            MyVector<Posting> dummyPostings;
            if (invertedIndex.getPostings(queryTerm, dummyPostings)) 
            {
                bestMatchOut = queryTerm;
                minDistanceOut = 0;
                return true;
            }

            // Otherwise, search across indexed vocabulary using EditDistance
            // (Note: In a full implementation, this iterates over unique vocabulary or utilizes a BK-tree)
            auto suggestions {autocompleteTrie.suggest("")}; // gets all words from trie
            int bestDistance {9999};
            string bestWord {""};

            for (const auto& pair : suggestions) 
            {
                const string& vocabWord {pair.first};
                int dist {EditDistance::levenshtein(queryTerm, vocabWord)};

                if (dist < bestDistance) 
                {
                    bestDistance = dist;
                    bestWord = vocabWord;
                }
            }

            if (bestDistance <= maxTolerance) 
            {
                bestMatchOut = bestWord;
                minDistanceOut = bestDistance;
                return true;
            }

            return false;
        }

        // Returns total indexed documents
        inline size_t getDocumentCount() const 
        {
            return documentStore.size();
        }

        // Returns total unique terms
        inline size_t getUniqueTermCount() const 
        {
            return uniqueTermCount;
        }
};