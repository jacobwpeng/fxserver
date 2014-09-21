/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  10:23:27 Apr 22, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  test of NetAddress
 *
 * =====================================================================================
 */

#include "ring_buffer.h"
#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using fx::base::RingBuffer;
static const char * mmap_filename = "/tmp/test-ringbuffer.mmap";

class RingBufferTest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            //create mmap file
            len_ = 20 * (1 << 20); //20 MiB
            int fd_ = open(mmap_filename, O_RDWR | O_CREAT, 0666);
            ASSERT_FALSE (fd_ < 0);
            fcntl(fd_, F_SETFD, FD_CLOEXEC);

            ftruncate(fd_, len_);
            mem_ = ::mmap(NULL, len_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
            ASSERT_FALSE (mem_ == MAP_FAILED);
            char * start = reinterpret_cast<char*>(mem_);
            char * end = start + len_;
            buf_ = new RingBuffer(start, end, false);

            GenerateRandomStringList();
        }

        virtual void TearDown()
        {
            delete buf_;
            ::munmap(mem_, len_);
            close(fd_);
        }

        void GenerateRandomStringList()
        {
            FILE * fp = fopen("/dev/urandom", "r");
            ASSERT_FALSE (fp == NULL);

            char buf[RingBuffer::kMaxBufferBodyLength];
            int total_len = 0;
            for (int i = 0; i != 1048576; ++i)
            {
                int len = rand() % RingBuffer::kMaxBufferBodyLength;

                if (len < 0) len = -len;
                else if (len == 0) continue;
                else {}

                fread(buf, 1, len, fp);
                strings_.push_back (std::string(buf, len));
                total_len += len;

                if (total_len >= (20 * (1<<20))) break;
            }

            fclose(fp);
        }

        void WriteFromSubProcess()
        {
            if (0 != fork()) return;
            int fd = open(mmap_filename, O_RDWR);
            ASSERT_FALSE (fd < 0);

            void * mem = ::mmap(NULL, len_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (mem == MAP_FAILED) perror("mmap");
            ASSERT_TRUE (mem != MAP_FAILED);
            char * start = reinterpret_cast<char*>(mem);
            char * end = start + len_;
            RingBuffer buf(start, end, true);

            for (size_t i = 0; i != strings_.size(); ++i)
            {
                bool succeed = false;
                do
                {
                    succeed = buf.Push(strings_[i].data(), strings_[i].size());
                }while (not succeed);
            }
            close(fd);
            exit(0);
        }

        int fd_;
        void * mem_;
        size_t len_;
        RingBuffer * buf_;
        std::vector<std::string> strings_;
};

TEST_F(RingBufferTest, TestBasic)
{
    EXPECT_TRUE (buf_->empty());
    EXPECT_TRUE (buf_->element_size() == 0u);
}

TEST_F(RingBufferTest, TestPush)
{
    std::string str = "Hello, World!";
    bool succeed = buf_->Push(str.c_str(), str.size()); 
    EXPECT_TRUE (succeed);
    EXPECT_FALSE (buf_->empty());
    EXPECT_EQ (buf_->element_size(), 1u);
}

TEST_F(RingBufferTest, TestPop)
{
    std::string str = "Hello, World!";
    bool succeed = buf_->Push(str.c_str(), str.size()); 
    EXPECT_TRUE (succeed);

    int len;
    char * msg = buf_->Pop(&len);
    std::string out(msg, len);

    EXPECT_EQ (len, str.size());
    EXPECT_EQ (out, str);
    EXPECT_EQ (buf_->element_size(), 0u);
    EXPECT_TRUE (buf_->empty());
}

TEST_F(RingBufferTest, TestMassivePush)
{
    const std::string fixed_length_string = "Hello, Fedora!";
    bool succeed = false;
    size_t size = 0;
    do
    {
        if (succeed) ++size;
        succeed = buf_->Push(fixed_length_string.c_str(), fixed_length_string.size());
    }while (succeed);

    EXPECT_TRUE ((size_t)buf_->space_left() < fixed_length_string.size());
    EXPECT_FALSE (buf_->empty());
    EXPECT_EQ (buf_->element_size(), size);
}

TEST_F(RingBufferTest, TestMassivePop)
{
    const std::string fixed_length_string = "Hello, Fedora!";
    bool succeed = false;
    size_t size = 0;
    do
    {
        if (succeed) ++size;
        succeed = buf_->Push(fixed_length_string.c_str(), fixed_length_string.size());
    }while (succeed);

    char * msg = NULL;
    int len;
    do
    {
        msg = buf_->Pop(&len);
        if (msg != NULL)
        {
            --size;
            EXPECT_EQ ((size_t)len, fixed_length_string.size());
            std::string out(msg, len);
            EXPECT_EQ (out, fixed_length_string);
        }
    }while (msg != NULL);

    EXPECT_TRUE (size == 0);
    EXPECT_TRUE (buf_->empty());
    EXPECT_TRUE (buf_->element_size() == 0);
    char * last = buf_->Pop(&len);
    EXPECT_TRUE (last == NULL);
    EXPECT_TRUE (len == 0);
}

TEST_F(RingBufferTest, TestRandomContent)
{
    for (size_t i = 0; i != strings_.size(); ++i)
    {
        bool succeed = buf_->Push(strings_[i].data(), strings_[i].size());
        EXPECT_TRUE (succeed);

        int len;
        char * out = buf_->Pop(&len);
        EXPECT_EQ ((size_t)len, strings_[i].size());
        EXPECT_TRUE (out != NULL);
        EXPECT_EQ (std::string(out, len), strings_[i]);
    }
}

TEST_F(RingBufferTest, TestLockFreeSingleRW)
{
    WriteFromSubProcess();
    for (size_t i = 0; i != strings_.size(); ++i)
    {
        char * out = NULL;
        int len;
        do
        {
            out = buf_->Pop(&len);
        }while (out == NULL);

        EXPECT_EQ ((size_t)len, strings_[i].size());
        EXPECT_EQ (std::string(out, len), strings_[i]);
    }
}
