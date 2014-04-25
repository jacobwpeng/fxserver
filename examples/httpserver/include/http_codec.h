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
#include "http_request_parsing_state.h"

class HTTPRequest;
class HTTPResponse;
struct HTTPRequestParsingState;
using std::map;
using std::string;
using fx::Buffer;
using fx::TcpConnectionPtr;

typedef std::vector<string> StringList;

class HTTPCodec
{
    public:
        typedef boost::function< void(TcpConnectionPtr conn, const HTTPRequest& req) > RequestCallback;
        typedef boost::function< void(TcpConnectionPtr conn, const HTTPResponse& res) > ErrorCallback;

    public:
        HTTPCodec();

        void set_request_callback( const RequestCallback & rcb ) { rcb_ = rcb; }
        void set_error_callback( const ErrorCallback & ecb ) { ecb_ = ecb; }
        void OnNewConnection( TcpConnectionPtr conn );
        void OnMessage(TcpConnectionPtr conn, Buffer * buf);
        string EncodeResponse( const HTTPResponse & res );

    private:
        /* TODO : use const shared_ptr? */
        ParseResult StartParsing( HTTPRequestParsingState * state, Buffer* buf);
        ParseResult ReadRequestLine( HTTPRequestParsingState * state, Buffer* buf);
        ParseResult ReadRequestHeader( HTTPRequestParsingState * state, Buffer* buf);
        ParseResult ReadRequestBody( HTTPRequestParsingState * state, Buffer* buf);

    private:
        static const char CR = '\r';
        static const char LF = '\n';
        static const char * HEADER_SEP;
        static const char * HEADER_BODY_SEP;
        static const size_t HEADER_SEP_LEN = 2;
        static const size_t HEADER_BODY_SEP_LEN = 4;
        map<unsigned, string> status_to_reason_;
        RequestCallback rcb_;
        ErrorCallback ecb_;

};

#endif   /* ----- #ifndef __HTTP_CODEC_H__----- */
