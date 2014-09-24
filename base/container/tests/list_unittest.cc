/*
 * =====================================================================================
 *
 *       Filename:  list_unittest.cc
 *        Created:  08/25/14 22:49:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <ctime>
#include <cstdlib>
#include <vector>
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>

#include "list.h"

using fx::base::container::List;

class ListUnittest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            ::srand(::time(NULL));
        }

        virtual void TearDown()
        {

        }
};

TEST_F(ListUnittest, Create)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (list != NULL);
}

TEST_F(ListUnittest, CreateFailed)
{
    typedef List<unsigned> ListType;
    const size_t kBufSize = ListType::kHeaderSize + fx::base::MemoryList::kHeaderSize - 1;
    char buf[kBufSize];

    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (list == NULL);
}

TEST_F(ListUnittest, CreateEmpty)
{
    typedef List<unsigned> ListType;
    const size_t kBufSize = ListType::kHeaderSize + fx::base::MemoryList::kHeaderSize;
    char buf[kBufSize];

    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), 0);
    EXPECT_EQ (list->capacity(), 0);
}

TEST_F(ListUnittest, PushBack)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), 0);
    //ListType::kHeaderSize = 24, fx::base::MemoryList::kHeaderSize = 48, ElementSize = 4 + 2 * 4, ExtraSpace = 2 * 4;
    //capacity = (65536 - 24 - 48) / (20) == 5455
    EXPECT_EQ (list->capacity(), 3273);

    while (ListType::kInvalidNodeId != list->PushBack(rand()));
    EXPECT_EQ (list->size(), list->capacity());
}

TEST_F(ListUnittest, PushFront)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (list != NULL);

    while (ListType::kInvalidNodeId != list->PushFront(rand()));
    EXPECT_EQ (list->size(), list->capacity());
}

TEST_F(ListUnittest, PopBack)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (list != NULL);

    std::vector<unsigned> v;
    while (1)
    {
        unsigned val = rand();
        if (ListType::kInvalidNodeId == list->PushFront(val)) break;
        else v.push_back(val);
    }
    ASSERT_EQ (list->size(), v.size());
    for (size_t i = 0; i != v.size(); ++i)
    {
        EXPECT_EQ (v[i], list->PopBack());
        EXPECT_EQ (list->size(), v.size() - i - 1);
    }
    EXPECT_EQ (list->size(), 0);
}

TEST_F(ListUnittest, PopFront)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (list != NULL);

    std::vector<unsigned> v;
    while (1)
    {
        unsigned val = rand();
        if (ListType::kInvalidNodeId == list->PushBack(val)) break;
        else v.push_back(val);
    }
    ASSERT_EQ (list->size(), v.size());
    for (size_t idx = 0; idx != v.size(); ++idx)
    {
        EXPECT_EQ (v[idx], list->PopFront());
        EXPECT_EQ (list->size(), v.size() - idx - 1);
    }
    EXPECT_EQ (list->size(), 0);
}

TEST_F(ListUnittest, Back)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (list != NULL);
    list->PushBack(1);
    list->PushBack(2);
    EXPECT_EQ (2, list->PopBack());
    EXPECT_EQ (1, list->Back());
    EXPECT_EQ (1, list->PopBack());
}

TEST_F(ListUnittest, Unlink)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (list != NULL);
    ListType::NodeId first = list->PushBack(1);
    ListType::NodeId second = list->PushBack(2);
    ListType::NodeId thrid = list->PushBack(3);

    EXPECT_EQ (2, list->Unlink(second));
    EXPECT_EQ (2, list->size());
    EXPECT_EQ (1, list->PopFront());
    EXPECT_EQ (3, list->PopBack());
}

TEST_F(ListUnittest, RestoreFrom)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef List<unsigned> ListType;
    
    std::vector<unsigned> v;
    {
        boost::scoped_ptr<ListType> list(ListType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (list != NULL);

        while (1)
        {
            unsigned val = rand();
            if (ListType::kInvalidNodeId == list->PushBack(val)) break;
            else v.push_back(val);
        }
    }

    //old capacity = (65536 - 24 - 48) / (20), padding = 4
    char newbuf[kBufSize * 2 + 4];                  /* larger space */
    ::memcpy(newbuf, buf, sizeof(buf));
    boost::scoped_ptr<ListType> list(ListType::RestoreFrom(newbuf, sizeof(newbuf)));
    ASSERT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), v.size());
    //capacity = (65536*2 + 4 - 24 - 48) / (20) == 6550
    EXPECT_EQ (list->capacity(), 6550);
    for (size_t idx = 0; idx != v.size(); ++idx)
    {
        EXPECT_EQ (v[idx], list->PopFront());
        EXPECT_EQ (list->size(), v.size() - idx - 1);
    }
}
