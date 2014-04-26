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

namespace fx
{
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
}
