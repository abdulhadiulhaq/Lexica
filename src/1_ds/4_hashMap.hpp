#pragma once


#include <string>                                       // for string
#include <cstddef>                                      // for size_t
using std::string;


// a generic hashMap implementation with chaining (open-hashing)
template <typename K, typename V>
class MyHashMap 
{
    // a node in the hashMap
    struct HashNode {
            K         key;
            V         value;
            HashNode* next;
            
            HashNode(const K& k, const V& v) 
            : key(k)
            , value(v)
            , next(nullptr) 
            {}
    };

    private:
        HashNode** buckets;
        size_t     capacity;
        size_t     size;

        // returns the hash of the input string
        unsigned long hashFunction(const string& str) const {
            unsigned long hash {5381};                              // the starting seed (used in DJB2 hash algorithm)
            
            for (auto ch : str)                     
                hash = (((hash << 5) + hash) + ch);                 // hash << 5: bitwise left-shift by 5 ie multiplying by 32

            return hash;
        }

        // returns the hash of the input number as is
        unsigned long hashFunction(const int key) const {
            return static_cast<unsigned long>(key);
        }

        // expands/increases the hashMaps capacity when it starts getting too crowded/full
        void rehash() 
        {
            auto origCapacity {capacity};
            capacity *= 2;                                                         // doubling the capacity

            HashNode** newBuckets = new HashNode*[capacity]();                     // creating a bigger bucket array with double the original capacity; () at the end means zero-initialize everything inside (ie in-this-case set all the nodes to nullptr)

            for (auto i {0}; i < origCapacity; ++i) {
                auto curr {buckets[i]};

                while (curr) 
                {
                    auto nextNode = curr->next;

                    auto idx {hashFunction(curr->key) % capacity};                 // recalculating the new index using the new capacity

                    // moving the node into the new bucket chain
                    curr->next = newBuckets[idx];                                  // setting the next of curr (curr->next) to the curr data at idx in the map
                    newBuckets[idx] = curr;                                        // setting element at idx to curr

                    curr = nextNode;
                }
            }

            delete[] buckets;
            buckets = newBuckets;
        }

    public:
        MyHashMap(size_t cap = 101)                                                 // arbitrary default capacity
            : capacity(cap)
            , size(0) 
            {
                buckets = new HashNode*[capacity]();                                // makes & zero-initializes the array

                // for (auto i {0}; i < capacity; ++i)                              // redundant cuz of the '()' at the end of line 80
                //     buckets[i] = nullptr;
            }

        ~MyHashMap() 
            {
                clear();
                delete[] buckets;
            }

        // inserts a key-value pair into the hashMap
        inline void insert(const K& key, const V& value) 
        {
            if (((size / static_cast<double>(capacity)) * 100) > 70)                         // rehashing if the array is > 70pc full
                rehash();

            auto idx  {hashFunction(key) % capacity};                               // computing the bucket index for the key
            auto curr {buckets[idx]};

            // searching the bucket's chain to see if the key already exists
            while (curr) 
            {
                if (curr->key == key) {
                    curr->value = value;                                            // updating the existing value
                    return;
                }

                curr = curr->next;
            }

            // if its a new key, create a new-node and prepend it to the chain
                auto newNode = new HashNode(key, value);
                newNode->next = buckets[idx];
                buckets[idx] = newNode;
                
                ++size;
        }

        // used to get the value of a key in the map
        inline bool get(const K& key, V& keyVal) const {
            auto idx  {hashFunction(key) % capacity};
            auto curr {buckets[idx]};

            while (curr)                                                // iterating through every node at buckets[idx]
            {
                if (curr->key == key) {                                 // if the key was found at idx:
                    keyVal = curr->value;                               //                             - set keyVal to curr's value
                    return true;                                        //                             - return true
                }

                curr = curr->next;
            }

            return false;
        }

        // returns whether the map contains a key
        inline bool contains(const K& key) const {
            V dummy;

            return get(key, dummy);
        }

        // empties the hashMap
        inline void clear() {
            for (auto i {0}; i < capacity; ++i) 
            {
                auto curr {buckets[i]};

                while (curr) {
                    auto next {curr->next};

                    delete curr;
                    curr = next;
                }

                buckets[i] = nullptr;
            }
            
            size = 0;
        }

        // returns the size of the map
        inline size_t size() const {
            return size;
        }
};