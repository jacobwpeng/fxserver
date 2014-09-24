/*
 * =====================================================================================
 *       Filename:  net_address.h
 *        Created:  15:14:55 Apr 21, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  ipv4 network address and relative ops
 *
 * =====================================================================================
 */

#ifndef  __NET_ADDRESS_H__
#define  __NET_ADDRESS_H__

#include <string>
#include <iosfwd>
#include "slice.h"

struct sockaddr_in;

namespace fx
{
    namespace net
    {
        class NetAddress
        {
            public:
                /* only accept ip address, such as 127.0.0.1 */
                NetAddress(const fx::base::Slice& ip_addr, int port);
                /* convert from sockaddr_in */
                NetAddress(const sockaddr_in & sa);
                ~NetAddress();

                std::string ip_addr() const { return addr_; }
                int port() const { return port_; }
                std::string ToString() const;

                sockaddr_in ToSockAddr() const;

            public:
                /* get local/peer addr from connected sockfd */
                static NetAddress GetLocalAddr(int sockfd);
                static NetAddress GetPeerAddr(int sockfd);

            private:
                void MakeFullAddr();

            private:
                std::string addr_;
                int port_;
                std::string full_addr_;
        };
        std::ostream & operator << (std::ostream& os, const NetAddress & addr);
    }
}

#endif   /* ----- #ifndef __NET_ADDRESS_H__----- */
