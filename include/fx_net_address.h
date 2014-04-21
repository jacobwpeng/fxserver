/*
 * =====================================================================================
 *       Filename:  fx_net_address.h
 *        Created:  15:14:55 Apr 21, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  class for represent ipv4 network address and relative ops
 *
 * =====================================================================================
 */

#ifndef  __FX_NET_ADDRESS_H__
#define  __FX_NET_ADDRESS_H__

#include <string>
#include <iosfwd>

struct sockaddr_in;

namespace fx
{
    class NetAddress
    {
        public:
            /* only accept ip address, such as 127.0.0.1 */
            NetAddress(const std::string& ip_addr, int port);
            /* convert from sockaddr_in */
            NetAddress( const sockaddr_in & sa );
            ~NetAddress();

            std::string ip_addr() const { return addr_; }
            int port() const { return port_; }
            std::string FullAddress() const;

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

#endif   /* ----- #ifndef __FX_NET_ADDRESS_H__----- */
