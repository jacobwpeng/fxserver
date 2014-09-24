/*
 * =====================================================================================
 *
 *       Filename:  udp_server.h
 *        Created:  08/22/14 15:23:16
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __UDP_SERVER_H__
#define  __UDP_SERVER_H__

#include <string>
#include <functional>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "net_address.h"

namespace fx
{
    namespace net
    {
        class Buffer;
        class EventLoop;
        class NetAddress;

        namespace udp
        {
            class UdpListener;

            class UdpServer : boost::noncopyable
            {
                public:
                    typedef std::function< int(const char*, size_t, std::string*) > ReadCallback;

                public:
                    UdpServer(EventLoop * loop, const NetAddress& addr);
                    ~UdpServer();

                    void Start();
                    void set_read_callback(const ReadCallback& rcb) { rcb_ = rcb; }

                private:
                    EventLoop * loop_;
                    ReadCallback rcb_;
                    boost::scoped_ptr<UdpListener> listener_;

                    const fx::net::NetAddress addr_;
            };
        }
    }
}

#endif   /* ----- #ifndef __UDP_SERVER_H__  ----- */
