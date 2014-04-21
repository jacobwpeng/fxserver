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
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_acceptor.h"
#include "fx_blocking_queue.hpp"
#include "fx_tcp_server.h"
#include "fx_tcp_connection.h"
#include "fx_net_address.h"

using std::string;
using namespace fx;

int timeout = 5 * 1000;                        /* milliseconds */

typedef boost::weak_ptr<TcpConnection> TcpConnectionWeakPtr;

void SayGoodbye( TcpConnectionWeakPtr weak_conn )
{
    TcpConnectionPtr conn = weak_conn.lock();
    if( conn and not conn->closed() )
    {
        conn->Write( "You give nothing!!!!!!\n" );
        LOG(INFO) << "active close connection";
        conn->ActiveClose();
    }
}

void OnNewConnection( TcpConnectionPtr conn )
{
    LOG(INFO) << "New Connection, peer addr = " << conn->PeerAddr();

    TcpConnectionWeakPtr weak_conn( conn );
    TimerId id = conn->loop()->RunAfter( timeout, boost::bind( SayGoodbye, weak_conn ) );
    conn->set_context( id );
}

void OnMessage( TcpConnectionPtr conn, Buffer * buf )
{
    string msg( buf->ReadAndClear() );

    LOG(INFO) << "localaddr = " << conn->LocalAddr() << ", peeraddr = " << conn->PeerAddr() << ", msg[" << msg << "]";

    conn->Write( msg );

    TimerId id = boost::any_cast<TimerId>( conn->context() );

    conn->loop()->RemoveTimer(id);
    id = conn->loop()->RunAfter( timeout, boost::bind( SayGoodbye, TcpConnectionWeakPtr(conn) ) );
    conn->set_context( id );
}

void OnConnectionClosed( TcpConnectionPtr conn )
{
    TcpConnection::Context ctx = conn->context();
    if( not ctx.empty() )
    {
        TimerId id = boost::any_cast<TimerId>( ctx );
        conn->loop()->RemoveTimer(id);
        conn->set_context( TcpConnection::Context() );
    }
    LOG(INFO) << "Connection closed, fd = " << conn->fd();
}

void ThreadFunc(int port)
{
    EventLoop loop;

    Acceptor acceptor(&loop);
    acceptor.BindOrAbort( "0.0.0.0", port );

    loop.RunInLoop( boost::bind( &Acceptor::Listen, &acceptor) );

    loop.Run();
}

void Callback( int idx )
{
    LOG(INFO) << "idx = " << idx;
}

int main(int argc, char * argv[])
{
    google::InitGoogleLogging(argv[0]);
    if( argc != 3 )
    {
        return -1;
    }
    EventLoop loop;

    int port = boost::lexical_cast<int>( argv[1] );
    size_t thread_count = boost::lexical_cast<size_t>( argv[2] );

    TcpServer s( &loop, "0.0.0.0", port );
    s.SetThreadNum( thread_count );
    s.set_read_callback( OnMessage );
    s.set_new_connection_callback( OnNewConnection );
    s.set_close_connection_callback( OnConnectionClosed );
    s.Start();

    loop.Run();
    return 0;
}
