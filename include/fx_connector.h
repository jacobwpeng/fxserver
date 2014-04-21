/*
 * =====================================================================================
 *
 *       Filename:  fx_connector.h
 *        Created:  04/13/2014 11:43:27 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_CONNECTOR__
#define  __FX_CONNECTOR__

#include <string>
#include <boost/function.hpp>

namespace fx
{
    class EventLoop;
    class NetAddress;
    class Connector
    {
        public:
            typedef boost::function< void(int, bool) > ConnectCallback;
            typedef boost::function< void(const std::string&, int) > ConnectErrorCallback;

        public:
            Connector(EventLoop * loop);
            ~Connector();

            void set_connect_callback(ConnectCallback ccb) { ccb_ = ccb; }
            void set_connect_error_calback( ConnectErrorCallback cecb ) { cecb_ = cecb; }

            void ConnectTo( const NetAddress & addr );
            void ConnectTo( const std::string& ip_addr, int port); /* TODO : 域名解析 */

        private:
            EventLoop * loop_;
            ConnectCallback ccb_;
            ConnectErrorCallback cecb_;
    };
}

#endif   /* ----- #ifndef __FX_CONNECTOR__  ----- */
