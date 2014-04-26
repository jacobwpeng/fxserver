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
#include <boost/any.hpp>
#include "fx_buffer.h"

namespace fx
{
    class Slice;
    class Channel;
    class EventLoop;
    class NetAddress;
    class TcpConnection;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

    enum TcpConnectionState
    {
        kConnecting = 1,
        kConnected = 2,
        kDisconnected = 3
    };

    class TcpConnection : boost::noncopyable, 
                        public boost::enable_shared_from_this<TcpConnection>
    {
        public:
            typedef boost::function< void(TcpConnectionPtr) > ConnectedCallback;
            typedef boost::function< void(TcpConnectionPtr, Buffer*) > ReadCallback;
            typedef boost::function< void(int) > CloseCallback;
            typedef boost::any Context;

        public:
            TcpConnection(EventLoop * loop, int fd, TcpConnectionState state);
            ~TcpConnection();


            /*-----------------------------------------------------------------------------
             *  避免在构造函数中EnableReading导致时序问题
             *-----------------------------------------------------------------------------*/
            void StartReading();

            void Write( const std::string& content );
            void Write( const char * buf, size_t len );
            void Write( const Slice& slice );

            void set_context( const Context & ctx ) { ctx_ = ctx; }
            Context context() const { return ctx_; }
            void set_connected_callback(ConnectedCallback connected_callback)
            { connected_callback_ = connected_callback; }

            void set_read_callback( ReadCallback rcb );
            void set_close_callback( CloseCallback ccb );
            /* 
             * 设置连接状态为kDisconnected，但是不会马上关闭描述符
             * 因为发送缓冲区中可能还有东西没有发 
             */
            void ActiveClose();
            void PassiveClose();
            /* 干掉自己, 应该是生命周期内最后调用的函数*/
            void Destroy();

            int fd() const;
            bool closed() const { return state_ == kDisconnected; }
            const NetAddress & LocalAddr() { return *local_addr_; }
            const NetAddress & PeerAddr() { return *peer_addr_; }
            EventLoop * loop() { return loop_; }

        private:
            void ReadFromPeer();
            void WriteToPeer();
            void HandleError();
            void ConnectedToPeer();
            void GetAddress();

        private:
            EventLoop * loop_;
            const int fd_;
            TcpConnectionState state_;

            ReadCallback rcb_;
            CloseCallback ccb_;
            ConnectedCallback connected_callback_;
            boost::scoped_ptr<Channel> channel_;
            boost::scoped_ptr<NetAddress> local_addr_;
            boost::scoped_ptr<NetAddress> peer_addr_;
            Buffer read_buf_;
            Buffer write_buf_;
            Context ctx_;
    };

}

#endif   /* ----- #ifndef __FX_TCP_CONNECTION_H__----- */
