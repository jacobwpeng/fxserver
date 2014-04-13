/*
 * =====================================================================================
 *
 *       Filename:  fx_tcp_client.h
 *        Created:  04/13/2014 03:39:09 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_TCP_CLIENT__
#define  __FX_TCP_CLIENT__

#include <map>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace fx
{
    class Buffer;
    class EventLoop;
    class Connector;
    class TcpConnection;

    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map< int, TcpConnectionPtr > TcpConnectionMap;

    typedef boost::function< void(TcpConnectionPtr) > ConnectedCallback;
    typedef boost::function< void(TcpConnectionPtr, Buffer*) > ReadCallback;
    class TcpClient : boost::noncopyable
    {
        public:
            TcpClient(EventLoop * loop);
            ~TcpClient();

            void ConnectTo( const std::string& addr, int port ); /* TODO : 域名解析 */
            void set_connected_callback( ConnectedCallback ccb ) { ccb_ = ccb; }
            void set_read_callback( ReadCallback rcb ) { rcb_ = rcb; }

        private:
            void OnConnect(int fd, bool connected);
            void OnConnected( TcpConnectionPtr conn );
            void OnCloseConnection( int fd );

        private:
            EventLoop * loop_;
            TcpConnectionMap connections_;
            boost::scoped_ptr<Connector> connector_;
            ConnectedCallback ccb_;
            ReadCallback rcb_;
    };
}

#endif   /* ----- #ifndef __FX_TCP_CLIENT__  ----- */
