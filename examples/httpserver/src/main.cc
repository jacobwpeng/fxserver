/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  15:27:51 Apr 23, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <map>
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_tcp_server.h"
#include "fx_tcp_connection.h"
#include "fx_net_address.h"

using std::map;
using std::vector;
using std::string;
using boost::optional;
using fx::EventLoop;
using fx::TcpServer;
using fx::NetAddress;
using fx::TcpConnectionPtr;
using fx::Buffer;

#define UNUSED(x) (void)x

typedef vector< string > StringList;

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
}

class HTTPRequest
{
    public:
        HTTPRequest()
            :len_(0), major_version_(0), minor_version_(0)
        {
        }

        void set_header_length( unsigned len ) { len_ = len; }
        void set_major_version( unsigned major_version ) { major_version_ = major_version; }
        void set_minor_version( unsigned minor_version ) { minor_version_ = minor_version; }
        void set_request_type( const string& type ) { request_type_ = type; }
        void set_request_path( const string& path ) { request_path_ = path; }

        bool AddHeader(const string& key, const string& val)
        {
            map<string, string>::const_iterator iter = headers_.find(key);
            if( iter != headers_.end() ) return false;
            else
            {
                headers_.insert( std::make_pair(key, val) );
                return true;
            }
        }

        string HTTPVersion() const
        {
            return boost::str( boost::format("%u.%u") % major_version_ % minor_version_ );
        }

        optional<string> GetHeader(const string& header_name) const
        {
            optional<string> res;
            map<string, string>::const_iterator iter = headers_.find(header_name);
            if( iter != headers_.end() )
            {
                res.reset( iter->second );
            }

            return res;
        }
        unsigned HeaderLength() const { return len_; }

    private:
        unsigned len_;
        unsigned major_version_;
        unsigned minor_version_;
        string request_type_;
        string request_path_;
        map<string, string> headers_;
};

class HTTPCodec
{
    public:
        typedef boost::function< void(const HTTPRequest& req) > RequestCallback;

    private:
        typedef optional<HTTPRequest> OptionalHTTPRequest;

    public:
        void set_request_callback( RequestCallback rcb )
        {
            rcb_ = rcb;
        }

        void OnMessage(TcpConnectionPtr conn, Buffer * buf)
        {
            /* parse the http header only */
            (void)conn;
            OptionalHTTPRequest optional_req = TryParseHTTPHeader( buf->ReadBegin(), buf->BytesToRead() );
            if( optional_req and rcb_ )
            {
                const HTTPRequest & req = optional_req.get();
                buf->ConsumeBytes( req.HeaderLength() ); /* we just read that long */
                rcb_( optional_req.get() );
            }
        }

    private:
        OptionalHTTPRequest TryParseHTTPHeader( const char* buf, size_t len )
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
                PCHECK( headers.size() > 1 ) << "headers.size() = " << headers.size();
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

        void ParseHTTPRequestLine( const string& line, HTTPRequest * req )
        {
            StringList parts;
            detail::SplitString( line, " ", &parts );
            PCHECK( parts.size() == 3 ) << "line = [" << line << "], size() = " << parts.size();
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

        void ParseHTTPRequestHeaders( const StringList & headers, HTTPRequest * req )
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

    private:
        static const char * HEADER_SEP;
        static const char * HEADER_BODY_SEP;
        RequestCallback rcb_;
};

const char* HTTPCodec::HEADER_SEP = "\r\n";
const char* HTTPCodec::HEADER_BODY_SEP = "\r\n\r\n";

class HTTPServer
{
    public:
        HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num)
            :loop_(loop), addr_(bind_addr), thread_num_(thread_num)
        {
            tcp_server_.reset( new TcpServer(loop, bind_addr) );
            codec_.reset( new HTTPCodec );
            codec_->set_request_callback( boost::bind(&HTTPServer::OnRequest, this, _1) );
        }

        void Run()
        {
            tcp_server_->SetThreadNum(thread_num_);
            tcp_server_->set_read_callback( boost::bind(&HTTPCodec::OnMessage, codec_.get(), _1, _2) );
            tcp_server_->Start();
        }

        void OnRequest( const HTTPRequest & req )
        {
            LOG(INFO) << "request version : " << req.HTTPVersion();
        }

    private:
        EventLoop * loop_;
        NetAddress addr_;
        size_t thread_num_;
        boost::scoped_ptr<TcpServer> tcp_server_;
        boost::scoped_ptr<HTTPCodec> codec_;
};

int main(int argc, char * argv[])
{
    UNUSED(argc);
    google::InitGoogleLogging(argv[0]);
    EventLoop loop;
    HTTPServer http_server(&loop, NetAddress("0.0.0.0", 9026), 8 );

    http_server.Run();
    loop.Run();
    return 0;
}
