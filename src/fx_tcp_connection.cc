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
    TcpConnection::TcpConnection( EventLoop * loop, int fd )
        :loop_(loop), fd_(fd), closed_(false)
    {
        channel_.reset( new Channel(loop, fd) );
        channel_->set_read_callback( boost::bind( &TcpConnection::ReadFromPeer, this ) );
        channel_->set_write_callback( boost::bind( &TcpConnection::WriteToPeer, this ) );
        channel_->EnableReading();
    }

    TcpConnection::~TcpConnection()
    {
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

    void TcpConnection::set_read_callback(TcpConnectionReadCallback rcb)
    {
        rcb_ = rcb;
    }

    void TcpConnection::set_close_callback( TcpConnectionCloseCallback ccb )
    {
        ccb_ = ccb;
    }

    void TcpConnection::Close()
    {
        closed_ = true;
        channel_->DisableReading();
        if( ccb_ ) ccb_( fd_ );
    }

    void TcpConnection::Destroy()
    {
        assert( closed_ );
        loop_->AssertInLoopThread();
        assert( shared_from_this().use_count() == 2 ); /* 一个正在调用这个方法， 另外一个是临时变量 */
    }

    void TcpConnection::ReadFromPeer()
    {
        char buf[ 1 << 20 ];                    /* 64K stack buf */
        const int iovcnt = 2;
        iovec iov[iovcnt];

        size_t bytes_left = read_buf_.BytesCanWrite();
        iov[0].iov_base = read_buf_.WriteBegin();
        iov[0].iov_len = bytes_left;
        iov[1].iov_base = buf;
        iov[1].iov_len = sizeof(buf);

        ssize_t bytes_read = readv(fd_, iov, iovcnt);
        PCHECK( bytes_read >= 0 ) << "readv failed"; /* TODO : 处理出错情况 */

        if( bytes_read == 0 )
        {
            /* 客户端断开连接 */
            Close();
        }
        else
        {
            LOG(INFO) << "read " << bytes_read << " bytes from peer.";

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
    }

    void TcpConnection::WriteToPeer()
    {
        LOG(INFO) << "WriteToPeer";
        size_t bytes_to_read = write_buf_.BytesToRead();
        assert( bytes_to_read >= 0 );

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
}
