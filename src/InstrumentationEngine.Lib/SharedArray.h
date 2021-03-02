// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <type_traits>
#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <atomic>
#pragma warning(pop)

namespace MicrosoftInstrumentationEngine
{
    // A simple class that works around the fact that the standard libraries
    // do not support shared pointers to arrays of POD objects. Internally allocates and
    // reference counts array data, ensuring that the standard array
    // deleter is called when all instances go out of scope.
    template <class T>
    class CSharedArray
    {
    private:

        // Internal shared data.
        class CSharedData
        {
        public:
            size_t m_count;
            T* m_internalData;

            CSharedData(size_t count) : m_count(count)
            {
                m_internalData = new T[count];
            }

            ~CSharedData()
            {
                if (m_internalData != nullptr)
                {
                    delete[] m_internalData;
                    m_internalData = nullptr;
                }
            }
        };

        // used to do reference counting.
        std::shared_ptr<CSharedData> m_data;

    public:

        CSharedArray() : m_data(nullptr) { }

        CSharedArray(size_t count)
        {
            m_data = make_shared<CSharedData>(count);
        }

        CSharedArray(const CSharedArray<T>& other)
        {
            m_data = other.m_data;
        }

        CSharedArray(CSharedArray<T>&& other)
        {
            m_data = std::move(other.m_data);
            other.m_data = nullptr;
        }

        CSharedArray<T>& operator=(const CSharedArray& other)
        {
            m_data = other.m_data;
            return *this;
        }

        CSharedArray<T>& operator=(CSharedArray&& other) noexcept
        {
            if (this != &other)
            {
                m_data = std::move(other.m_data);
                other.m_data = nullptr;
            }

            return *this;
        }

        T operator[](size_t index) const
        {
            return m_data->m_internalData[index];
        }

        T& operator[](size_t index)
        {
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