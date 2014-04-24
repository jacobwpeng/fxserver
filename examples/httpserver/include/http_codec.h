/*
 * =====================================================================================
 *       Filename:  http_codec.h
 *        Created:  16:39:49 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_CODEC_H__
#define  __HTTP_CODEC_H__

#include <map>
#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/optional/optional_fwd.hpp>

#include "fx_buffer.h"
#include "fx_tcp_connection.h"
#include "http_request.h"

class HTTPRequest;
class HTTPResponse;
using std::map;
using std::string;
using fx::Buffer;
using fx::TcpConnectionPtr;

typedef std::vector<string> StringList;

class HTTPCodec
{
    public:
        typedef boost::function< void(TcpConnectionPtr conn, const HTTPRequest& req) > RequestCallback;

    public:
        HTTPCodec();

        void set_request_callback( RequestCallback rcb ) { rcb_ = rcb; }
        void OnMessage(TcpConnectionPtr conn, Buffer * buf);
        string EncodeResponse( const HTTPResponse & res );

    private:
        OptionalHTTPRequest TryParseHTTPHeader( const char* buf, size_t len );
        void ParseHTTPRequestLine( const string& line, HTTPRequest * req );
        void ParseHTTPRequestHeaders( const StringList & headers, HTTPRequest * req );

    private:
        static const char * HEADER_SEP;
        static const char * HEADER_BODY_SEP;
        map<unsigned, string> status_to_reason_;
        RequestCallback rcb_;

};

#endif   /* ----- #ifndef __HTTP_CODEC_H__----- */
