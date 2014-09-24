/*
 * =====================================================================================
 *
 *       Filename:  udp_server.cc
 *        Created:  08/22/14 15:28:49
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "udp_server.h"
#include <glog/logging.h>

#include "net_address.h"
#include "event_loop.h"
#include "udp_listener.h"

namespace fx
{
    namespace net
    {
        namespace udp
        {
            UdpServer::UdpServer(EventLoop * loop, const fx::net::NetAddress& addr)
                :loop_(loop), addr_(addr)
            {
            }

            UdpServer::~UdpServer()
            {
            }

            void UdpServer::Start()
            {
                listener_.reset(new UdpListener(loop_));
                listener_->set_message_callback(rcb_);
                LOG(INFO) << "bind to " << addr_;
                listener_->BindOrAbort(addr_);
                listener_->Start();
            }
        }
    }
}
