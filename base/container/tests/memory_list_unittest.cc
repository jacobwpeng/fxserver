/*
 * =====================================================================================
 *
 *       Filename:  memory_list_unittest.cc
 *        Created:  08/24/14 10:45:32
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "memory_list.h"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>

using namespace fx::base;

class MemoryListUnittest : public ::testing::Test
{
    protected:
        struct Element
        {
            MemoryList::SliceId id;
            unsigned uin;
            unsigned timestamp;
        };

        virtual void SetUp()
        {
            ::srand(::time(NULL));
        }

        virtual void TearDown()
        {
        }
};

TEST_F(MemoryListUnittest, Create)
{
    char buf[1 << 16];
    boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, sizeof(buf), 20));
    EXPECT_TRUE (ml != NULL);
    EXPECT_EQ (ml->BufferLength(), sizeof(buf));
    // header -> 48Byte
    // floor((65536 - 48) / (20 + 8)) == 2338
    EXPECT_EQ (ml->capacity(), 2338);
    EXPECT_EQ (0, ml->size());
    EXPECT_EQ (buf, ml->start());
    EXPECT_FALSE (ml->full());
}

TEST_F(MemoryListUnittest, CreateFailed)
{
    char buf[MemoryList::kHeaderSize - 1];
    boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, sizeof(buf), 1));
    EXPECT_TRUE (ml == NULL);
}

TEST_F(MemoryListUnittest, CreateEmpty)
{
    char buf[MemoryList::kHeaderSize];
    boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, sizeof(buf), 1));
    EXPECT_TRUE (ml != NULL);
    EXPECT_EQ (ml->capacity(), 0);
    EXPECT_EQ (ml->size(), 0);
    EXPECT_TRUE (ml->full());
    EXPECT_TRUE (MemoryList::kInvalidSliceId == ml->GetSlice());
}

TEST_F(MemoryListUnittest, Malloc)
{
    const size_t kBufSize = 1 << 16;            /* 64 KiB */
    char buf[kBufSize];
    boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, kBufSize, 20));
    ASSERT_TRUE (ml != NULL);

    int size = 0;

    while (1)
    {
        MemoryList::SliceId id = ml->GetSlice();
        if (id != MemoryList::kInvalidSliceId)
        {
            EXPECT_EQ (id, size);
            ++size;
        }
        else break;
    }

    EXPECT_EQ (size, ml->size());
    EXPECT_EQ (size, ml->capacity());
    EXPECT_TRUE (ml->full());
}

TEST_F(MemoryListUnittest, Free)
{
    const size_t kBufSize = 1 << 16;            /* 64 KiB */
    char buf[kBufSize];
    boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, kBufSize, 20));
    ASSERT_TRUE (ml != NULL);

    MemoryList::SliceId ids[ml->capacity()];
    size_t size = 0;
    while (1)
    {
        MemoryList::SliceId id = ml->GetSlice();
        if (id != MemoryList::kInvalidSliceId)
        {
            ids[size] = id;
            ++size;
        }
        else break;
    }
    EXPECT_EQ (size, ml->size());

    for (size_t idx = 0; idx != ml->capacity(); ++idx)
    {
        EXPECT_EQ (ml->size(), ml->capacity() - idx);
        ml->FreeSlice(ids[idx]);
    }

    EXPECT_EQ (ml->size(), 0);
}

TEST_F(MemoryListUnittest, Restore)
{
    const size_t kBufSize = 1 << 16;            /* 64 KiB */
    std::vector<Element> elements;
    char buf[kBufSize];
    {
        boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(buf, kBufSize, sizeof(Element)));
        ASSERT_TRUE (ml != NULL);
        while (1)
        {
            MemoryList::SliceId id = ml->GetSlice();
            if (id != MemoryList::kInvalidSliceId)
            {
                Element * element = reinterpret_cast<Element*>(ml->GetBuffer(id));
                element->id = id;
                element->uin = rand();
                element->timestamp = rand();
                elements.push_back (*element);
            }
            else break;
        }
    }

    //move to another address space
    char newbuf[kBufSize * 2 + 1024];                  /* with larger space */
    ::memcpy(newbuf, buf, kBufSize);
    {
        boost::scoped_ptr<MemoryList> ml(MemoryList::CreateFrom(newbuf, sizeof(newbuf), sizeof(Element)));
        ASSERT_TRUE (ml != NULL);

        for (size_t i = 0; i != elements.size(); ++i)
        {
            Element element = elements[i];
            Element * ptr = reinterpret_cast<Element*>(ml->GetBuffer(element.id));
            EXPECT_TRUE (::memcmp(&element, ptr, sizeof(Element)) == 0);
        }
    }
}
