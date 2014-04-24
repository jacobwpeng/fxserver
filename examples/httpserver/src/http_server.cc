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
#include "http_response.h"
#include <boost/bind.hpp>

#define UNUSED(x) (void)x

HTTPServer::HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num)
    :loop_(loop), addr_(bind_addr), thread_num_(thread_num)
{
    tcp_server_.reset( new TcpServer(loop, bind_addr) );
    codec_.reset( new HTTPCodec );
    codec_->set_request_callback( boost::bind(&HTTPServer::OnRequest, this, _1, _2) );
}

void HTTPServer::Run()
{
    tcp_server_->SetThreadNum(thread_num_);
    tcp_server_->set_read_callback( boost::bind(&HTTPCodec::OnMessage, codec_.get(), _1, _2) );
    tcp_server_->Start();
}

void HTTPServer::OnRequest(TcpConnectionPtr conn, const HTTPRequest & req )
{
    UNUSED(req);
    HTTPResponse res;
    res.set_status( 200 );
    res.SetHTTPVersion( 1, 1 );

    conn->Write( codec_->EncodeResponse(res) );
    conn->ActiveClose();
}
