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

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>

#include "fx_slice.h"
#include "http_response.h"
#include "http_utilities.h"

const char* HTTPCodec::HEADER_SEP = "\r\n";
const char* HTTPCodec::HEADER_BODY_SEP = "\r\n\r\n";

typedef boost::shared_ptr<HTTPRequestParsingState> HTTPRequestParsingStatePtr;
using fx::Slice;
using fx::TcpConnection;

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

void HTTPCodec::OnNewConnection( TcpConnectionPtr conn )
{
    boost::shared_ptr<HTTPRequestParsingState> state = boost::make_shared<HTTPRequestParsingState>();
    state->status = kInit;
    state->func = boost::bind( &HTTPCodec::StartParsing, this, _1, _2 );
    conn->set_context( state );
}

void HTTPCodec::OnMessage(TcpConnectionPtr conn, Buffer * buf)
{
    TcpConnection::Context ctx = conn->context();
    HTTPRequestParsingStatePtr state = boost::any_cast<HTTPRequestParsingStatePtr>(ctx);

    ParseResult res = kParseError;

    while( state->status != kParsingDone )
    {
        res = state->func( state.get(), buf );
        if( res != kParseOK ) break;
    }

    if( res == kParseError and ecb_ )
    {
        assert( state->res );                   /* must got a response */
        ecb_( conn, state->res.get() );         /* call error callback with */
    }
    else if( res == kParseNeedMore )
    {

    }
    else
    {
        /* parse header done */
        conn->clear_context();
        if( rcb_ ) rcb_( conn, state->req );
    }
}

string HTTPCodec::EncodeResponse( const HTTPResponse& res )
{
    string reply;
    bool has_body = res.body_length() != 0u;
    reply += boost::str( boost::format("%s %u %s%s") % res.HTTPVersion() 
                                                    % res.status_ 
                                                    % status_to_reason_[res.status_] 
                                                    % HEADER_SEP );

    reply += boost::str( boost::format("Date: %s%s") % Rfc1123TimeNow() % HEADER_SEP );
    reply += boost::str( boost::format("Server: FX Server/0.0.1%s") % HEADER_SEP );
    reply += boost::str( boost::format("Content-Type: text/html%s") % HEADER_SEP );
    reply += boost::str( boost::format("Connection: close%s") % HEADER_SEP );

    if( has_body )
    {
        reply += boost::str( boost::format("Content-Length: %u%s") % res.body_length() 
                                                                % HEADER_SEP );
        reply += HEADER_SEP;                    /* for response body sep */
    }

    return reply;
}

ParseResult HTTPCodec::StartParsing( HTTPRequestParsingState * state, Buffer* buf )
{
    assert( state->status == kInit );
    assert( buf != NULL );
    if( buf->BytesToRead() != 0u )
    {
        state->status = kParsingRequestLine;
        state->func = boost::bind( &HTTPCodec::ReadRequestLine, this, _1, _2);
        return kParseOK;
    }
    return kParseNeedMore;
}

