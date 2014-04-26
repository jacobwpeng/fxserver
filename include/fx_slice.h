/*
 * =====================================================================================
 *
 *       Filename:  fx_slice.h
 *        Created:  04/26/14 18:38:18
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  raw bytes wrapper
 *
 * =====================================================================================
 */

#ifndef  __FX_SLICE__
#define  __FX_SLICE__

#include <string>

using std::string;

namespace fx
{
    class Slice
    {
        public:
            Slice();
            Slice(const char * buf, size_t len);
            Slice(const string& str);

            void clear();
            void swap(Slice& rhs);

            bool empty() const { return buf_ == NULL; }
            const char * data() const { return buf_; }
            size_t size() const { return len_; }

        private:
            const char * buf_;
            size_t len_;
    };
}

#endif   /* ----- #ifndef __FX_SLICE__  ----- */
