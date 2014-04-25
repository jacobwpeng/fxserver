/*
 * =====================================================================================
 *       Filename:  http_server.h
 *        Created:  16:52:15 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_SERVER_H__
#define  __HTTP_SERVER_H__

#include "fx_event_loop.h"
#include "fx_net_address.h"
#include "fx_tcp_connection.h"
#include "fx_tcp_server.h"
#include "http_codec.h"

#include <boost/scoped_ptr.hpp>

class HTTPRequest;
class HTTPResponse;
using fx::TcpConnectionPtr;
using fx::EventLoop;
using fx::NetAddress;
using fx::TcpServer;

class HTTPServer
{
    public:
        HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num);
        void Run();
        void OnRequest(TcpConnectionPtr conn, const HTTPRequest & req );
        void OnParseHeaderError(TcpConnectionPtr conn, const HTTPResponse & res );

    private:
        EventLoop * loop_;
        NetAddress addr_;
        size_t thread_num_;
        boost::scoped_ptr<TcpServer> tcp_server_;
        boost::scoped_ptr<HTTPCodec> codec_;
};

#endif   /* ----- #ifndef __HTTP_SERVER_H__----- */
