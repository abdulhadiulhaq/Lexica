#pragma once

#include "my_vector.hpp"

typedef long long size_t;

template <typename T>
class MinHeap 
{
    private:
        MyVector<T> data;

        void siftUp(size_t idx) 
        {
            while (idx > 0) 
            {
                size_t parent = (idx - 1) / 2;

                if (data[idx] < data[parent]) 
                {
                    T temp = data[idx];
                    data[idx] = data[parent];
                    data[parent] = temp;
                    idx = parent;
                } 
                
                else 
                {
                    break;
                }
            }
        }

        void siftDown(size_t idx) 
        {
            size_t sz = data.size();

            while ((2 * idx + 1) < sz) 
            {
                size_t left = 2 * idx + 1;
                size_t right = left + 1;
                size_t smallest = idx;

                if (data[left] < data[smallest]) 
                    smallest = left;
                if (right < sz && data[right] < data[smallest]) 
                    smallest = right;

                if (smallest != idx) 
                {
                    T temp = data[idx];
                    data[idx] = data[smallest];
                    data[smallest] = temp;
                    idx = smallest;
                } 
                else 
                {
                    break;
                }
            }
        }

    public:
        MinHeap() 
            {}

        inline void push(const T& val) 
        {
            data.push_back(val);
            siftUp(data.size() - 1);
        }

        inline const T& top() const 
        {
            return data[0];
        }

        inline size_t size() const 
        {
            return data.size();
        }

        inline bool empty() const 
        {
            return data.empty();
        }
};