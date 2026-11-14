#pragma once


#include <string>                                       // for string
#include <cstddef>                                      // for size_t
#include "../../1_ds/1_vector.hpp"                      // for MyVector
#include "../../1_ds/4_hashMap.hpp"                     // for MyHashMap
using std::string;


// a struct representing a document posting containing docID, token frequency, and score
struct Posting 
{
    int    docID;                                   // id of the doc
    int    freq;                                    // the num of times that word comes in the doc
    double tf_idf_score;                            // the tfIDF score of that word in the doc

    Posting() 
        : docID(0)
        , freq(0)
        , tf_idf_score(0.0) 
        {}

    Posting(const int id, const int freq) 
        : docID(id)
        , freq(freq)
        , tf_idf_score(0.0) 
        {}

    // an overloaded < operator to compare 2 postings based on their tf_idf scores
    inline bool operator<(const Posting& other) const {
        return tf_idf_score < other.tf_idf_score;
    }
};


// an inverted index class mapping terms to posting lists using a custom hashMap
class InvertedIndex 
{
    private:
        MyHashMap<string, MyVector<Posting>> index;
        size_t                               totalDocuments;

    public:
        InvertedIndex() 
            : totalDocuments(0) 
            {}

        // adds a document's tokens to the inverted index
        inline void addDocument(const int docID, const MyVector<string>& tokens) 
        {
            ++totalDocuments;

            MyHashMap<string, bool> processed;                          // ie effectively a hashSet

            for (const auto& token : tokens)
            {
                auto dummy {false};

                if (!processed.get(token, dummy)) 
                {
                    processed.insert(token, true);                          // inserting the token into the hashSet if it no existed

                    // count the frequency of the token in the token list
                    auto freq {0};
                    for (const auto& t : tokens)
                        if (t == token) ++freq;

                    Posting posting(docID, freq);
                    MyVector<Posting> postingsList;

                    if (index.get(token, postingsList)) {                       // if the token is alr in the index, updates its postingsList
                        postingsList.push_back(posting);                        // updating the postingsList
                        index.insert(token, postingsList);                      // reinserting the updated list into the hashMap/index
                    } 
                    else {                                                      // the token and its postingsList wasnt in the index
                        MyVector<Posting> newList;
                        newList.push_back(posting);
                        index.insert(token, newList);                           // inserts newly-formed list into the index for the given token
                    }
                }
            }
        }

        // retrieves the posting list for a given search token, returning true if found
        inline bool getPostings(const string& token, MyVector<Posting>& outPostings) const {
            return index.get(token, outPostings);
        }

        // returns the total number of docs indexed
        inline size_t getTotalDocuments() const {
            return totalDocuments;
        }
};