#pragma once


#include <string>                                       // for string
#include <chrono>                                       // for high-resolution timing
#include "modules/1_fileLoader.hpp"                     // for FileLoader and Document
#include "modules/2_tokenizer.hpp"                      // for Tokenizer
#include "modules/3_documentStore.hpp"                  // for DocumentStore
#include "modules/4_invertedIndex.hpp"                  // for InvertedIndex
#include "modules/5_ranker.hpp"                         // for Ranker and SearchResult
#include "modules/6_editDistance.hpp"                   // for EditDistance
#include "../1_ds/1_vector.hpp"                         // for MyVector
#include "../1_ds/3_trie.hpp"                           // for Trie (Autocomplete)
#include "../1_ds/5_lruCache.hpp"                       // for LRUCache (Query Caching)
using std::string;
namespace chr = std::chrono;


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

    IndexStats(const size_t files, const size_t tokens, const size_t unique, const double seconds) 
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

    SearchResponse(const int id, const string& name, const double s) 
        : docID(id)
        , fileName(name)
        , score(s) 
        {}
};


// the central Lexica search engine orchestrator class (headless backend supporting index, suggest, search, and fuzzy)
class Engine 
{
    private:
        DocumentStore                              documentStore;
        InvertedIndex                              invertedIndex;
        Trie                                       autocompleteTrie;
        LRUCache<string, MyVector<SearchResponse>> queryCache;                           // LRU cache for query results
        size_t                                     uniqueTermCount;
        size_t                                     totalTokenCount;
        bool                                       isInitialized;

    public:
        Engine(size_t cacheCapacity = 101) 
            : queryCache(cacheCapacity)
            , uniqueTermCount(0)
            , totalTokenCount(0)
            , isInitialized(false) 
            {}


        // index: scans directory, tokenizes documents, builds inverted index & trie, and computes stats
        inline IndexStats initialize(const string& dirPath) 
        {
            auto startTime {chr::high_resolution_clock::now()};                     // highResolution: greatest poss accuracy

            auto docs {FileLoader::loadDirectory(dirPath)};
            if (docs.empty())                                                               // no files in the directory to index
                return IndexStats();

            size_t fileCount   {0};
            size_t totalTokens {0};

            MyHashMap<string, bool> vocabulary;                                             // custom hash set to track unique vocabulary across the entire corpus

            for (const auto& doc : docs) 
            {
                documentStore.addDocument(doc);
                ++fileCount;

                auto tokens {Tokenizer::tokenize(doc.content)};
                for (const auto& token : tokens) 
                {
                    ++totalTokens;

                    auto dummy {false};
                    if (!vocabulary.get(token, dummy))                                      // insert the token if it not alr inserted
                    {
                        vocabulary.insert(token, true);
                        autocompleteTrie.insert(token, 1);                                  // populate trie for autocomplete
                    }
                }

                invertedIndex.addDocument(doc.docID, tokens);
            }

            // calculating the duration of the indexing
                auto endTime {chr::high_resolution_clock::now()};
                chr::duration<double> elapsed {endTime - startTime};                    

            uniqueTermCount = vocabulary.size();
            totalTokenCount = totalTokens;
            isInitialized   = true;

            return IndexStats(fileCount, totalTokenCount, uniqueTermCount, elapsed.count());
        }


        // autocomplete: returns prefix-matching suggestions with frequencies from the Trie
        inline MyVector<std::pair<string, int>> suggest(const string& prefix, size_t topK = 5) const 
        {
            MyVector<std::pair<string, int>> results;
            if (!isInitialized || prefix.empty()) 
                return results;

            auto rawSuggestions {autocompleteTrie.suggest(prefix)};

            // Sort suggestions descending by frequency using insertion sort
            for (auto i {1}; i < rawSuggestions.size(); ++i) 
            {
                auto key {rawSuggestions[i]};
                auto j   {i - 1};

                while ((j >= 0) && (rawSuggestions[j].second < key.second)) 
                {
                    rawSuggestions[j + 1] = rawSuggestions[j];
                    --j;
                }

                rawSuggestions[j + 1] = key;
            }

            // Truncate to topK
                auto limit = min(rawSuggestions.size(), topK);
                for (auto i {0}; i < limit; ++i) 
                    results.push_back(rawSuggestions[i]);

            return results;
        }


        // searching with lru cache: evaluates queries via TF-IDF and checks cache hit/miss status
        inline MyVector<SearchResponse> search(const string& queryString, size_t topK, bool& cacheHitOut) 
        {
            MyVector<SearchResponse> responses;

            if (!isInitialized || queryString.empty()) 
            {
                cacheHitOut = false;
                return responses;
            }

            string cacheKey {queryString + "_k" + std::to_string(topK)};                                // constructing the cache key by combining query string and topK limit

            // checking the LRU Cache first
                if (queryCache.get(cacheKey, responses)) 
                {
                    cacheHitOut = true;
                    return responses;
                }

                cacheHitOut = false;

            auto queryTokens {Tokenizer::tokenize(queryString)};
            if (queryTokens.empty()) 
                return responses;

            // ranking via the TF-IDF Ranker
            auto rawResults {Ranker::rankQuery(queryTokens, invertedIndex, documentStore.size(), topK)};

            // Resolve docIDs into filenames and scores
            for (const auto& res : rawResults) 
            {
                Document doc(0, "", "");
                if (documentStore.getDocument(res.docID, doc)) 
                    responses.push_back(SearchResponse(res.docID, doc.fileName, res.score));
            }

            // Store result in LRU Cache for future queries
            queryCache.put(cacheKey, responses);

            return responses;
        }


        // fuzzy search: finds the closest vocabulary term using Levenshtein distance
        inline bool fuzzyMatch(const string& queryTerm, string& bestMatchOut, int& minDistanceOut, const int maxTolerance = 3) const 
        {
            if (!isInitialized || queryTerm.empty()) 
                return false;

            // if exact term exists, distance is 0
            MyVector<Posting> dummyPostings;
            if (invertedIndex.getPostings(queryTerm, dummyPostings)) 
            {
                bestMatchOut   = queryTerm;
                minDistanceOut = 0;

                return true;
            }

            // Otherwise, search across indexed vocabulary using EditDistance
            auto   suggestions  {autocompleteTrie.suggest("")};                                   // gets all words from trie
            auto   bestDistance {9999};
            string bestWord     {""};

            for (const auto& pair : suggestions) 
            {
                const string& vocabWord {pair.first};
                int           dist      {EditDistance::levenshtein(queryTerm, vocabWord)};

                if (dist < bestDistance) 
                {
                    bestDistance = dist;
                    bestWord     = vocabWord;
                }
            }

            if (bestDistance <= maxTolerance) 
            {
                bestMatchOut   = bestWord;
                minDistanceOut = bestDistance;

                return true;
            }

            return false;
        }

        // returns total indexed documents
        inline size_t getDocumentCount() const {
            return documentStore.size();
        }

        // returns total unique terms
        inline size_t getUniqueTermCount() const {
            return uniqueTermCount;
        }
};