#pragma once


#include <cstddef>                                             // for size_t


template <typename T>
class MyVector 
{
    private:
        T*     data;                                           // a pointer to a contingous array of T
        size_t size;                                           // the current num of items stored in the vector
        size_t capacity;                                       // the max poss num of items which can be added into the vector

        // private helper to increase the capacity of the vector when size >= cap
        void increaseCapacity(){
            capacity *= 2;                                     // doubles the capacity each time

            T* newData = new T[capacity];

            for (auto i {0}; i < size; ++i)
                newData[i] = data[i];

            delete[] data;
            data = newData;
        }

    public:
        MyVector() 
            : data(new T[2])
            , size(0)
            , capacity(2) 
            {}

        ~MyVector() 
            {
                delete[] data;
            }

        // adds data to the back
        inline void push_back(const T& val) {
            if (size >= capacity)
                increaseCapacity();                             // increase cap if vector is full

            data[size++] = val;
        }

        // provides random access using []
        inline T& operator[](const size_t index) const {
            return data[index];
        }

        // provides random access using []
        inline const T& operator[](const size_t index) const {
            return data[index];
        }

        // returns the size of the vector
        inline size_t size() const {
            return size;
        }

        // returns true if size == 0
        inline bool empty() const {
            return (size == 0);
        }

        // empties the vector 
        inline void clear() {
            size = 0;                                        // only the size changes; the cap remains as is
        }
};