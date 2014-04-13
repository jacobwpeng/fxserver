/*
 * =====================================================================================
 *       Filename:  fx_tcp_connection.h
 *        Created:  10:50:35 Apr 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_TCP_CONNECTION_H__
#define  __FX_TCP_CONNECTION_H__

#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "fx_buffer.h"

namespace fx
{
    class Channel;
    class EventLoop;
    class TcpConnection;

    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef boost::function< void( TcpConnectionPtr conn ) > TcpConnectionConnectedCallback;
    typedef boost::function< void( TcpConnectionPtr conn, Buffer * buf) > TcpConnectionReadCallback;
    typedef boost::function< void( int ) > TcpConnectionCloseCallback;

    enum TcpConnectionState
    {
        kConnecting = 1,
        kConnected = 2,
        kDisconnected = 3
    };

    class TcpConnection : boost::noncopyable, public boost::enable_shared_from_this<TcpConnection>
    {
        public:
            TcpConnection(EventLoop * loop, int fd, TcpConnectionState state);
            ~TcpConnection();

            void Write( const std::string& content );
            void Write( const char * buf, size_t len );

            void set_connected_callback( TcpConnectionConnectedCallback connected_callback) { connected_callback_ = connected_callback; }
            void set_read_callback( TcpConnectionReadCallback rcb );
            void set_close_callback( TcpConnectionCloseCallback ccb );
            /* 关闭连接，但是不会马上关闭描述符，因为发送缓冲区中可能还有东西没有发 */
            void Close();
            /* 干掉自己 */
            void Destroy();

            int fd() const;
            bool closed() const { return state_ == kDisconnected; }
            EventLoop * loop() { return loop_; }

        private:
            void ReadFromPeer();
            void WriteToPeer();
            void ConnectedToPeer();

        private:
            EventLoop * loop_;
            const int fd_;
            TcpConnectionState state_;

            TcpConnectionReadCallback rcb_;
            TcpConnectionCloseCallback ccb_;
            TcpConnectionConnectedCallback connected_callback_;
            boost::scoped_ptr<Channel> channel_;
            Buffer read_buf_;
            Buffer write_buf_;
    };

}

#endif   /* ----- #ifndef __FX_TCP_CONNECTION_H__----- */
