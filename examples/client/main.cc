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
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_tcp_connection.h"
#include "fx_tcp_client.h"
#include "fx_buffer.h"

using namespace fx;

void OnConnected( TcpConnectionPtr conn )
{
    conn->Write( boost::lexical_cast<std::string>(1) );
}

void OnMessage( TcpConnectionPtr conn, Buffer * buf )
{
    std::string msg( buf->ReadAndClear() );
    int idx = boost::lexical_cast<int>(msg);
    LOG(INFO) << "from server [" << idx<< "]";
    conn->Write( boost::lexical_cast<std::string>(++idx) );
}

int main(int argc, char * argv[])
{
    if( argc != 3 )
    {
        return -1;
    }

    int port = boost::lexical_cast<int>( argv[2] );

    google::InitGoogleLogging(argv[0]);
    EventLoop loop;

    TcpClient client(&loop);
    client.set_connected_callback( OnConnected );
    client.set_read_callback( OnMessage );

    client.ConnectTo(argv[1], port);

    loop.Run();
    return 0;
}