ParseResult HTTPCodec::ReadRequestLine( HTTPRequestParsingState * state, Buffer* buf )
{
    assert( state->status == kParsingRequestLine );
    assert( buf != NULL );

    Slice msg( buf->ReadBegin(), buf->BytesToRead() );
    size_t pos = msg.find( HTTPCodec::HEADER_SEP );

    if( pos == Slice::npos ) return kParseNeedMore;
    Slice request_line = msg.subslice(0, pos);

    StringList parts;
    SplitString( Slice::to_string(request_line), " ", &parts );
    if( parts.size() != 3 )
    {
        HTTPResponse res(400);                  /* Bad Request */
        state->res.reset( res );
        return kParseError;
    }

    const string& request_type = parts[0];
    state->req.set_request_type( request_type );
    state->req.set_request_path( parts[1] );

    const string& request_version = parts[2];

    size_t s = request_version.find( '/' );
    if( s == string::npos or request_version.substr(0, s) != "HTTP" )
    {
        HTTPResponse res(400);                  /* Bad Request */
        state->res.reset( res );
        return kParseError;
    }

    size_t e = request_version.find( '.', s + 1 );
    if( e == string::npos or e == request_version.size() )
    {
        HTTPResponse res(400);                  /* Bad Request */
        state->res.reset( res );
        return kParseError;
    }

    /* TODO : overflow? */
    string major_version_str = request_version.substr( s+1, e - s - 1);
    string minor_version_str = request_version.substr( e+1 );
    try
    {
        state->req.set_major_version( boost::lexical_cast<unsigned>(major_version_str) );
        state->req.set_minor_version( boost::lexical_cast<unsigned>(minor_version_str) );
    }
    catch( boost::bad_lexical_cast & e )
    {
        LOG(WARNING) << "Cannot parse http version, request_version = [" << request_version << "]";
        HTTPResponse res(400);                  /* Bad Request */
        state->res.reset( res );
        return kParseError;
    }

    buf->ConsumeBytes( request_line.size() + HTTPCodec::HEADER_SEP_LEN );
    state->status = kParsingRequestHeader;
    state->func = boost::bind( &HTTPCodec::ReadRequestHeader, this, _1, _2);
    return kParseOK;
}

ParseResult HTTPCodec::ReadRequestHeader( HTTPRequestParsingState * state, Buffer* buf)
{
    assert( state->status == kParsingRequestHeader );
    assert( buf != NULL );

    Slice msg( buf->ReadBegin(), buf->BytesToRead() );
    size_t pos = msg.find( HTTPCodec::HEADER_SEP );
    if( pos == Slice::npos ) return kParseNeedMore;
    else if( pos == 0u )
    {
        /* Blank line, which means header ends here*/
        /* see if request got any body */
        boost::optional<string> content_length_val = state->req.GetHeader("content-length");
        if( content_length_val )
        {
            unsigned content_length = 0;
            try
            {
                content_length = boost::lexical_cast<unsigned>( content_length_val.get() );
            }
            catch( boost::bad_lexical_cast & e )
            {
                LOG(WARNING) << "lexical_cast failed, content_length_val.get() = " << content_length_val.get();
                HTTPResponse res(400);                  /* Bad Request */
                state->res.reset( res );
                return kParseError;
            }
            state->status = kParsingRequestBody;
            state->req.set_body_length( content_length );
            state->func = boost::bind( &HTTPCodec::ReadRequestBody, this, _1, _2 );
        }
        else
        {
            state->status = kParsingDone;
            state->func = NULL;
        }
        buf->ConsumeBytes( HTTPCodec::HEADER_SEP_LEN ); /* just CRLF */
        return kParseOK;
    }
    else
    {
        /* ordinary header line */
        Slice line = msg.subslice( 0, pos );
        StringList kv;
        SplitString( Slice::to_string(line), ": ", &kv );
        if( kv.size() != 2 )
        {
            HTTPResponse res(400);                  /* Bad Request */
            state->res.reset( res );
            return kParseError;
        }
        state->req.set_header_length( state->req.header_length() + pos + HTTPCodec::HEADER_SEP_LEN );
        buf->ConsumeBytes( pos + HTTPCodec::HEADER_SEP_LEN );
        boost::algorithm::to_lower(kv[0]);      /* header key is case-insensitive */
        state->req.AddHeader( kv[0], kv[1] );
        return kParseOK;
    }
}

ParseResult HTTPCodec::ReadRequestBody( HTTPRequestParsingState * state, Buffer* buf)
{
    assert( state->status == kParsingRequestBody );
    assert( buf != NULL );
    unsigned body_length = state->req.body_length();
    assert( body_length != 0 );                 /* if body_length == 0, we should never be here */

    if( buf->BytesToRead() < body_length )
    {
        return kParseNeedMore;
    }
    else
    {
        state->req.set_body( buf->ReadBegin(), body_length );
        buf->ConsumeBytes( body_length );
        state->status = kParsingDone;
        state->func = NULL;
        return kParseOK;
    }
}
