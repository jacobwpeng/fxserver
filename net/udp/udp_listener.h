/*
 * =====================================================================================
 *
 *       Filename:  udp_listener.h
 *        Created:  08/22/14 15:44:10
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  listen to udp message
 *
 * =====================================================================================
 */

#ifndef  __UDP_LISTENER_H__
#define  __UDP_LISTENER_H__

#include <string>
#include <functional>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace fx
{
    namespace net
    {
        class Channel;
        class EventLoop;
        class NetAddress;
        namespace udp
        {
            class UdpListener : boost::noncopyable
            {
                public:
                    typedef std::function< int(const char*, size_t, std::string*) > MessageCallback;

                public:
                    UdpListener(EventLoop * loop);
                    ~UdpListener();

                    void set_message_callback(const MessageCallback& mcb) { mcb_ = mcb; }
                    void BindOrAbort(const NetAddress& addr);
                    void Start();

                private:
                    void OnMessage();

                private:
                    EventLoop * loop_;
                    int fd_;
                    boost::scoped_ptr<Channel> channel_;
                    MessageCallback mcb_;
            };
        }
    }
}
#endif   /* ----- #ifndef __UDP_LISTENER_H__  ----- */
