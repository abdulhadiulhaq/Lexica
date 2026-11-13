#pragma once


typedef  long long    size_t;
template <typename T>


class MyVector 
{
    private:
        T*     data;
        size_t size;
        size_t capacity;

        void increaseCapacity() 
        {
            capacity = ((!capacity)? 4 : (capacity * 2));

            T* newData = new T[capacity];

            for (auto i {0}; i < size; ++i)
                new_data[i] = data[i];

            delete[] data;
            data = newData;
        }

    public:
        MyVector() 
            : data(new T[4])
            , size(0)
            , capacity(4) 
            {}

        ~MyVector() 
        {
            delete[] data;
        }


        inline void push_back(const T& val) 
        {
            if (size >= capacity)
                increaseCapacity();

            data[size++] = val;
        }

        inline T& operator[](const size_t index) 
        {
            return data[index];
        }

        inline const T& operator[](const size_t index) const 
        {
            return data[index];
        }

        inline size_t size() const 
        {
            return size;
        }

        inline bool empty() const 
        {
            return (size == 0);
        }

        inline void clear() 
        {
            size = 0;
        }
};