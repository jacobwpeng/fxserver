/*
 * =====================================================================================
 *
 *       Filename:  ring_buffer.h
 *        Created:  08/07/14 14:26:45
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __RING_BUFFER_H__
#define  __RING_BUFFER_H__

#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fx
{
    namespace base
    {
        namespace container
        {
            class RingBuffer : boost::noncopyable
            {
                private:
                    struct OffsetData
                    {
                        ptrdiff_t front_offset;
                        ptrdiff_t back_offset;
                        size_t element_size;
                    };
                    RingBuffer();

                public:
                    static const size_t kMaxBufferBodyLength = 1 << 16; //64KB

                public:

                    static std::unique_ptr<RingBuffer> RestoreFrom(void * start, size_t len);
                    static std::unique_ptr<RingBuffer> CreateFrom(void * start, size_t len);

                    bool Push(const char * buf, int len);
                    char * Pop(int* len);

                    int space_left() const;
                    bool empty() const;
                    size_t element_size() const;

                private:
                    char * get_front() const;
                    char * get_back() const;

                    void set_front(char * front);
                    void set_back(char * back);
                    void set_size(size_t size);

                    int NextBufferLength() const;
                    void Write(const char * buf, int len);
                    char * Read(int * plen);

                private:
                    static const size_t kBufferHeaderLength = sizeof(int);
                    static const size_t kMaxBufferLength = kBufferHeaderLength  + kMaxBufferBodyLength;
                    static const size_t kExtraSpace = 1;
                    char * start_;
                    char * end_;
                    volatile OffsetData * offset_;
            };
        }
    }
}

#endif   /* ----- #ifndef __RING_BUFFER_H__  ----- */
