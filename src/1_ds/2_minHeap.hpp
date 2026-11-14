#pragma once


#include <cstddef>                                      // for size_t
#include "1_vector.hpp"                                 // for MyVector


/*
    indexing in the vector-based minHeap

    root:          (0)
    left-child:    (2 * idx) + 1
    right-child:   (2 * idx) + 2    
    parent:        (idx - 1) / 2
*/


// a generic minHeap class
template <typename T>
class MinHeap                                           
{
    private:
        MyVector<T> data;                               // a vector to hold the heap data

        // a helper method which correctly inserts an added element into its corr position in the heap
        void siftUp(size_t idx) {
            while (idx) 
            {
                auto parent ((idx - 1) / 2);

                if (data[idx] < data[parent]) 
                {
                    swap(data[idx], data[parent]); 
                    idx = parent;
                } 
                
                else 
                    break;
            }
        }

        // a helper method which correctly rearranges the heap when an item is popped from it
        void siftDown(size_t idx) 
        {
            auto size {data.size()};

            while ((2 * idx + 1) < size) 
            {
                auto left     = ((2 * idx) + 1);
                auto right    = (left + 1);
                auto smallest = (idx);

                if (data[left] < data[smallest]) 
                    smallest = left;
                if ((right < size) && (data[right] < data[smallest])) 
                    smallest = right;

                if (smallest != idx) 
                {
                    swap(data[idx], data[smallest]);
                    idx = smallest;
                } 

                else 
                    break;
            }
        }

    public:
        MinHeap() 
            {}

        // inserts an element into the heap
        inline void push(const T& val) {
            data.push_back(val);

            siftUp(data.size() - 1);                    // sifts the added element into its corr pos in the heap
        }

        // removes the top element from the heap
        inline void pop() {
            if (empty()) return;

            data[0] = data[data.size() - 1];            // moves the last element to the front of the vector/heap (ie to the root)
            data.pop_back();                            // removes the last, redundant element from the vector
            
            if (!empty()) siftDown(0);                  // sifts the new root down the heap to restore the (min)heap property
        }

        // returns the top (& hence the smallest) element of the heap
        inline const T& top() const {
            return data[0];
        }

        // returns the size of the heap
        inline size_t size() const {
            return data.size();
        }

        // returns true if the heap is empty
        inline bool empty() const {
            return data.empty();
        }
};