/*
 * =====================================================================================
 *       Filename:  fx_acceptor.cc
 *        Created:  14:11:36 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */
#include "fx_acceptor.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_channel.h"
#include "fx_tcp_connection.h"

namespace detail
{
    void ReadCallback( fx::TcpConnectionPtr conn, fx::Buffer * buf )
    {
        std::string content = buf->ReadAndClear();
        LOG(INFO) << " buf ByteSize = " << buf->ByteSize();
        LOG(INFO) << "read from peer [" << content << "]";
        conn->Write( content );
    }
}

namespace fx
{
    Acceptor::Acceptor(EventLoop * loop)
        :loop_(loop)
    {

    }

    Acceptor::~Acceptor()
    {
        close( listen_channel_->fd() );
    }

    void Acceptor::BindOrAbort(const std::string& addr, int port)
    {
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        int enable_reuse_addr = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable_reuse_addr, sizeof(enable_reuse_addr) );

        timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(listen_fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout) );

        sockaddr_in server_addr;
        memset(&server_addr, 0x0, sizeof(server_addr) );
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(addr.c_str());

        PCHECK( bind(listen_fd_, (sockaddr *)(&server_addr), sizeof(server_addr) ) >= 0 ) << "Bind failed!";

        SetNonblocking(listen_fd_);
    }

    void Acceptor::Listen()
    {
        PCHECK( listen(listen_fd_, SOMAXCONN) >= 0 ) << "listen failed!";

        listen_channel_.reset( new Channel(loop_, listen_fd_) );
        listen_channel_->EnableReading();

        listen_channel_->set_read_callback( boost::bind( &Acceptor::NewConnetion, this ) );
        loop_->UpdateChannel( listen_channel_.get() );
    }

    void Acceptor::NewConnetion()
    {
        sockaddr_in clt_addr;
        const int len = sizeof(clt_addr);
        int client_fd = accept(listen_fd_, (sockaddr *)&clt_addr, (unsigned*)&len );

        LOG(INFO) << "New Connection, fd = " << client_fd;

        SetNonblocking( client_fd );
        TcpConnectionPtr conn = boost::make_shared<TcpConnection>(loop_, client_fd);
        conn->set_read_callback( detail::ReadCallback );
        conn->set_close_callback( boost::bind(&Acceptor::OnCloseConnection, this, _1) );

        assert( connections_.find( client_fd ) == connections_.end() );

        connections_[client_fd] = conn;
    }

    void Acceptor::SetNonblocking(int fd)
    {
        int opts;
        PCHECK( (opts = fcntl(fd, F_GETFL)) >= 0 ) << "get opts failed!" ;

        opts = opts | O_NONBLOCK;
        PCHECK( fcntl(fd, F_SETFL, opts) >= 0 ) << "set opts failed!";
    }

    void Acceptor::OnCloseConnection(int fd)
    {
        TcpConnectionMap::iterator iter = connections_.find( fd );
        assert( iter != connections_.end() );
        LOG(INFO) << "remove conn from connections, fd = " << fd;
        loop_->RunInLoop( boost::bind( &TcpConnection::Destroy, iter->second ) );
        connections_.erase( iter );
    }
}