
#pragma once

#include <vector>
#include <mutex>

template<typename T>
class CircularBuffer
{
public:
    CircularBuffer(size_t capacity) : m_Data(capacity) {}

public:
    using Iterator = typename std::vector<T>::iterator;
    using ConstIterator = typename std::vector<T>::const_iterator;

public:
    size_t Size() const { return m_Size; };

    void Write(const T* src, size_t count)
    {
        const int distanceToEnd{ m_Data.cend() - m_WritePoint };
        const int overrun{ static_cast<int>(count) - distanceToEnd };

        if (overrun < 0)
        {
            std::copy(src, src + count, m_WritePoint);
            m_WritePoint += count;
        }
        else
        {
            if (count > m_Data.size())
                count = m_Data.size();

            std::copy(src, src + distanceToEnd, m_WritePoint);
            std::copy(src + distanceToEnd, src + count, m_Data.begin());
            m_WritePoint = m_Data.begin() + overrun;
        }

        std::unique_lock<std::mutex> lk{ m_Mutex };
        if (m_Size + count < m_Data.size())
            m_Size += count;
        else
        {
            m_ReadPoint = m_WritePoint;
            m_Size = m_Data.size();
        }
    }

    size_t Read(T* dest, size_t count)
    {
        std::unique_lock<std::mutex> lk{ m_Mutex };

        if (count > m_Size)
        {
            count = m_Size;
            if (!count)
                return count;
        }

        const int distanceToEnd{ m_Data.cend() - m_ReadPoint };
        const int overrun{ static_cast<int>(count) - distanceToEnd };

        if (overrun < 0)
        {
            std::copy(m_ReadPoint, m_ReadPoint + count, dest);
            m_ReadPoint += count;
        }
        else
        {
            std::copy(m_ReadPoint, m_Data.cend(), dest);
            std::copy(m_Data.cbegin(), m_Data.cbegin() + overrun, dest + distanceToEnd);
            m_ReadPoint = m_Data.cbegin() + overrun;
        }

        m_Size -= count;
        return count;
    }

    size_t ReadAndSum(T* dest, size_t count)
    {
        std::unique_lock<std::mutex> lk{ m_Mutex };

        if (count > m_Size)
        {
            count = m_Size;
            if (!count)
                return count;
        }

        const int distanceToEnd{ m_Data.cend() - m_ReadPoint };
        const int overrun{ static_cast<int>(count) - distanceToEnd };

        if (overrun < 0)
        {
            std::transform(m_ReadPoint, m_ReadPoint + count, dest, dest, std::plus<T>());
            m_ReadPoint += count;
        }
        else
        {
            std::transform(m_ReadPoint, m_Data.cend(), dest, dest, std::plus<T>());
            std::transform(m_Data.cbegin(),
                m_Data.cbegin() + overrun,
                dest + distanceToEnd,
                dest + distanceToEnd,
                std::plus<T>());
            m_ReadPoint = m_Data.cbegin() + overrun;
        }

        m_Size -= count;
        return count;
    }
   
private:
    std::vector<T> m_Data;
    Iterator m_WritePoint = m_Data.begin();
    ConstIterator m_ReadPoint = m_Data.cbegin();
    size_t m_Size = 0;
    std::mutex m_Mutex;
};