#pragma once


#include <string>                                       // for string
#include <cmath>                                        // for log
#include "../../1_ds/1_vector.hpp"                      // for MyVector
#include "4_invertedIndex.hpp"                          // for InvertedIndex and Posting
using std::string;


// a struct representing a search result containing docID and final ranking score
struct SearchResult 
{
    int    docID;
    double score;

    SearchResult() 
        : docID(0)
        , score(0.0) 
        {}

    SearchResult(int id, double s) 
        : docID(id)
        , score(s) 
        {}

    inline bool operator<(const SearchResult& other) const {
        return (score < other.score);
    }
};


// a ranker class to compute TF-IDF scores and rank documents for a given query
class Ranker 
{
    public:
        Ranker() 
            {}

        // ranks documents against query tokens using TF-IDF and returns the top-k results
        inline static MyVector<SearchResult> rankQuery(const MyVector<string>& queryTokens, const InvertedIndex& index, size_t totalDocs, size_t topK = 5) 
        {
            MyVector<SearchResult> results;

            if (!totalDocs|| queryTokens.empty())                   // returns an empty results vector if totalDocs == 0 or the query is empty
                return results;

            // allocate score accumulator array for all document IDs (1-indexed)
            auto scores {new double[totalDocs + 1]()};

            for (auto i {0}; i < queryTokens.size(); ++i) 
            {
                string term = queryTokens[i];
                MyVector<Posting> postings;

                if (index.getPostings(term, postings)) 
                {
                    auto df {postings.size()};
                    if (df == 0) continue;

                    // IDF formula: ln(1 + (Total Docs / Document Frequency))
                    auto idf {log(1.0 + (totalDocs / static_cast<double>(df)))};

                    for (auto j {0}; j < postings.size(); ++j) 
                    {
                        const auto& p {postings[j]};

                        if ((p.docID >= 1) && (p.docID <= totalDocs)) 
                        {
                            auto tf {static_cast<double>(p.freq)};                  // getting tf: term frequency
                            scores[p.docID] += (tf * idf);                          // calculating & storing tf * idf
                        }
                    }
                }
            }

            // collect all documents with a score > 0.0
            for (auto id {1}; id <= totalDocs; ++id) {
                if (scores[id] > 0.0) 
                    results.push_back(SearchResult(id, scores[id]));
            }

            delete[] scores;

            // sort results descending by score using insertion sort
                for (auto i {1}; i < results.size(); ++i) 
                {
                    auto key {results[i]};
                    auto j   {i - 1};

                    while ((j >= 0) && (results[j].score < key.score)) 
                    {
                        results[j + 1] = results[j];
                        --j;
                    }

                    results[j + 1] = key;
                }

            // truncate to topK
                MyVector<SearchResult> topResults;
                auto limit {std::min(results.size(), topK)};                // getting the lesser of the 2

                for (auto i {0}; i < limit; ++i) 
                    topResults.push_back(results[i]);

            return topResults;
        }
};