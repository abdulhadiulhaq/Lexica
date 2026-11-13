#pragma once

#include <string>
using std::string;

typedef long long size_t;

template <typename K, typename V>
class MyHashMap 
{
    private:
        struct HashNode 
        {
            K         key;
            V         value;
            HashNode* next;

            HashNode(const K& k, const V& v) 
                : key(k)
                , value(v)
                , next(nullptr) 
                {}
        };

        HashNode** buckets;
        size_t     capacity;
        size_t     size;

        unsigned long hashFunction(const string& str) const 
        {
            unsigned long hash = 5381;
            
            for (char c : str) 
            {
                hash = ((hash << 5) + hash) + c;
            }

            return hash;
        }

        unsigned long hashFunction(int key) const 
        {
            return static_cast<unsigned long>(key);
        }

        void rehash() 
        {
            size_t old_cap = capacity;
            capacity *= 2;
            HashNode** new_buckets = new HashNode*[capacity]();

            for (auto i {0}; i < old_cap; ++i) 
            {
                HashNode* curr = buckets[i];

                while (curr != nullptr) 
                {
                    HashNode* next_node = curr->next;
                    unsigned long idx = hashFunction(curr->key) % capacity;
                    curr->next = new_buckets[idx];
                    new_buckets[idx] = curr;
                    curr = next_node;
                }
            }

            delete[] buckets;
            buckets = new_buckets;
        }

    public:
        MyHashMap(size_t cap = 101) 
            : capacity(cap)
            , size(0) 
            {
                buckets = new HashNode*[capacity]();

                for (auto i {0}; i < capacity; ++i)
                    buckets[i] = nullptr;
            }

        ~MyHashMap() 
        {
            clear();
            delete[] buckets;
        }

        inline void insert(const K& key, const V& value) 
        {
            if ((size * 10) / capacity > 7) 
            {
                rehash();
            }

            unsigned long idx = hashFunction(key) % capacity;
            HashNode* curr = buckets[idx];

            while (curr != nullptr) 
            {
                if (curr->key == key) 
                {
                    curr->value = value;
                    return;
                }
                curr = curr->next;
            }

            HashNode* new_node = new HashNode(key, value);
            new_node->next = buckets[idx];
            buckets[idx] = new_node;
            size++;
        }

        inline bool get(const K& key, V& out_val) const 
        {
            unsigned long idx = hashFunction(key) % capacity;
            HashNode* curr = buckets[idx];

            while (curr != nullptr) 
            {
                if (curr->key == key) 
                {
                    out_val = curr->value;
                    return true;
                }

                curr = curr->next;
            }

            return false;
        }

        inline bool contains(const K& key) const 
        {
            V dummy;
            return get(key, dummy);
        }

        inline void clear() 
        {
            for (auto i {0}; i < capacity; ++i) 
            {
                HashNode* curr = buckets[i];

                while (curr != nullptr) 
                {
                    HashNode* next = curr->next;
                    delete curr;
                    curr = next;
                }

                buckets[i] = nullptr;
            }
            
            size = 0;
        }

        inline size_t get_size() const 
        {
            return size;
        }
};