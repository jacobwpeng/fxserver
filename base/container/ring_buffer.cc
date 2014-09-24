/*
 * =====================================================================================
 *
 *       Filename:  ring_buffer.cc
 *        Created:  08/07/14 14:48:29
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "ring_buffer.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            static __thread char local_buf[RingBuffer::kMaxBufferBodyLength];

            RingBuffer::RingBuffer()
                :start_(NULL), end_(NULL), offset_(NULL)
            {
            }

            std::unique_ptr<RingBuffer> RingBuffer::RestoreFrom(void * start, size_t len)
            {
                std::unique_ptr<RingBuffer> rb(new RingBuffer);
                void * mem = start;
                rb->offset_ = reinterpret_cast<OffsetData*>(mem);

                assert (len >= sizeof(OffsetData) + kExtraSpace);

                rb->start_ = static_cast<char*>(start) + sizeof(OffsetData);
                rb->end_ = static_cast<char*>(start) + len;
                return rb;
            }

            std::unique_ptr<RingBuffer> RingBuffer::CreateFrom(void * start, size_t len)
            {
                std::unique_ptr<RingBuffer> rb(new RingBuffer);
                void * mem = start;
                rb->offset_ = reinterpret_cast<OffsetData*>(mem);

                assert (len >= sizeof(OffsetData) + kExtraSpace);

                rb->start_ = static_cast<char*>(start) + sizeof(OffsetData);
                rb->end_ = static_cast<char*>(start) + len;
                rb->set_front(rb->start_);
                rb->set_back(rb->start_);
                rb->set_size(0);
                return rb;
            }

            bool RingBuffer::Push(const char * buf, int len)
            {
                assert (buf != NULL);
                assert (len > 0);

                if (len > space_left()) return false;

                this->Write(buf, len);
                set_size(1 + element_size() );
                return true;
            }

            char * RingBuffer::Pop(int * plen)
            {
                assert (plen);
                if (empty())
                {
                    *plen = 0;
                    return NULL;
                }
                else
                {
                    char * buf = Read(plen);
                    set_size(element_size() - 1);
                    return buf;
                }
            }

            int RingBuffer::space_left() const
            {
                char * front = get_front();
                char * back = get_back();
                if (back >= front)
                {
                    return end_ - back + front - start_ - kExtraSpace;
                }
                else
                {
                    return front - back - kExtraSpace;
                }
            }

            bool RingBuffer::empty() const
            {
                return get_front() == get_back();
            }

            size_t RingBuffer::element_size() const
            {
                return offset_->element_size;
            }

            void RingBuffer::set_size(size_t new_size)
            {
                offset_->element_size = new_size;
            }

            char * RingBuffer::get_front() const
            {
                return offset_->front_offset + start_;
            }

            char * RingBuffer::get_back() const
            {
                return offset_->back_offset + start_;
            }

            void RingBuffer::set_front(char * front)
            {
                assert (front >= start_);
                offset_->front_offset = front - start_;
            }

            void RingBuffer::set_back(char * back)
            {
                assert (back >= start_);
                offset_->back_offset = back - start_;
            }

            void RingBuffer::Write(const char * buf, int len)
            {
                char * front = get_front();
                char * back = get_back();
                assert (end_ >= back);
                if (back + len + sizeof(len) <= end_)
                {
                    //enough space
                    memcpy(back, &len, sizeof(len));
                    back += sizeof(len);
                    memcpy(back, buf, len);
                    back += len;
                }
                else
                {
                    if (back + sizeof(len) <= end_)
                    {
                        //enough space for buffer length
                        memcpy(back, &len, sizeof(len));
                        back += sizeof(len);
                        int first = end_ - back;
                        memcpy(back, buf, first);
                        memcpy(start_, buf + first, len - first);
                        back = start_ + len - first;
                    }
                    else
                    {
                        char * len_addr = reinterpret_cast<char*>(&len);
                        int first = end_ - back;
                        memcpy(back, &len, first);
                        memcpy(start_, len_addr + first, sizeof(len) - first);
                        back = start_ + sizeof(len) - first;

                        memcpy(back, buf, len);
                        back += len;
                    }
                }
                set_back(back);
            }

            char * RingBuffer::Read(int * plen)
            {
                assert (plen);
                char * front = get_front();

                if (empty())
                {
                    *plen = 0;
                    return NULL;
                }

                const int kBufferLength = NextBufferLength();
                assert (kBufferLength > 0);
                assert ((size_t)kBufferLength <= RingBuffer::kMaxBufferBodyLength);   
                *plen = kBufferLength;
                char * buf = local_buf;
                char * content = front + RingBuffer::kBufferHeaderLength;

                if (content > end_)
                {
                    ptrdiff_t offset = content - end_;
                    memcpy(buf, start_ + offset, kBufferLength);
                    front = start_ + offset;
                }
                else if (content + kBufferLength > end_)
                {
                    ptrdiff_t tail_length = end_ - content;
                    memcpy(buf, content, tail_length);
                    memcpy(buf + tail_length, start_, kBufferLength - tail_length);
                    front = start_ + kBufferLength - tail_length;
                }
                else
                {
                    memcpy(buf, content, kBufferLength);
                    front = content + kBufferLength;
                }
                set_front(front);
                return buf;
            }

            int RingBuffer::NextBufferLength() const
            {
                char * front = get_front();

                assert (not empty());
                int len = 0;
                if (front + RingBuffer::kBufferHeaderLength <= end_)
                {
                    len = *(reinterpret_cast<int*>(front));
                }
                else
                {
                    ptrdiff_t offset = end_ - front;
                    char * len_addr = reinterpret_cast<char *>(&len);
                    memcpy(len_addr, front, offset);
                    memcpy(len_addr + offset, start_, RingBuffer::kBufferHeaderLength - offset);
                }
                return len;
            }

        }
    }
}
