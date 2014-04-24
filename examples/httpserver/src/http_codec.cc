/*
 * =====================================================================================
 *       Filename:  http_codec.cc
 *        Created:  16:43:26 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_codec.h"
#include "http_response.h"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

namespace detail
{
    void SplitString( const string& src, const string& sep, StringList * res)
    {
        typedef boost::split_iterator<string::const_iterator> StringSplitIterator;
        for (StringSplitIterator iter = boost::make_split_iterator(src, boost::first_finder(sep, boost::is_equal()));
                                 iter != StringSplitIterator();
                                 ++iter
            )
        {
            res->push_back( boost::copy_range<string>(*iter) );
        }
    }

    string Rfc1123_DateTimeNow()
    {
        std::stringstream ss;
        using namespace boost::posix_time;
        time_facet *facet = new time_facet("%a, %d %b %Y %H:%M:%S GMT");
        ss.imbue( std::locale( ss.getloc(), facet ) );
        ss << second_clock::universal_time();
        return ss.str();
    }
}

const char* HTTPCodec::HEADER_SEP = "\r\n";
const char* HTTPCodec::HEADER_BODY_SEP = "\r\n\r\n";

HTTPCodec::HTTPCodec()
{
    status_to_reason_[301] = "Moved Permanently";
    status_to_reason_[302] = "Found";
    status_to_reason_[303] = "See Other";
    status_to_reason_[304] = "Not Modified";
    status_to_reason_[305] = "Use Proxy";
    status_to_reason_[307] = "Temporary Redirect";
    status_to_reason_[400] = "Bad Request";
    status_to_reason_[401] = "Unauthorized";
    status_to_reason_[402] = "Payment Required";
    status_to_reason_[403] = "Forbidden";
    status_to_reason_[404] = "Not Found";
    status_to_reason_[405] = "Method Not Allowed";
    status_to_reason_[406] = "Not Acceptable";
    status_to_reason_[407] = "Proxy Authentication Required";
    status_to_reason_[408] = "Request Time-out";
    status_to_reason_[409] = "Conflict";
    status_to_reason_[410] = "Gone";
    status_to_reason_[411] = "Length Required";
    status_to_reason_[412] = "Precondition Failed";
    status_to_reason_[413] = "Request Entity Too Large";
    status_to_reason_[414] = "Request-URI Too Large";
    status_to_reason_[415] = "Unsupported Media Type";
    status_to_reason_[416] = "Requested range not satisfiable";
    status_to_reason_[417] = "Expectation Failed";
    status_to_reason_[500] = "Internal Server Error";
    status_to_reason_[501] = "Not Implemented";
    status_to_reason_[502] = "Bad Gateway";
    status_to_reason_[503] = "Service Unavailable";
    status_to_reason_[504] = "Gateway Time-out";
    status_to_reason_[505] = "HTTP Version not supported";
}

void HTTPCodec::OnMessage(TcpConnectionPtr conn, Buffer * buf)
{
    /* parse the http header only */
    OptionalHTTPRequest optional_req = TryParseHTTPHeader( buf->ReadBegin(), buf->BytesToRead() );
    if( !optional_req ) return;

    const HTTPRequest & req = optional_req.get();
    buf->ConsumeBytes( req.HeaderLength() ); /* we just read that long */
    if( rcb_ ) rcb_( conn, optional_req.get() );
}

string HTTPCodec::EncodeResponse( const HTTPResponse& res )
{
    string reply;
    reply += boost::str( boost::format("%s %u %s%s") % res.HTTPVersion() % res.status_ % status_to_reason_[res.status_] % HEADER_SEP );
    reply += boost::str( boost::format("Date: %s%s") % detail::Rfc1123_DateTimeNow() % HEADER_SEP );
    reply += boost::str( boost::format("Server: FX Server Ver 0.0.1%s") % HEADER_SEP );
    reply += boost::str( boost::format("Content-Type:text/html; charset=iso-8859-1%s") % HEADER_SEP );
    reply += boost::str( boost::format("Connection: close%s") % HEADER_SEP );

    return reply;
}

OptionalHTTPRequest HTTPCodec::TryParseHTTPHeader( const char* buf, size_t len )
{
    string content(buf, len);           /* TODO : avoid copy */
    OptionalHTTPRequest optional_req;

    typedef boost::iterator_range<string::iterator> StringIteratorRange;
    StringIteratorRange iter = boost::algorithm::find_first(content, "\r\n\r\n"); 

    /* find the header ending '\r\n\r\n' */
    if( iter != StringIteratorRange() )
    {
        /* read a complete HTTP header */
        unsigned header_len = std::distance( content.begin(), iter.begin() );
        string header( buf, header_len );
        StringList headers;
        detail::SplitString( header, HEADER_SEP, &headers );
        assert( headers.size() > 1 );
        /* now we just consider the right HTTP header format */
        assert( not headers.empty() );

        HTTPRequest req;
        req.set_header_length( header_len );
        ParseHTTPRequestLine( headers[0], &req );
        ParseHTTPRequestHeaders( headers, &req );

        optional_req.reset( req );
    }

    return optional_req;
}

void HTTPCodec::ParseHTTPRequestLine( const string& line, HTTPRequest * req )
{
    StringList parts;
    detail::SplitString( line, " ", &parts );
    assert( parts.size() == 3 );
    req->set_request_type( parts[0] );
    req->set_request_path( parts[1] );

    size_t s = parts[2].find( '/' );
    assert( s + 1 < parts[2].length() );
    size_t e = parts[2].find( '.', s + 1 );
    assert( e + 2 == parts[2].length() );
    assert( e - s == 2 );
    req->set_major_version( parts[2][s+1] - '0' );
    req->set_minor_version( parts[2][e+1] - '0' );
}

void HTTPCodec::ParseHTTPRequestHeaders( const StringList & headers, HTTPRequest * req )
{
    assert( headers.size() > 1u );
    StringList kv;
    for( size_t idx = 1; idx != headers.size(); ++idx )
    {
        detail::SplitString( headers[idx], ": ", &kv );
        assert( kv.size() == 2u );
        req->AddHeader( kv[0], kv[1] );
        kv.clear();
    }
}
