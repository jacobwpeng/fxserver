/*
 * =====================================================================================
 *       Filename:  http_server.cc
 *        Created:  16:57:28 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_server.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <glog/logging.h>

#include "http_response.h"

HTTPServer::HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num)
    :loop_(loop), addr_(bind_addr), thread_num_(thread_num)
{
    tcp_server_.reset( new TcpServer(loop, bind_addr) );
    codec_.reset( new HTTPCodec );
    codec_->set_request_callback( 
            boost::bind(&HTTPServer::OnRequest, this, _1, _2) );
    codec_->set_error_callback(
            boost::bind(&HTTPServer::OnParseHeaderError, this, _1, _2));
}

RetCode HTTPServer::InitModules()
{
    /* construction orders DOES matters!!! */
    request_validator_.reset( new HTTPModuleRequestValidator(this) );
    request_rewriter_.reset( new HTTPModuleRequestRewrite(this) );

    char * cwd = get_current_dir_name();
    file_accessor_.reset(new HTTPModuleFileAccessor(this, cwd) );
    LOG(INFO) << "htdocs path : " << cwd;
    free(cwd);
    return kOk;
}

RetCode HTTPServer::Run()
{
    RetCode ret = InitModules();
    if( ret != kOk ) return ret;

    tcp_server_->SetThreadNum(thread_num_);
    tcp_server_->set_new_connection_callback( 
                    boost::bind( &HTTPCodec::OnNewConnection, codec_.get(), _1) 
            );
    tcp_server_->set_read_callback( boost::bind(&HTTPCodec::OnMessage, codec_.get(), _1, _2) );
    tcp_server_->Start();

    return kOk;
}

void HTTPServer::OnRequest(TcpConnectionPtr conn, const HTTPRequest & r )
{
    LOG(INFO) << '\n' << r;
    HTTPRequestPtr req = boost::make_shared<HTTPRequest>(r);
    HTTPResponsePtr res;
    BOOST_FOREACH( const PreProcessingFunc& f, pre_processing_funcs_ )
    {
        res = f(req);
        if( res ) goto end;
    }

    res = boost::make_shared<HTTPResponse>(200);
    BOOST_FOREACH( const ProcessingFunc& f, processing_funcs_ )
    {
        f(req, res);
    }

    BOOST_FOREACH( const PostProcessingFunc& f, post_processing_funcs_ )
    {
        f(req, res);
    }

end:
    WriteResponseAndClose( conn, *res );

    //HTTPResponse res(200);

    //int fd = open("/tmp/404.html", O_RDONLY);
    //struct stat file_stat;
    //PCHECK( fstat(fd, &file_stat) == 0 ) << "fstat failed";

    //res.set_body_length( file_stat.st_size );
    //conn->ZeroCopyWrite( fd, 0, file_stat.st_size );

    ///* check if request format is valid */
    ///* set request, response pair context*/
    ///* check has path cache */

    //WriteResponseAndClose( conn, res );
}

void HTTPServer::OnParseHeaderError(TcpConnectionPtr conn, const HTTPResponse & res )
{
    WriteResponseAndClose(conn, res);
}

void HTTPServer::WriteResponseAndClose(TcpConnectionPtr conn, const HTTPResponse& res)
{
    conn->Write( codec_->EncodeResponse(res) );
    size_t body_len = res.body_length();
    if( body_len != 0u )
    {
        if( res.is_zero_copy_body() ) 
        {
            conn->ZeroCopyWrite( res.zero_copy_fd(), 0u, body_len );
        }
        else
        {
            conn->Write( res.GetConstBodyBuf(), body_len );
        }
    }
    /* make sure we write response to the socket buffer before close connection */
    conn->set_write_done_callback( 
                    boost::bind( &HTTPServer::WriteResponseDone, this, 
                                        TcpConnectionWeakPtr(conn) ) );
}

void HTTPServer::WriteResponseDone( TcpConnectionWeakPtr weak_conn )
{
    if( TcpConnectionPtr conn = weak_conn.lock() )
    {
        /* response has been write to system buf, now we close connection */
        conn->ActiveClose();
    }
}
