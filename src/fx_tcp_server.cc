/*
 * =====================================================================================
 *       Filename:  fx_tcp_server.cc
 *        Created:  09:35:32 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_tcp_server.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_acceptor.h"
#include "fx_tcp_connection.h"
#include "fx_buffer.h"
#include "fx_event_loop_thread_pool.h"

namespace fx
{
    TcpServer::TcpServer(EventLoop * loop, const std::string& addr, int port)
        :base_loop_(loop), addr_(addr), port_(port), thread_num_(0)
    {
        acceptor_.reset( new Acceptor(loop) );
        loop_threads_.reset( new EventLoopThreadPool(loop) );
    }

    TcpServer::~TcpServer()
    {
    }

    void TcpServer::Start()
    {
        acceptor_->set_new_connection_callback( boost::bind(&TcpServer::OnNewConnection, this, _1) );
        acceptor_->BindOrAbort( addr_, port_ );
        base_loop_->RunInLoop( boost::bind( &Acceptor::Listen, acceptor_.get() ) );
        loop_threads_->Start();
    }

    void TcpServer::SetThreadNum( unsigned thread_num )
    {
        if( thread_num == 0 ) return;
        else
        {
            loop_threads_->SetLoopThreadCount( thread_num );
            thread_num_ = thread_num;
        }
    }

    void TcpServer::OnNewConnection(int fd)
    {
        EventLoop * loop = loop_threads_->NextLoop();
        TcpConnectionPtr conn = boost::make_shared<TcpConnection>(loop, fd, kConnected);
        conn->set_read_callback( rcb_ );
        conn->set_close_callback( boost::bind(&TcpServer::OnConnectionClosed, this, _1) );

        assert( connections_.find( fd ) == connections_.end() );

        connections_[fd] = conn;

        if( nccb_ ) 
        {
            loop->RunInLoop( boost::bind( nccb_, conn ) ); /* FIXME : 与conn->EnableReading存在时序问题 */
        }
        conn->StartReading();
    }

    void TcpServer::OnConnectionClosed(int fd)
    {
        TcpConnectionMap::iterator iter = connections_.find( fd );
        assert( iter != connections_.end() );
        LOG(INFO) << "remove conn from connections, fd = " << fd;

        TcpConnectionPtr conn = iter->second;
        connections_.erase( iter );

        if( cccb_ ) cccb_( conn );
        conn->loop()->QueueInLoop( boost::bind( &TcpConnection::Destroy, conn ) );
    }
}
