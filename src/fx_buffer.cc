/*
 * =====================================================================================
 *       Filename:  fx_buffer.cc
 *        Created:  11:17:49 Apr 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_buffer.h"
#include <cassert>
#include <cstring>                              /* for memcpy */

namespace fx
{
    Buffer::Buffer(size_t reserved_size)
        :internal_buf_(reserved_size), read_index_(0), write_index_(0)
    {

    }

    Buffer::~Buffer()
    {

    }

    size_t Buffer::BytesCanWrite() const
    {
        assert( internal_buf_.size() >= write_index_ );
        return internal_buf_.size() - write_index_;
    }

    char * Buffer::WriteBegin()
    {
        assert( internal_buf_.size() >= write_index_ );
        return &internal_buf_[write_index_];
    }

    void Buffer::AddBytes(size_t len)
    {
        write_index_ += len;
        assert( write_index_ <= internal_buf_.size() );
    }

    size_t Buffer::BytesToRead() const
    {
        assert( write_index_ >= read_index_ );
        return write_index_ - read_index_;
    }

    char * Buffer::ReadBegin()
    {
        assert( internal_buf_.size() >= read_index_ );
        return &internal_buf_[read_index_];
    }

    void Buffer::ConsumeBytes(size_t len)
    {
        read_index_ += len;
        assert( read_index_ <= write_index_ );
        if( read_index_ == write_index_ )
        {
            /* 所有内容都读完了 */
            Clear();
        }
    }

    void Buffer::Append( const char * buf, size_t len )
    {
        EnsureSpace( len );
        assert( BytesCanWrite() >= len );

        memcpy( WriteBegin(), buf, len );
        write_index_ += len;
    }

    std::string Buffer::ReadAndClear()
    {
        std::string tmp( ReadBegin(), BytesToRead() );
        Clear();
        return tmp;
    }

    void Buffer::Clear()
    {
        read_index_ = write_index_ = 0;
    }

    size_t Buffer::ByteSize() const
    {
        return internal_buf_.size();
    }

    void Buffer::EnsureSpace(size_t len)
    {
        size_t bytes_left = BytesCanWrite();
        /* TODO : 更高级的扩充方式 */
        if( bytes_left < len )
        {
            internal_buf_.resize( internal_buf_.size() + len - bytes_left );
        }
    }
}
