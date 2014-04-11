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
            TcpServer(EventLoop * loop_, const std::string& addr, int port); /* TODO : 封装地址结构 */
            ~TcpServer();

            void Start();
            void SetThreadNum( unsigned thread_num );

        private:
            void OnNewConnection(int fd);
            void OnRead(TcpConnectionPtr conn, Buffer * buf);
            void OnConnectionClose(int fd);

        private:
            typedef std::map<int, TcpConnectionPtr> TcpConnectionMap;
            TcpConnectionMap connections_;
            boost::scoped_ptr<Acceptor> acceptor_;
            EventLoop * base_loop_;
            boost::scoped_ptr<EventLoopThreadPool> loop_threads_;

            std::string addr_;
            int port_;
            unsigned thread_num_;
    };
}

#endif   /* ----- #ifndef __FX_TCP_SERVER_H__----- */
