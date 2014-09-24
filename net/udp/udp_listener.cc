/*
 * =====================================================================================
 *
 *       Filename:  udp_listener.cc
 *        Created:  08/22/14 15:47:38
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "udp_listener.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glog/logging.h>

#include "net_address.h"
#include "channel.h"
#include "event_loop.h"


namespace fx
{
    namespace net
    {
        namespace udp
        {
            UdpListener::UdpListener(EventLoop * loop)
                :loop_(loop), fd_(-1)
            {
            }

            UdpListener::~UdpListener()
            {
                if (channel_)
                {
                    channel_->Remove();
                }
                if (fd_ != -1)
                {
                    ::close(fd_);
                }
            }

            void UdpListener::BindOrAbort(const fx::net::NetAddress & addr)
            {
                fd_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                PCHECK(fd_ >= 0) << "create socket failed";

                struct sockaddr_in sa = addr.ToSockAddr();
                int ret = ::bind(fd_, reinterpret_cast<const struct sockaddr *>(&sa), sizeof(sa));
                PCHECK(ret == 0) << "bind failed";
            }

            void UdpListener::Start()
            {
                channel_.reset(new Channel(loop_, fd_));
                channel_->set_read_callback(std::bind(&UdpListener::OnMessage, this));
                channel_->EnableReading();
                channel_->DisableWriting();
            }

            void UdpListener::OnMessage()
            {
                const size_t kMaxUdpMessageSize = 1 << 16; //64KiB
                char in[kMaxUdpMessageSize];
                struct sockaddr_in ca;
                socklen_t len = sizeof(ca);
                ssize_t bytes = ::recvfrom(fd_, in, sizeof(in), MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), &len);
                if (bytes < 0)
                {
                    PLOG(WARNING) << "recvfrom failed";
                    return;
                }
                if (mcb_)
                {
                    std::string out;
                    int ret = mcb_(in, bytes, &out);
                    if (ret < 0)
                    {
                        LOG(WARNING) << "MessageCallback return " << ret;
                    }
                    else if (out.size() > kMaxUdpMessageSize)
                    {
                        LOG(WARNING) << "reply size[" << out.size() << "] is too larger, truncate reply to 64KiB";
                        ::sendto(fd_, out.data(), kMaxUdpMessageSize, MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), sizeof(ca));
                    }
                    else
                    {
                        ::sendto(fd_, out.data(), out.size(), MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), sizeof(ca));
                    }
                }
            }
        }
    }
}
