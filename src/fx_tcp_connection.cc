/*
 * =====================================================================================
 *       Filename:  fx_tcp_connection.cc
 *        Created:  10:55:37 Apr 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_tcp_connection.h"

#include <sys/uio.h>

#include <boost/bind.hpp>
#include <glog/logging.h>

#include "fx_slice.h"
#include "fx_channel.h"
#include "fx_event_loop.h"
#include "fx_socket_op.h"
#include "fx_net_address.h"

namespace fx
{
    TcpConnection::TcpConnection( EventLoop * loop, int fd, TcpConnectionState state)
        :loop_(loop), fd_(fd), state_( state )
    {
        channel_.reset( new Channel(loop, fd) );
        if( state_ == kConnecting )
        {
            channel_->set_write_callback( boost::bind( &TcpConnection::ConnectedToPeer, this ) );
            loop_->RunInLoop( boost::bind( &Channel::EnableWriting, channel_.get() ) );
        }
        else if( state_ == kConnected )
        {
            channel_->set_read_callback( boost::bind( &TcpConnection::ReadFromPeer, this ) );
            channel_->set_write_callback( boost::bind( &TcpConnection::WriteToPeer, this ) );
            GetAddress();
        }
        else
        {
            assert( false );                    /* 奇怪的构造参数 */
        }
        channel_->set_error_callback( boost::bind( &TcpConnection::HandleError, this ) );
    }

    TcpConnection::~TcpConnection()
    {
        LOG(INFO) << "TcpConnection::~TcpConnection";
        close(fd_);
    }

    void TcpConnection::StartReading()
    {
        loop_->RunInLoop( boost::bind( &Channel::EnableReading, channel_.get() ) );
    }

    void TcpConnection::Write( const std::string& content )
    {
        Write( content.c_str(), content.length() );
    }

    void TcpConnection::Write(const Slice& slice)
    {
        Write( slice.data(), slice.size() );
    }

    void TcpConnection::Write(const char * buf, size_t len)
    {
        loop_->AssertInLoopThread();
        write_buf_.Append( buf, len );
        channel_->EnableWriting();
    }

    void TcpConnection::ActiveClose()
    {
        assert( state_ != kDisconnected );
        if( state_ == kConnected )
        {
            channel_->DisableReading();
            socketop::DisableReading( fd_ );
        }
        else if( state_ == kConnecting )
        {
            channel_->DisableWriting();
        }

        state_ = kDisconnected;
        if( ccb_ ) ccb_( fd_ );
    }

    void TcpConnection::PassiveClose()
    {
        assert( state_ != kDisconnected );
        state_ = kDisconnected;
        channel_->DisableReading();
        channel_->DisableWriting();
        if( ccb_ ) ccb_( fd_ );
    }

    void TcpConnection::Destroy()
    {
        assert( state_ == kDisconnected );
        loop_->AssertInLoopThread();
        socketop::DisableWriting( fd_ );
        channel_->Remove();
        LOG(INFO) << "TcpConnection::Destroy, use_count() = " << (shared_from_this().use_count() - 1);
    }

    void TcpConnection::ReadFromPeer()
    {
        loop_->AssertInLoopThread();
        const size_t local_buf_len = 1 << 16;   /* 64k stack buf */
        char buf[ local_buf_len ];
        const int iovcnt = 2;
        iovec iov[iovcnt];

        size_t bytes_left = read_buf_.BytesCanWrite();
        iov[0].iov_base = read_buf_.WriteBegin();
        iov[0].iov_len = bytes_left;
        iov[1].iov_base = buf;
        iov[1].iov_len = sizeof(buf);

        ssize_t bytes_read = readv(fd_, iov, iovcnt);
        LOG_IF( INFO, bytes_read <= 0 ) << " bytes_read = " << bytes_read;

        if( bytes_read == 0 || 
            (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK )
          )
        {
            /* 客户端断开连接 */
            LOG(INFO) << "passive close connection, fd = " << fd_;
            PassiveClose();
        }
        else
        {
            if( static_cast<size_t>(bytes_read) > bytes_left )
            {
                read_buf_.AddBytes( bytes_left );
                size_t local_buf_bytes = bytes_read - bytes_left;
                read_buf_.Append( buf, local_buf_bytes );
            }
            else
            {
                read_buf_.AddBytes( bytes_read );
            }

            if( rcb_ ) rcb_( shared_from_this(), &read_buf_ );
        }
        /* 没读完的情况很少，而且如果发生了这样的情况poller会在下一次poll的时候仍然继续读取数据 */
    }

    void TcpConnection::WriteToPeer()
    {
        loop_->AssertInLoopThread();
        size_t bytes_to_read = write_buf_.BytesToRead();

        while( bytes_to_read != 0 )
        {
            ssize_t bytes_write = write(fd_, write_buf_.ReadBegin(), bytes_to_read );
            if( bytes_write == -1 )
            {
                if( errno == EAGAIN || errno == EWOULDBLOCK )
                {
                    /* Cannot write only more */
                    LOG(WARNING) << "Write would block";
                }
                else if( state_ == kDisconnected ) /* connection has been closed */
                {
                }
                else
                {
                    /* write just failed, so we close this connection */
                    ActiveClose();
                }
                break;
            }
            write_buf_.ConsumeBytes( bytes_write );
            bytes_to_read = write_buf_.BytesToRead();
        }

        /* only disable writing when all contents have been written to socket */
        if( bytes_to_read == 0 )
        {
            channel_->DisableWriting();
            if( wdcb_ ) wdcb_();
        }
    }

    void TcpConnection::HandleError()
    {
        LOG(WARNING) << "Got Error while HandleEvents, fd = " << fd_;
    }

    void TcpConnection::ConnectedToPeer()
    {
        assert( state_ == kConnecting );
        int so_error = socketop::GetAndClearError(fd_);
        if( so_error != 0 )
        {
            LOG(WARNING) << "Socket Error : " << strerror(so_error)
                << ", fd = " << fd_;
            ActiveClose();
        }
        else
        {
            state_ = kConnected;

            GetAddress();

            channel_->EnableReading();
            channel_->set_read_callback( boost::bind( &TcpConnection::ReadFromPeer, this ) );
            channel_->DisableWriting();
            channel_->set_write_callback( boost::bind( &TcpConnection::WriteToPeer, this ) );

            if( connected_callback_ ) connected_callback_( shared_from_this() );
        }
    }

    void TcpConnection::GetAddress()
    {
        /* can only be called on connected socket */
        assert( state_ == kConnected );
        /* can only be called once */
        assert( not local_addr_ );
        assert( not peer_addr_ );

        local_addr_.reset( new NetAddress(NetAddress::GetLocalAddr(fd_) ) );
        peer_addr_.reset( new NetAddress(NetAddress::GetPeerAddr(fd_) ) );
    }

    int TcpConnection::fd() const 
    { 
        return channel_->fd(); 
    }
}
