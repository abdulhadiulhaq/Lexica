#pragma once

#include <string>

typedef long long size_t;

template <typename K, typename V>
class LRUCache 
{
    private:
        struct Node 
        {
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

        struct HashNode 
        {
            K         key;
            Node*     cache_node;
            HashNode* next;

            HashNode(const K& k, Node* node) 
                : key(k)
                , cache_node(node)
                , next(nullptr) 
                {}
        };

        HashNode** buckets;
        size_t     capacity;
        size_t     size;
        Node*      head;
        Node*      tail;

        unsigned long hashFunction(const std::string& str) const 
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

        void moveToHead(Node* node) 
        {
            if (head == node) 
                return;
            
            if (node->prev) 
                node->prev->next = node->next;
            if (node->next) 
                node->next->prev = node->prev;
            if (tail == node) 
                tail = node->prev;
            
            node->prev = nullptr;
            node->next = head;
            if (head) 
                head->prev = node;
            head = node;
            if (!tail) 
                tail = head;
        }

        void removeTail() 
        {
            if (!tail) 
                return;
            Node* old_tail = tail;
            
            unsigned long idx = hashFunction(old_tail->key) % capacity;
            HashNode* curr = buckets[idx];
            HashNode* prev = nullptr;
            while (curr) 
            {
                if (curr->key == old_tail->key) 
                {
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

            tail = tail->prev;
            if (tail) 
                tail->next = nullptr;
            else 
                head = nullptr;

            delete old_tail;
            size--;
        }

    public:
        LRUCache(size_t cap = 100) 
            : capacity(cap)
            , size(0)
            , head(nullptr)
            , tail(nullptr) 
        {
            buckets = new HashNode*[capacity]();
            for (auto i {0}; i < capacity; ++i) 
            {
                buckets[i] = nullptr;
            }
        }

        ~LRUCache() 
        {
            Node* curr = head;
            while (curr) 
            {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
            for (auto i {0}; i < capacity; ++i) 
            {
                HashNode* hn = buckets[i];
                while (hn) 
                {
                    HashNode* next_hn = hn->next;
                    delete hn;
                    hn = next_hn;
                }
            }
            delete[] buckets;
        }

        inline bool get(const K& key, V& out_val) 
        {
            unsigned long idx = hashFunction(key) % capacity;
            HashNode* curr = buckets[idx];
            while (curr) 
            {
                if (curr->key == key) 
                {
                    moveToHead(curr->cache_node);
                    out_val = curr->cache_node->value;
                    return true;
                }
                curr = curr->next;
            }
            return false;
        }

        inline void put(const K& key, const V& value) 
        {
            unsigned long idx = hashFunction(key) % capacity;
            HashNode* curr = buckets[idx];
            while (curr) 
            {
                if (curr->key == key) 
                {
                    curr->cache_node->value = value;
                    moveToHead(curr->cache_node);
                    return;
                }
                curr = curr->next;
            }

            if (size >= capacity) 
            {
                removeTail();
            }

            Node* new_node = new Node(key, value);
            new_node->next = head;
            if (head) 
                head->prev = new_node;
            head = new_node;
            if (!tail) 
                tail = head;

            HashNode* new_hn = new HashNode(key, new_node);
            new_hn->next = buckets[idx];
            buckets[idx] = new_hn;
            size++;
        }
};