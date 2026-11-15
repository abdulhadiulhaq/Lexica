#pragma once


#include <string>                                       // for string
#include <cstddef>                                      // for size_t
using std::string;


// a generic least-recently-used (LRU) cache implementation combining a doubly-linked list & hashMap
template <typename K, typename V>
class LRUCache 
{
    private:
        // a doubly-linked list node holding the actual cached key-value pair
        struct Node {
            K     key;
            V     value;
            Node* prev;
            Node* next;

            Node(const K& k, const V& val) 
                : key(k)
                , value(val)
                , prev(nullptr)
                , next(nullptr) 
                {}
        };

        // a hashMap bucket node mapping a key directly to its position in the doubly-linked list
        struct HashNode {
            K         key;
            Node*     cacheNode;
            HashNode* next;

            HashNode(const K& k, Node* node) 
                : key(k)
                , cacheNode(node)
                , next(nullptr) 
                {}
        };

        HashNode** buckets;
        size_t     capacity;
        size_t     cacheSize;
        Node*      head;                                // most-recently-used (MRU) item
        Node*      tail;                                // least-recently-used (LRU) item

        // returns the hash of the input string
        unsigned long hashFunction(const string& str) const {
            unsigned long hash {5381};                  // the starting seed (used in DJB2 hash algorithm)
            
            for (auto ch : str) 
                hash = (((hash << 5) + hash) + ch);     // hash << 5: bitwise left-shift by 5 ie multiplying by 32

            return hash;
        }

        // returns the hash of the input number as is
        unsigned long hashFunction(const int key) const {
            return static_cast<unsigned long>(key);
        }

        // detaches a node from its curr position & moves it to the front (MRU) of the list
        void moveToHead(Node* node) 
        {
            if (head == node) return;

            // detaching node from its current neighbors
                if (node->prev)   node->prev->next = node->next;
                if (node->next)   node->next->prev = node->prev;
                if (tail == node) tail             = node->prev;

            // placing node at the front (head)
                node->prev = nullptr;
                node->next = head;

            if (head) head->prev = node;
            head = node;

            if (!tail) tail = head;
        }

        // evicts the least-recently-used item (at the tail) from both the linked list & hashMap
        void removeTail() 
        {
            if (!tail) return;

            auto oldTail {tail};
            
            // removing the corresponding HashNode from the hashMap
                auto idx  {hashFunction(oldTail->key) % capacity};
                auto curr {buckets[idx]};
                HashNode* prev = nullptr;

            while (curr) 
            {
                if (curr->key == oldTail->key) {
                    if (!prev) 
                        buckets[idx] = curr->next;
                    else 
                        prev->next = curr->next;

                    delete curr;
                    break;
                }

                prev = curr;
                curr = curr->next;
            }

            // updating tail pointer in the doubly-linked list
            tail = tail->prev;
            
            if   (tail) tail->next = nullptr;
            else        head       = nullptr;            // head = tail = nullptr

            delete oldTail;
            --cacheSize;
        }

    public:
        LRUCache(size_t cap = 101)                      // arbitrary default cache capacity of 101 items
            : capacity(cap)
            , cacheSize(0)
            , head(nullptr)
            , tail(nullptr) 
            {
                buckets = new HashNode*[capacity]();    // makes & zero-initializes the array to nullptr
            }

        ~LRUCache() 
            {
                // freeing all doubly-linked list nodes
                    auto curr {head};

                    while (curr) {
                        auto next {curr->next};
                        delete curr;
                        curr = next;
                    }

                // freeing all hashMap bucket chains
                    for (auto i {0}; i < capacity; ++i) {
                        auto bucket {buckets[i]};

                        while (bucket) {
                            auto nextbucket {bucket->next};
                            delete bucket;
                            bucket = nextbucket;
                        }
                    }

                delete[] buckets;
            }

        // retrieves a cached value by key & marks it as most-recently-used
        inline bool get(const K& key, V& keyVal) 
        {
            auto idx  {hashFunction(key) % capacity};
            auto curr {buckets[idx]};

            while (curr) 
            {
                if (curr->key == key) {
                    moveToHead(curr->cacheNode);        // promote to front (MRU)
                    keyVal = curr->cacheNode->value;

                    return true;
                }

                curr = curr->next;
            }

            return false;
        }

        // inserts or updates a key-value pair in the cache, evicting the LRU item if full
        inline void put(const K& key, const V& value) 
        {
            auto idx  {hashFunction(key) % capacity};
            auto curr {buckets[idx]};

            // if key already exists, update its value & promote to front
            while (curr) 
            {
                if (curr->key == key) {
                    curr->cacheNode->value = value;
                    moveToHead(curr->cacheNode);

                    return;
                }

                curr = curr->next;
            }

            // evict least-recently-used item if cache is at max capacity
            if (cacheSize >= capacity) 
                removeTail();

            // create & prepend new node to the front (MRU) of the doubly-linked list
                auto newNode = new Node(key, value);
                newNode->next = head;
                
                if (head) head->prev = newNode;
                head = newNode;
                
                if (!tail) tail = head;

            // prepend corresponding HashNode to the hashMap bucket
                auto newbucket  = new HashNode(key, newNode);
                newbucket->next = buckets[idx];
                buckets[idx]    = newbucket;
            
            ++cacheSize;
        }

        // returns the current number of cached items
        inline size_t size() const {
            return cacheSize;
        }

        // returns true if the cache is empty
        inline bool empty() const {
            return (cacheSize == 0);
        }
};