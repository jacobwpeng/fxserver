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
            static const size_t npos = (size_t)(-1);
            static string to_string( const Slice& slice );

        public:
            Slice();
            Slice(const char * buf, size_t len);
            explicit Slice(const string& str);

            Slice(const Slice& slice);

            void clear();
            void swap(Slice& rhs);
            void assign(const Slice& );
            void assign(const char* buf, size_t len);
            void assign(const string& str);
            Slice subslice(size_t pos = 0, size_t len = npos);

            bool empty() const { return buf_ == NULL; }
            const char * data() const { return buf_; }
            size_t size() const { return len_; }
            /* KMP style find using next array */
            size_t find(const char* p) const;

            /* compare the internal buf content */
            bool Equals( const Slice& rhs);

        private:
            /* KMP style find using DFA */
            size_t InternalFind( const char * p ) const;

        private:
            const char * buf_;
            size_t len_;
    };

    /* just compare the internal buf and len */
    bool operator == (const Slice & lhs, const Slice & rhs );
}

#endif   /* ----- #ifndef __FX_SLICE__  ----- */
