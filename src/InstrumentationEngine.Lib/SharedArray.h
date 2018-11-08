// Copyright (c) Microsoft Corporation. All rights reserved.
// 
#pragma once

#include <type_traits>
#include <atomic>

namespace MicrosoftInstrumentationEngine
{
    // A simple class that works around the fact that the standard libraries
    // do not support shared pointers to arrays of POD objects. Internally allocates and
    // reference counts array data, ensuring that the standard array
    // deleter is called when all instances go out of scope.
    template <class T>
    class SharedArray
    {
    private:

        // Internal shared data. 
        class SharedData
        {
        public:
            size_t m_count;
            T* m_internalData;

            SharedData(size_t count) : m_count(count)
            {
                m_internalData = new T[count];
            }

            ~SharedData()
            {
                if (m_internalData != nullptr)
                {
                    delete[] m_internalData;
                    m_internalData = nullptr;
                }
            }
        };

        // used to do reference counting.
        std::shared_ptr<SharedData> m_data;

        void CheckIndex(size_t index) const
        {
            if (index >= Count())
            {
                throw std::out_of_range("SharedArray index out of range");
            }
        }

    public:

        SharedArray() : m_data(nullptr) { }

        SharedArray(size_t count)
        {
            m_data = make_shared<SharedData>(count);
        }

        SharedArray(const SharedArray<T>& other)
        {
            m_data = other.m_data;
        }

        SharedArray(SharedArray<T>&& other)
        {
            m_data = std::move(other.m_data);
            other.m_data = nullptr;
        }

        SharedArray<T>& operator=(const SharedArray& other)
        {
            m_data = other.m_data;
            return *this;
        }

        SharedArray<T>& operator=(SharedArray&& other)
        {
            if (this != other)
            {
                m_data = std::move(other.m_data);
                other.m_data = nullptr;
            }

            return &this;
        }

        T operator[](size_t index) const
        {
            CheckIndex(index);
            return m_data->m_internalData[index];
        }

        T& operator[](size_t index)
        {
            CheckIndex(index);
            return m_data->m_internalData[index];
        }

        size_t Count() const
        {
            if (m_data == nullptr)
            {
                return 0;
            }

            return m_data->m_count;
        }

        bool IsEmpty() const
        {
            return Count() == 0;
        }

        void Reset(const SharedArray<T>& other)
        {
            m_data = other.m_data;
        }

        // Returns a pointer to the underlying data. Notes, clients
        // should not store this pointer, as it can become invalid
        // when all instances of the shared array go out of scope.
        T* Get()
        {
            if (m_data == nullptr)
            {
                return nullptr;
            }

            return m_data->m_internalData;
        }

        void ZeroMem()
        {
            if (Count() > 0)
            {
                memset(Get(), 0, Count() * sizeof(T));
            }
        }
    };
}