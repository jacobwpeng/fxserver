/*
 * =====================================================================================
 *       Filename:  fx_net_address.cc
 *        Created:  15:21:25 Apr 21, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_net_address.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <ostream>
#include <boost/format.hpp>
#include <glog/logging.h>

namespace fx
{
    std::ostream & operator << (std::ostream& os, const NetAddress & addr)
    {
        os << addr.FullAddress();
        return os;
    }
    
    NetAddress::NetAddress( const std::string& addr, int port )
        :addr_(addr), port_(port)
    {
        MakeFullAddr();
    }

    NetAddress::NetAddress( const sockaddr_in & sa )
    {
        char buf[INET_ADDRSTRLEN];
        PCHECK( inet_ntop(AF_INET, &(sa.sin_addr), buf, INET_ADDRSTRLEN) != NULL ) << "inet_ntop failed";

        port_ = ntohs(sa.sin_port);
        addr_ = buf;

        MakeFullAddr();
    }

    NetAddress::~NetAddress()
    {

    }

    std::string NetAddress::FullAddress() const
    {
        return full_addr_;
    }

    sockaddr_in NetAddress::ToSockAddr() const
    {
        sockaddr_in sa;
        memset( &sa, 0x0, sizeof(sa) );

        sa.sin_family = AF_INET;
        sa.sin_port = htons(port_);
        int ret = inet_pton( AF_INET, addr_.c_str(), &sa.sin_addr );
        PCHECK( ret == 1 ) << "inet_pton failed, ret = " << ret << ", addr_ = " << addr_;

        return sa;
    }

    NetAddress NetAddress::GetLocalAddr(int sockfd)
    {
        sockaddr_in sa;
        memset( &sa, 0x0, sizeof(sa) );
        socklen_t sock_len = sizeof(sa);

        PCHECK( getsockname(sockfd, reinterpret_cast<sockaddr*>(&sa), &sock_len) == 0 ) << "getsockname failed";

        return NetAddress(sa);
    }

    NetAddress NetAddress::GetPeerAddr(int sockfd)
    {
        sockaddr_in sa;
        memset( &sa, 0x0, sizeof(sa) );
        socklen_t sock_len = sizeof(sa);

        PCHECK( getpeername(sockfd, reinterpret_cast<sockaddr*>(&sa), &sock_len) == 0 ) << "getsockname failed";

        return NetAddress(sa);
    }

    void NetAddress::MakeFullAddr()
    {
        if( full_addr_.empty() )
        {
            full_addr_ = boost::str( boost::format("%s:%d") % addr_ % port_  );
        }
    }
}
