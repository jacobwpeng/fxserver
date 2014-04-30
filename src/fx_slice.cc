/*
 * =====================================================================================
 *
 *       Filename:  fx_slice.cc
 *        Created:  04/26/14 18:41:26
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_slice.h"
#include <cassert>
#include <vector>
#include <cstring>

namespace fx
{
    string Slice::to_string( const Slice& slice )
    {
        return string(slice.data(), slice.size() );
    }

    Slice::Slice()
        :buf_(NULL), len_(0u)
    {
    }

    Slice::Slice( const char * buf, size_t len ) 
        :buf_(buf), len_(len)
    {
    }

    Slice::Slice( const string& str )
        :buf_(str.c_str()), len_(str.length())
    {
    }

    Slice::Slice(const Slice& slice)
    {
        buf_ = slice.buf_;
        len_ = slice.len_;
    }

    void Slice::clear()
    {
        buf_ = NULL;
        len_ = 0u;
    }

    void Slice::swap(Slice& rhs)
    {
        std::swap( buf_, rhs.buf_ );
        std::swap( len_, rhs.len_ );
    }

    void Slice::assign(const Slice& rhs)
    {
        buf_ = rhs.buf_;
        len_ = rhs.len_;
    }

    void Slice::assign(const char* buf, size_t len)
    {
        buf_ = buf;
        len_ = len;
    }

    void Slice::assign(const string& str)
    {
        buf_ = str.c_str();
        len_ = str.length();
    }

    Slice Slice::subslice(size_t pos, size_t len)
    {
        if( pos == len_ ) return Slice();
        assert( pos < len_ );

        if( len > (len_ - pos) ) return Slice( buf_ + pos, len_ - pos );
        else return Slice( buf_ + pos, len );
    }

    size_t Slice::find(const char* p) const
    {
        size_t pattern_len = strlen(p);
        if( len_ < pattern_len ) return npos;

        /* KMP next array */
        std::vector<size_t> next(pattern_len, 0);
        if( pattern_len > 2 )
        {
            for( size_t pos = 2; pos != pattern_len; ++pos )
            {
                int base = next[pos-1];
                if( base == 0 )
                {
                    next[pos] = p[pos-1] == p[0] ? 1 : 0;
                }
                else
                {
                    if( p[pos-1] == p[base] ) next[pos] = base + 1;
                    else next[pos] = 0;
                }
            }
        }

        /* m : index in buf_, i : index in p */
        size_t m = 0, i = 0;
        while( m + i < len_ )
        {
            /* char in source */
            char sc = buf_[m+i];
            /* char in pattern */
            char pc = p[i];

            if( sc == pc )
            {
                ++i;
                if( i == pattern_len ) break;
            }
            else if( i == 0 )
            {
                ++m;
            }
            else
            {
                m += i - next[i];
                i = next[i];
            }
        }
#ifndef NDEBUG
        size_t pos = std::string(buf_, len_).find(p);
#endif
        if( m + i < len_ or ( m + i == len_ and i == pattern_len ) )
        {
            /* got match */
            assert( pos == m );
            return m;
        }
        else
        {
            assert( pos == string::npos );
            return npos;
        }
    }

    bool Slice::Equals( const Slice& rhs)
    {
        return len_ == rhs.len_ and memcmp(buf_, rhs.buf_, len_) == 0;
    }

    bool operator == (const Slice & lhs, const Slice & rhs )
    {
        return lhs.data() == rhs.data() and lhs.size() == rhs.size();
    }
}
