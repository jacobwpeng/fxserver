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

static __thread char local_buf[fx::base::RingBuffer::kMaxBufferBodyLength];
fx::base::RingBuffer::RingBuffer(char * start, char * end, bool reuse_offsetdata)
{
    assert (end >= start);
    assert (end >= start + sizeof(OffsetData) + kExtraSpace);

    void * mem = start;
    offset_ = reinterpret_cast<OffsetData*>(mem);

    start_ = start + sizeof(OffsetData);
    end_ = end;

    if (not reuse_offsetdata)
    {
        set_front(start_);
        set_back(start_);
        set_size(0);
    }
}

bool fx::base::RingBuffer::Push(const char * buf, int len)
{
    assert (buf != NULL);
    assert (len > 0);

    if (len > space_left()) return false;

    this->Write(buf, len);
    set_size(1 + element_size() );
    return true;
}

char * fx::base::RingBuffer::Pop(int * plen)
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

int fx::base::RingBuffer::space_left() const
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

bool fx::base::RingBuffer::empty() const
{
    return get_front() == get_back();
}

size_t fx::base::RingBuffer::element_size() const
{
    return offset_->element_size;
}

void fx::base::RingBuffer::set_size(size_t new_size)
{
    offset_->element_size = new_size;
}

char * fx::base::RingBuffer::get_front() const
{
    return offset_->front_offset + start_;
}

char * fx::base::RingBuffer::get_back() const
{
    return offset_->back_offset + start_;
}

void fx::base::RingBuffer::set_front(char * front)
{
    assert (front >= start_);
    offset_->front_offset = front - start_;
}

void fx::base::RingBuffer::set_back(char * back)
{
    assert (back >= start_);
    offset_->back_offset = back - start_;
}

void fx::base::RingBuffer::Write(const char * buf, int len)
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

char * fx::base::RingBuffer::Read(int * plen)
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
    assert ((size_t)kBufferLength <= fx::base::RingBuffer::kMaxBufferBodyLength);   
    *plen = kBufferLength;
    char * buf = local_buf;
    char * content = front + fx::base::RingBuffer::kBufferHeaderLength;

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

int fx::base::RingBuffer::NextBufferLength() const
{
    char * front = get_front();

    assert (not empty());
    int len = 0;
    if (front + fx::base::RingBuffer::kBufferHeaderLength <= end_)
    {
        len = *(reinterpret_cast<int*>(front));
    }
    else
    {
        ptrdiff_t offset = end_ - front;
        char * len_addr = reinterpret_cast<char *>(&len);
        memcpy(len_addr, front, offset);
        memcpy(len_addr + offset, start_, fx::base::RingBuffer::kBufferHeaderLength - offset);
    }
    return len;
}
