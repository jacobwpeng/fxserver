/*
 * =====================================================================================
 *       Filename:  HTTPResponse.cc
 *        Created:  16:31:34 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_response.h"
#include <boost/format.hpp>

HTTPResponse::HTTPResponse(unsigned status)
    :status_(status), major_version_(1), minor_version_(0), body_length_(0)
     ,zero_copy_fd_(-1), is_zero_copy_body_(false)
{

}

bool HTTPResponse::AddHeader(const string& key, const string& val)
{
    std::map<string, string>::const_iterator iter = headers_.find(key);
    if( iter != headers_.end() ) return false;
    else
    {
        headers_.insert( std::make_pair(key, val) );
        return true;
    }
}

string HTTPResponse::HTTPVersion() const
{
    return boost::str( 
            boost::format("HTTP/%u.%u") % major_version_ % minor_version_ );
}

char * HTTPResponse::GetBodyBuf(size_t len)
{
    body_.clear();
    body_.resize(len);
    body_length_ = len;
    return &body_[0];
}
