#pragma once

#include <string>
#include <cmath>
#include "my_vector.hpp"
#include "index.hpp"

struct SearchResult 
{
    int    doc_id;
    double score;

    SearchResult() 
        : doc_id(0)
        , score(0.0) 
        {}

    SearchResult(int id, double s) 
        : doc_id(id)
        , score(s) 
        {}

    bool operator<(const SearchResult& other) const 
    {
        return score < other.score;
    }
};

class Ranker 
{
    public:
        static inline MyVector<SearchResult> rankQuery(
            const MyVector<std::string>& query_tokens,
            const InvertedIndex& index,
            int total_docs,
            int top_k = 5) 
        {
            MyVector<SearchResult> results;
            if (total_docs == 0 || query_tokens.empty()) 
            {
                return results;
            }

            // Allocate score accumulator array for all document IDs (1-indexed)
            double* scores = new double[total_docs + 1]();

            for (auto i {0}; i < query_tokens.size(); ++i) 
            {
                std::string term = query_tokens[i];
                MyVector<Posting> postings;
                if (index.getPostings(term, postings)) 
                {
                    int df = static_cast<int>(postings.size());
                    if (df == 0) continue;

                    // IDF formula: ln(1 + (Total Docs / Document Frequency))
                    double idf = std::log(1.0 + (static_cast<double>(total_docs) / static_cast<double>(df)));

                    for (auto j {0}; j < postings.size(); ++j) 
                    {
                        const Posting& p = postings[j];
                        if (p.doc_id >= 1 && p.doc_id <= total_docs) 
                        {
                            double tf = static_cast<double>(p.term_freq);
                            scores[p.doc_id] += (tf * idf);
                        }
                    }
                }
            }

            // Collect all documents with a score > 0
            for (int id = 1; id <= total_docs; ++id) 
            {
                if (scores[id] > 0.0) 
                {
                    results.push_back(SearchResult(id, scores[id]));
                }
            }
            delete[] scores;

            // Sort results descending by score (Simple Insertion Sort)
            for (auto i {1}; i < results.size(); ++i) 
            {
                SearchResult key = results[i];
                long long j = i - 1;
                while (j >= 0 && results[j].score < key.score) 
                {
                    results[j + 1] = results[j];
                    j--;
                }
                results[j + 1] = key;
            }

            // Truncate to top_k
            MyVector<SearchResult> top_results;
            size_t limit = (results.size() < static_cast<size_t>(top_k)) ? results.size() : static_cast<size_t>(top_k);
            for (auto i {0}; i < limit; ++i) 
            {
                top_results.push_back(results[i]);
            }

            return top_results;
        }
};