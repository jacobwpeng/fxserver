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

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fx
{
    namespace base
    {
        class RingBuffer : boost::noncopyable
        {
            struct OffsetData
            {
                OffsetData()
                    :front_offset(0), back_offset(0), element_size(0)
                {

                }
                ptrdiff_t front_offset;
                ptrdiff_t back_offset;
                size_t element_size;
            };

            public:
                typedef boost::function< char* (int) > Allocator;
                static const size_t kMaxBufferBodyLength = 1 << 16; //64KB

            public:
                RingBuffer(char * start, char * end, bool reuse_offsetdata, Allocator allocator = RingBuffer::DefaultAllocator);
                bool Push(const char * buf, int len);
                char * Pop(int* len);

                int space_left() const;
                bool empty() const;
                size_t element_size() const;
                
            private:
                static char * DefaultAllocator(int len);
                char * get_front() const;
                char * get_back() const;

                void set_front(char * front);
                void set_back(char * back);
                void set_size(size_t size);

                int NextBufferLength() const;
                void Write(const char * buf, int len);
                char * Read(int * plen);

            private:
                static const size_t kMaxBufferHeaderLength = sizeof(int);
                static const size_t kMaxBufferLength = kMaxBufferHeaderLength  + kMaxBufferBodyLength;
                static const size_t kExtraSpace = 1;
                char * start_;
                char * end_;
                volatile OffsetData * offset_;
                Allocator allocator_;
        };
    }
}

#endif   /* ----- #ifndef __RING_BUFFER_H__  ----- */
