#pragma once


#include <string>                                   // for string
#include <cstddef>                                  // for size_t
#include <utility>                                  // for pair
#include "1_vector.hpp"                             // for MyVector
using std::pair, std::string;


// a node in the Trie class
struct TrieNode 
{
    TrieNode* children[26];                         // for the 26 alphabets
    bool      isEnd;                                // true if a valid word ends at this exact letter
    int       frequency;                            // the total num of occurances of the current word/prefix

    TrieNode() 
        : isEnd(false)
        , frequency(0) 
        {
            for (auto i {0}; i < 26; ++i) 
                children[i] = nullptr;
        }

    ~TrieNode() 
        {
            for (auto i {0}; i < 26; ++i) 
                if (children[i]) delete children[i];
        }
};

class Trie 
{
    private:
        TrieNode* root;

        // a recursive helper-method which returns all the complete words in the trie starting at currPrefix
        void collectAll(TrieNode* node, string currPrefix, MyVector<pair<string, int>>& results) const {
            if (!node) return;

            if (node->isEnd) 
                results.push_back({currPrefix, node->frequency});

            for (auto i {0}; i < 26; ++i) {
                if (node->children[i]) 
                {
                    char next = ('a' + i);
                    collectAll(node->children[i], currPrefix + next, results);
                }
            }
        }

    public:
        Trie() 
            : root(new TrieNode()) 
            {}

        ~Trie() 
            {
                delete root;
            }

        // adds a word into the trie
        inline void insert(const string& word, int freq = 1) {
            auto curr = root;

            for (auto ch : word) 
            {
                if (!isalpha(ch)) continue;                                 // continue if ch is not a alphabet 

                auto idx {ch - 'a'};

                if (!curr->children[idx]) 
                    curr->children[idx] = new TrieNode();                   // inserts a newNode for that ch if it no exists

                curr = curr->children[idx];
            }

            curr->isEnd      = true;                                        // a comp word ending at this node is formed
            curr->frequency += freq;                                        // increment its freq
        }

        // returns a vector list of all the words matching/starting-with the given prefix
        inline MyVector<pair<string, int>> suggest(const string& prefix) const {
            auto curr = root;
            MyVector<pair<string, int>> results;

            for (auto ch : prefix)                      // tries to traverses the trie until the node with the last char of the prefix is reached
            {
                if (!isalpha(ch)) 
                    return results;                     // return an empty vector if ch is not an alphabet

                auto idx {ch - 'a'};
                
                if (!curr->children[idx]) 
                    return results;                     // return an empty vector if ch is not in the trie(node)

                curr = curr->children[idx];             // current ch was in the trie :D
            }

            collectAll(curr, prefix, results);

            return results;
        }
};