#pragma once


#include <cstddef>                                             // for size_t


// a generic vector/dynamic-array class
template <typename T>
class MyVector 
{
    private:
        T*     data;                                           // a pointer to a contingous array of T
        size_t vectorSize;                                     // the current num of items stored in the vector
        size_t capacity;                                       // the max poss num of items which can be added into the vector

        // private helper to increase the capacity of the vector when vectorSize >= cap
        void increaseCapacity(){
            capacity *= 2;                                     // doubles the capacity each time

            T* newData = new T[capacity];

            for (auto i {0}; i < vectorSize; ++i)
                newData[i] = data[i];

            delete[] data;
            data = newData;
        }

    public:
        MyVector() 
            : data(new T[2])
            , vectorSize(0)
            , capacity(2) 
            {}

        ~MyVector() 
            {
                delete[] data;
            }

        // copy contructor
        MyVector(const MyVector& other) 
            : vectorSize(other.vectorSize)
            , capacity(other.capacity)
            , data(new T[other.capacity]) 
            {
                for (auto i {0}; i < vectorSize; ++i)
                    data[i] = other.data[i];
            }

        // copy assignment operator
        MyVector& operator=(const MyVector& other) {
            if (this == &other) return *this;
            
            delete[] data;

            vectorSize = other.vectorSize;
            capacity   = other.capacity;
            data       = new T[capacity];
            
            for (auto i {0}; i < vectorSize; ++i)
                data[i] = other.data[i];

            return *this;
        }

        // adds data to the back
        inline void push_back(const T& val) {
            if (vectorSize >= capacity)
                increaseCapacity();                             // increase cap if vector is full

            data[vectorSize++] = val;
        }
        
        // removes data from the back
        inline void pop_back() {
            if (vectorSize) --vectorSize;
        }

        // provides random access and modification using []
        inline T& operator[](const size_t index) {
            return data[index];
        }

        // provides read-only random access usin    g [] for const objects
        inline const T& operator[](const size_t index) const {
            return data[index]; 
        }

        // returns the vectorSize of the vector
        inline size_t size() const {
            return vectorSize;
        }

        // returns true if vectorSize == 0
        inline bool empty() const {
            return (vectorSize == 0);
        }

        // empties the vector 
        inline void clear() {
            vectorSize = 0;                                        // only the vectorSize changes; the cap remains as is
        }

        inline T* begin() {
            return data;                                           // ie &data[0]
        }

        inline T* end() {
            return (data + vectorSize);                            // ie &data[data.size()]
        }

        inline const T* begin() const {
            return data;                                           // ie &data[0]
        }

        inline const T* end() const {
            return (data + vectorSize);                            // ie &data[data.size()]
        }
};