/*
 * =====================================================================================
 *       Filename:  http_response.h
 *        Created:  16:33:19 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_RESPONSE_H__
#define  __HTTP_RESPONSE_H__

#include <string>
#include <vector>
#include <map>
#include "fx_slice.h"
#include "http_defines.h"

using std::string;
using fx::Slice;

class HTTPResponse
{
    public:
        HTTPResponse(unsigned status);

        bool AddHeader(const string& key, const string& val);
        string HTTPVersion() const;
        size_t body_length() const { return body_length_; }

        char * GetBodyBuf(size_t len);
        const char * GetConstBodyBuf() const { return &body_[0]; }

        /* for zero copy write */
        bool is_zero_copy_body() const { return is_zero_copy_body_; }
        int zero_copy_fd() const
        {
            assert( is_zero_copy_body_ );
            return zero_copy_fd_;
        }

        void set_zero_copy_write_callback( const ZeroCopyWriteCallback& cb )
        { zero_copy_write_cb_ = cb; }

        void set_zero_copy_fd(int fd)
        { zero_copy_fd_ = fd; is_zero_copy_body_ = true; }

        ZeroCopyWriteCallback zero_copy_write_callback() const
        { return zero_copy_write_cb_; }

    private:
        friend class HTTPCodec;

    private:
        unsigned status_;
        unsigned major_version_;
        unsigned minor_version_;
        size_t body_length_;
        int zero_copy_fd_;

        bool is_zero_copy_body_;
        bool is_body_gzipped_;
        std::vector<char> body_;
        std::map<string, string> headers_;
        ZeroCopyWriteCallback zero_copy_write_cb_;
};

#endif   /* ----- #ifndef __HTTP_RESPONSE_H__----- */
