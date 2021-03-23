
#include "pch.h"

#include "include/CircularBuffer.h"

TEST(CircularBuffer, Construct) {

    CircularBuffer<float> cb(10);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, SimpleWriteRead)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(5);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(5);

    cb.Write(writeFrom.data(), 5);
    EXPECT_TRUE(cb.Size() == 5);

    EXPECT_TRUE(cb.Read(readTo.data(), 5) == 5);
    EXPECT_TRUE(writeFrom == readTo);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, WriteReadCapacitySize)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(10);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(10);

    cb.Write(writeFrom.data(), 10);
    EXPECT_TRUE(cb.Size() == 10);

    EXPECT_TRUE(cb.Read(readTo.data(), 10) == 10);
    EXPECT_TRUE(writeFrom == readTo);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, WriteReadOverCapacitySize)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(15);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> expected(15);
    std::iota(expected.begin(), expected.begin() + 5, 5.0f);
    std::iota(expected.begin() + 5, expected.begin() + 10, 0.0f);

    std::vector<float> readTo(15);

    cb.Write(writeFrom.data(), 15);
    EXPECT_TRUE(cb.Size() == 10);

    EXPECT_TRUE(cb.Read(readTo.data(), 15) == 10);
    EXPECT_TRUE(readTo == expected);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, WriteReadZero)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(15);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(15);
    const std::vector<float> readToCopy(readTo);

    cb.Write(writeFrom.data(), 0);
    EXPECT_TRUE(cb.Size() == 0);

    EXPECT_TRUE(cb.Read(readTo.data(), 0) == 0);
    EXPECT_TRUE(cb.Size() == 0);
    EXPECT_TRUE(readTo == readToCopy);
}

TEST(CircularBuffer, ReadFromEmptyBuffer)
{
    CircularBuffer<float> cb(10);

    std::vector<float> readTo(15, -1);
    const std::vector<float> readToCopy(readTo);

    EXPECT_TRUE(cb.Read(readTo.data(), 100) == 0);
    EXPECT_TRUE(readTo == readToCopy);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, MultipleWritesAndReads)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(9);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(9);

    cb.Write(writeFrom.data(), 3);
    EXPECT_TRUE(cb.Size() == 3);
    cb.Write(writeFrom.data() + 3, 3);
    EXPECT_TRUE(cb.Size() == 6);
    cb.Write(writeFrom.data() + 6, 3);
    EXPECT_TRUE(cb.Size() == 9);

    EXPECT_TRUE(cb.Read(readTo.data(), 3) == 3);
    EXPECT_TRUE(cb.Size() == 6);
    EXPECT_TRUE(cb.Read(readTo.data() + 3, 3) == 3);
    EXPECT_TRUE(cb.Size() == 3);
    EXPECT_TRUE(cb.Read(readTo.data() + 6, 3) == 3);
    EXPECT_TRUE(cb.Size() == 0);

    EXPECT_TRUE(writeFrom == readTo);
}

TEST(CircularBuffer, WriteThenReadAndSum)
{
    CircularBuffer<float> cb(100);
    constexpr size_t transactions{ 5 };

    std::vector<float> writeFrom(15);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(15);

    std::vector<float> expected(15);
    for (size_t i{ 0 }; i < writeFrom.size(); ++i)
        expected[i] = writeFrom[i] * transactions;

    for (size_t i{ 0 }; i < transactions; ++i)
    {
        cb.Write(writeFrom.data(), writeFrom.size());
        EXPECT_TRUE(cb.Size() == (i + 1) * writeFrom.size());
    }

    for (size_t i{ 0 }; i < transactions; ++i)
    {
        EXPECT_TRUE(cb.ReadAndSum(readTo.data(), writeFrom.size()) == writeFrom.size());
        EXPECT_TRUE(cb.Size() == (transactions - i - 1) * writeFrom.size());
    }

    EXPECT_TRUE(readTo == expected);
}

TEST(CircularBuffer, WriteReadAndSumCapacitySize)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(10);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(10);

    cb.Write(writeFrom.data(), 10);
    EXPECT_TRUE(cb.Size() == 10);

    EXPECT_TRUE(cb.ReadAndSum(readTo.data(), 10) == 10);
    EXPECT_TRUE(writeFrom == readTo);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, WriteReadAndSumOverCapacitySize)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(15);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> expected(15);
    std::iota(expected.begin(), expected.begin() + 5, 5.0f);
    std::iota(expected.begin() + 5, expected.begin() + 10, 0.0f);

    std::vector<float> readTo(15);

    cb.Write(writeFrom.data(), 15);
    EXPECT_TRUE(cb.Size() == 10);

    EXPECT_TRUE(cb.ReadAndSum(readTo.data(), 15) == 10);
    EXPECT_TRUE(readTo == expected);
    EXPECT_TRUE(cb.Size() == 0);
}

TEST(CircularBuffer, WriteReadAndSumZero)
{
    CircularBuffer<float> cb(10);

    std::vector<float> writeFrom(15);
    std::iota(writeFrom.begin(), writeFrom.end(), 0.0f);

    std::vector<float> readTo(15);
    const std::vector<float> readToCopy(readTo);

    cb.Write(writeFrom.data(), 0);
    EXPECT_TRUE(cb.Size() == 0);

    EXPECT_TRUE(cb.ReadAndSum(readTo.data(), 0) == 0);
    EXPECT_TRUE(cb.Size() == 0);
    EXPECT_TRUE(readTo == readToCopy);
}

TEST(CircularBuffer, ReadAndSumFromEmptyBuffer)
{
    CircularBuffer<float> cb(10);

    std::vector<float> readTo(15, -1);
    const std::vector<float> readToCopy(readTo);

    EXPECT_TRUE(cb.ReadAndSum(readTo.data(), 100) == 0);
    EXPECT_TRUE(readTo == readToCopy);
    EXPECT_TRUE(cb.Size() == 0);
}