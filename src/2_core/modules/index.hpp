#pragma once

#include <string>
#include "../include/dataStructures/my_vector.hpp"
#include "../include/dataStructures/my_hashMap.hpp"

typedef long long size_t;

struct Posting 
{
    int    doc_id;
    int    term_freq;
    double tf_idf_score;

    Posting() 
        : doc_id(0)
        , term_freq(0)
        , tf_idf_score(0.0) 
        {}

    Posting(int id, int freq) 
        : doc_id(id)
        , term_freq(freq)
        , tf_idf_score(0.0) 
        {}

    bool operator<(const Posting& other) const 
    {
        return tf_idf_score < other.tf_idf_score;
    }
};

class InvertedIndex 
{
    private:
        MyHashMap<std::string, MyVector<Posting>> index;
        int total_documents;

    public:
        InvertedIndex() 
            : total_documents(0) 
            {}

        inline void addDocument(int doc_id, const MyVector<std::string>& tokens) 
        {
            total_documents++;

            MyHashMap<std::string, bool> processed;
            for (auto i {0}; i < tokens.size(); ++i) 
            {
                std::string term = tokens[i];
                bool dummy = false;
                if (!processed.get(term, dummy)) 
                {
                    processed.insert(term, true);
                    
                    // Count frequency of term in token list
                    int freq = 0;
                    for (auto j {0}; j < tokens.size(); ++j) 
                    {
                        if (tokens[j] == term) freq++;
                    }

                    Posting posting(doc_id, freq);
                    
                    MyVector<Posting> postings_list;
                    if (index.get(term, postings_list)) 
                    {
                        postings_list.push_back(posting);
                        index.insert(term, postings_list);
                    } 
                    else 
                    {
                        MyVector<Posting> new_list;
                        new_list.push_back(posting);
                        index.insert(term, new_list);
                    }
                }
            }
        }

        inline bool getPostings(const std::string& term, MyVector<Posting>& out_postings) const 
        {
            return index.get(term, out_postings);
        }

        inline int getTotalDocuments() const 
        {
            return total_documents;
        }
};