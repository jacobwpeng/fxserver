/*
 * =====================================================================================
 *
 *       Filename:  fx_tcp_client.cc
 *        Created:  04/13/2014 03:42:17 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_tcp_client.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <glog/logging.h>

#include "fx_connector.h"
#include "fx_event_loop.h"
#include "fx_tcp_connection.h"

namespace fx
{
    TcpClient::TcpClient(EventLoop * loop)
        :loop_(loop)
    {
        connector_.reset( new Connector(loop) );
        connector_->set_connect_callback( boost::bind( &TcpClient::OnConnect, this, _1, _2) );
    }

    TcpClient::~TcpClient()
    {

    }

    void TcpClient::ConnectTo( const std::string& addr, int port )
    {
        loop_->QueueInLoop( boost::bind( &Connector::ConnectTo, connector_.get(), addr, port ) );
    }

    void TcpClient::OnConnect(int fd, bool connected)
    {
        assert( connections_.find(fd) == connections_.end() );

        TcpConnectionState state = connected ? kConnected : kConnecting; 
        TcpConnectionPtr conn = boost::make_shared<TcpConnection>( loop_, fd, state );
        conn->set_read_callback( rcb_ );
        conn->set_close_callback( boost::bind( &TcpClient::OnCloseConnection, this, _1) );

        connections_[fd] = conn;

        if( connected && ccb_) ccb_( conn );
        else
        {
            conn->set_connected_callback( boost::bind( &TcpClient::OnConnected, this, _1 ) );
        }
    }

    void TcpClient::OnConnected( TcpConnectionPtr conn )
    {
        int fd = conn->fd();
        assert( connections_.find(fd) != connections_.end() );

        if( ccb_ ) ccb_(conn);
    }

    void TcpClient::OnCloseConnection( int fd )
    {
        TcpConnectionMap::iterator iter = connections_.find(fd);
        assert( iter != connections_.end() );
        TcpConnectionPtr conn = iter->second;

        connections_.erase(iter);
        conn->loop()->QueueInLoop( boost::bind( &TcpConnection::Destroy, conn ) );
    }
}
