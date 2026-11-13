#pragma once

#include <string>
#include <utility>
#include "my_vector.hpp"
using std::pair, std::string;

typedef long long size_t;

class Trie 
{
    private:
        struct TrieNode 
        {
            TrieNode* children[26];
            bool      isEnd;
            int       frequency;

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
                {
                    if (children[i]) 
                        delete children[i];
                }
            }
        };

        TrieNode* root;

        void collectAll(TrieNode* node, std::string current_prefix, MyVector<std::pair<std::string, int>>& results) const 
        {
            if (!node) 
                return;

            if (node->isEnd) 
            {
                results.push_back({current_prefix, node->frequency});
            }

            for (auto i {0}; i < 26; ++i) 
            {
                if (node->children[i]) 
                {
                    char next_char = 'a' + i;
                    collectAll(node->children[i], current_prefix + next_char, results);
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

        inline void insert(const std::string& word, int freq = 1) 
        {
            TrieNode* curr = root;

            for (char c : word) 
            {
                if (c < 'a' || c > 'z') 
                    continue;

                int idx = c - 'a';

                if (!curr->children[idx]) 
                {
                    curr->children[idx] = new TrieNode();
                }

                curr = curr->children[idx];
            }

            curr->isEnd      = true;
            curr->frequency += freq;
        }

        inline MyVector<std::pair<std::string, int>> suggest(const std::string& prefix) const 
        {
            TrieNode* curr = root;
            MyVector<pair<string, int>> results;

            for (char c : prefix) 
            {
                if (c < 'a' || c > 'z') 

                    return results;
                int idx = c - 'a';
                if (!curr->children[idx]) 
                {
                    return results;
                }

                curr = curr->children[idx];
            }
            
            collectAll(curr, prefix, results);
            return results;
        }
};