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
    typedef boost::function< void(int, bool) > ConnectCallback;
    class Connector
    {
        public:
            Connector(EventLoop * loop);
            ~Connector();

            void set_connect_callback(ConnectCallback ccb);

            void ConnectTo( const std::string& ip_addr, int port); /* TODO : 域名解析 */

        private:
            EventLoop * loop_;
            ConnectCallback ccb_;
    };
}

#endif   /* ----- #ifndef __FX_CONNECTOR__  ----- */
