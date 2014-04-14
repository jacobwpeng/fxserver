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

#include "fx_channel.h"
#include "fx_event_loop.h"

namespace fx
{
    TcpConnection::TcpConnection( EventLoop * loop, int fd, TcpConnectionState state)
        :loop_(loop), fd_(fd), state_( state )
    {
        channel_.reset( new Channel(loop, fd) );
        if( state_ == kConnecting )
        {
            channel_->set_write_callback( boost::bind( &TcpConnection::ConnectedToPeer, this ) );
            loop_->RunInLoop( boost::bind( &Channel::EnableWriting, channel_.get() ) ); /* TODO : 是否有必要RunInLoop */
        }
        else if( state_ == kConnected )
        {
            channel_->set_read_callback( boost::bind( &TcpConnection::ReadFromPeer, this ) );
            channel_->set_write_callback( boost::bind( &TcpConnection::WriteToPeer, this ) );
            loop_->RunInLoop( boost::bind( &Channel::EnableReading, channel_.get() ) ); /* TODO : 是否有必要RunInLoop */
        }
        else
        {
            assert( false );                    /* 奇怪的构造参数 */
        }
        /* TODO : 增加error callback */

    }

    TcpConnection::~TcpConnection()
    {
        LOG(INFO) << "TcpConnection Destroyed.";
        close(fd_);
    }

    void TcpConnection::Write( const std::string& content )
    {
        Write( content.c_str(), content.length() );
    }

    void TcpConnection::Write( const char * buf, size_t len )
    {
        write_buf_.Append( buf, len );
        channel_->EnableWriting();
    }

    void TcpConnection::set_read_callback(ReadCallback rcb)
    {
        rcb_ = rcb;
    }

    void TcpConnection::set_close_callback( CloseCallback ccb )
    {
        ccb_ = ccb;
    }

    void TcpConnection::Close()
    {
        state_ = kDisconnected;
        channel_->DisableReading();
        if( ccb_ ) ccb_( fd_ );
    }

    void TcpConnection::Destroy()
    {
        assert( state_ == kDisconnected );
        loop_->AssertInLoopThread();
        LOG(INFO) << "TcpConnection::Destroy";
        //LOG(INFO) << "conn use_count = " << shared_from_this().use_count();
    }

    void TcpConnection::ReadFromPeer()
    {
        char buf[ 1 << 16 ];                    /* 64K stack buf */
        const int iovcnt = 2;
        iovec iov[iovcnt];

        size_t bytes_left = read_buf_.BytesCanWrite();
        iov[0].iov_base = read_buf_.WriteBegin();
        iov[0].iov_len = bytes_left;
        iov[1].iov_base = buf;
        iov[1].iov_len = sizeof(buf);

        ssize_t bytes_read = readv(fd_, iov, iovcnt);
        LOG_IF( INFO, bytes_read <= 0 ) << " bytes_read = " << bytes_read;

        if( bytes_read == 0 || (bytes_read == -1 && errno != EAGAIN) )
        {
            /* 客户端断开连接 */
            Close();
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
        /* TODO : 可能还没读完？ */
    }

    void TcpConnection::WriteToPeer()
    {
        size_t bytes_to_read = write_buf_.BytesToRead();

        while( bytes_to_read )
        {
            /* TODO : 判断EWOULDBLOCK 和 EAGAIN */
            ssize_t bytes_write = write(fd_, write_buf_.ReadBegin(), bytes_to_read );
            PCHECK( bytes_write >= 0 ) << "write failed.";

            write_buf_.ConsumeBytes( bytes_write );
            bytes_to_read = write_buf_.BytesToRead();
        }

        channel_->DisableWriting();
    }

    void TcpConnection::ConnectedToPeer()
    {
        assert( state_ == kConnecting );
        state_ = kConnected;

        channel_->EnableReading();
        channel_->set_read_callback( boost::bind( &TcpConnection::ReadFromPeer, this ) );
        channel_->DisableWriting();
        channel_->set_write_callback( boost::bind( &TcpConnection::WriteToPeer, this ) );

        if( connected_callback_ ) connected_callback_( shared_from_this() );
    }

    int TcpConnection::fd() const 
    { 
        return channel_->fd(); 
    }
}
