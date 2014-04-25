/*
 * =====================================================================================
 *       Filename:  http_request.cc
 *        Created:  16:26:02 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_request.h"
#include <ostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

HTTPRequest::HTTPRequest()
    :header_length_(0), body_length_(0), major_version_(0), minor_version_(0)
{

}

bool HTTPRequest::AddHeader(const string& key, const string& val)
{
    map<string, string>::const_iterator iter = headers_.find(key);
    if( iter != headers_.end() ) return false;
    else
    {
        headers_.insert( std::make_pair(key, val) );
        return true;
    }
}

string HTTPRequest::HTTPVersion() const
{
    return boost::str( boost::format("HTTP/%u.%u") % major_version_ % minor_version_ );
}

StringList HTTPRequest::HeaderNames() const
{
    StringList keys;
    boost::copy( headers_ | boost::adaptors::map_keys, std::back_inserter(keys));
    return keys;
}

optional<string> HTTPRequest::GetHeader(const string& header_name) const
{
    optional<string> res;
    map<string, string>::const_iterator iter = headers_.find(header_name);
    if( iter != headers_.end() )
    {
        res.reset( iter->second );
    }
    return res;
}

ostream & operator<< (ostream& os, const HTTPRequest & req )
{
    os << "request type: " << req.request_type() << '\n';
    os << "request path: " << req.request_path() << '\n';
    os << "http version: " << req.HTTPVersion() << '\n';
    os << "header length: " << req.header_length() << '\n';
    os << "body length: " << req.body_length() << '\n';
    BOOST_FOREACH( const string& key, req.HeaderNames() )
    {
        os << key << ": " << req.GetHeader(key).get() << '\n';
    }
    return os;
}
