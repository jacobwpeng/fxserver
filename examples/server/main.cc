/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  11:30:15 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <string>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_acceptor.h"
#include "fx_blocking_queue.hpp"
#include "fx_tcp_server.h"
#include "fx_tcp_connection.h"

using std::string;
using namespace fx;

void OnNewConnection( TcpConnectionPtr conn )
{
    TcpConnection::Context ctx( conn->fd() );
    LOG(INFO) << "New Connection, fd = " << conn->fd();
    conn->set_context( ctx );
}

void OnMessage( TcpConnectionPtr conn, Buffer * buf )
{
    string msg( buf->ReadAndClear() );

    LOG(INFO) << "msg[" << msg << "]";

    conn->Write( msg );
}

void OnConnectionClosed( TcpConnectionPtr conn )
{
    LOG(INFO) << "Close Connection, fd = " << boost::any_cast<int>(conn->context());
}

void ThreadFunc(int port)
{
    EventLoop loop;

    Acceptor acceptor(&loop);
    acceptor.BindOrAbort( "0.0.0.0", port );

    loop.RunInLoop( boost::bind( &Acceptor::Listen, &acceptor) );

    loop.Run();
}

int main(int argc, char * argv[])
{
    if( argc != 2 )
    {
        return -1;
    }
    google::InitGoogleLogging(argv[0]);
    const size_t thread_count = 4;

    EventLoop loop;

    int port = boost::lexical_cast<int>( argv[1] );

    TcpServer s( &loop, "0.0.0.0", port );
    s.SetThreadNum( thread_count );
    s.set_read_callback( OnMessage );
    s.set_new_connection_callback( OnNewConnection );
    s.set_close_connection_callback( OnConnectionClosed );
    s.Start();

    loop.Run();
    return 0;
}
