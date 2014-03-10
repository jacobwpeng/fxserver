/*
 * =====================================================================================
 *       Filename:  fxbuffer.cc
 *        Created:  15:56:03 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <algorithm>
#include <glog/logging.h>
#include "fxbuffer.h"

FXBuffer::FXBuffer(size_t reserved_size)
    :buf_(reserved_size), read_index_(0), write_index_(0)
{
}

FXBuffer::~FXBuffer()
{

}

void FXBuffer::Append(const char * addr, size_t len)
{
    EnsureSpace(len);
    memcpy(this->Begin(), addr, len);
    write_index_ += len;
}

void FXBuffer::Swap(FXBuffer & other)
{
    std::swap( buf_, other.buf_ );
    std::swap( read_index_, other.read_index_ );
    std::swap( write_index_, other.write_index_ );
}

size_t FXBuffer::BytesToRead() const
{
    assert( write_index_ >= read_index_ );
    return write_index_ - read_index_;
}

const char * FXBuffer::Begin() const
{
    return const_cast<FXBuffer*>(this)->Begin();
}

char * FXBuffer::Begin()
{
    return &buf_[write_index_];
}

size_t FXBuffer::Capacity() const
{
    return this->buf_.size();
}

const char * FXBuffer::Read() const
{
    return &buf_[read_index_];
}

void FXBuffer::Clear()
{
    read_index_ = write_index_ = 0;
}

void FXBuffer::Shrink()
{
    assert( write_index_ >= read_index_ );
    size_t content_size = write_index_ - read_index_;
    size_t new_size = ( content_size / BUFFER_INCREASE_STEP + 1 ) * BUFFER_INCREASE_STEP;

    std::vector<char> tmp(new_size);
    memcpy( &tmp[0], &buf_[read_index_], content_size );
    buf_.swap( tmp );
    read_index_ = 0;
    write_index_ = content_size;
}

void FXBuffer::EnsureSpace(size_t append_len)
{
    size_t len = buf_.size();
    assert( write_index_ < len );

    if( len - write_index_ < append_len )
    {
        size_t extra_space_len = len - write_index_;
        size_t extra_space_to_grow = (extra_space_len / BUFFER_INCREASE_STEP + 1) * BUFFER_INCREASE_STEP;
        buf_.resize( buf_.size() + extra_space_to_grow );

    }
}
