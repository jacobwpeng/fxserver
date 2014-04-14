/*
 * =====================================================================================
 *       Filename:  fx_tcp_server.h
 *        Created:  09:25:40 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_TCP_SERVER_H__
#define  __FX_TCP_SERVER_H__

#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

namespace fx
{
    class Buffer;
    class Acceptor;
    class TcpConnection;
    class EventLoop;
    class EventLoopThreadPool;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

    class TcpServer
    {
        public:
            typedef boost::function< void(TcpConnectionPtr, Buffer*) > ReadCallback;
            typedef boost::function< void(TcpConnectionPtr) > CloseConnectionCallback;
            typedef boost::function< void(TcpConnectionPtr) > NewConnectionCallback;

        public:
            TcpServer(EventLoop * loop_, const std::string& addr, int port); /* TODO : 封装地址结构 */
            ~TcpServer();

            void Start();
            void SetThreadNum( unsigned thread_num );
            void set_read_callback( ReadCallback rcb ) { rcb_ = rcb; }
            void set_close_connection_callback( CloseConnectionCallback cccb ) { cccb_ = cccb; }
            void set_new_connection_callback( NewConnectionCallback nccb ) { nccb_ = nccb; }

        private:
            void OnNewConnection(int fd);
            void OnConnectionClosed(int fd);

        private:
            typedef std::map<int, TcpConnectionPtr> TcpConnectionMap;
            TcpConnectionMap connections_;
            boost::scoped_ptr<Acceptor> acceptor_;
            EventLoop * base_loop_;
            boost::scoped_ptr<EventLoopThreadPool> loop_threads_;

            std::string addr_;
            int port_;
            unsigned thread_num_;
            ReadCallback rcb_;
            CloseConnectionCallback cccb_;
            NewConnectionCallback nccb_;
    };
}

#endif   /* ----- #ifndef __FX_TCP_SERVER_H__----- */
