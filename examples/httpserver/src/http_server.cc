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
#include <glog/logging.h>

#define UNUSED(x) (void)x

HTTPServer::HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num)
    :loop_(loop), addr_(bind_addr), thread_num_(thread_num)
{
    tcp_server_.reset( new TcpServer(loop, bind_addr) );
    codec_.reset( new HTTPCodec );
    codec_->set_request_callback( boost::bind(&HTTPServer::OnRequest, this, _1, _2) );
    codec_->set_error_callback( boost::bind( &HTTPServer::OnParseHeaderError, this, _1, _2) );
}

void HTTPServer::Run()
{
    tcp_server_->SetThreadNum(thread_num_);
    tcp_server_->set_new_connection_callback( boost::bind( &HTTPCodec::OnNewConnection, codec_.get(), _1) );
    tcp_server_->set_read_callback( boost::bind(&HTTPCodec::OnMessage, codec_.get(), _1, _2) );
    tcp_server_->Start();
}

void HTTPServer::OnRequest(TcpConnectionPtr conn, const HTTPRequest & req )
{
    LOG(INFO) << '\n' << req;
    HTTPResponse res(200);

    conn->Write( codec_->EncodeResponse(res) );
    conn->ActiveClose();
}

void HTTPServer::OnParseHeaderError(TcpConnectionPtr conn, const HTTPResponse & res )
{
    conn->Write( codec_->EncodeResponse(res) );
    conn->ActiveClose();
}
