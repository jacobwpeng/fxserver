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

#include <boost/bind.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_tcp_connection.h"
#include "fx_tcp_client.h"
#include "fx_buffer.h"

using namespace fx;

void OnConnected( TcpConnectionPtr conn )
{
    conn->Write( "1024" );
}

void OnMessage( TcpConnectionPtr conn, Buffer * buf )
{
    conn->Write( buf->ReadAndClear() );
}

int main(int argc, char * argv[])
{
    (void)argc;
    google::InitGoogleLogging(argv[0]);
    EventLoop loop;

    TcpClient client(&loop);
    client.set_connected_callback( OnConnected );
    client.set_read_callback( OnMessage );

    client.ConnectTo("127.0.0.1", 9026);

    loop.Run();
    return 0;
}
