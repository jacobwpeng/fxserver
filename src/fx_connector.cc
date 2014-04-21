/*
 * =====================================================================================
 *
 *       Filename:  fx_connector.cc
 *        Created:  04/13/2014 12:42:34 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_connector.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <glog/logging.h>

#include "fx_net_address.h"
#include "fx_socket_op.h"
#include "fx_event_loop.h"

namespace fx
{
    Connector::Connector(EventLoop * loop)
        :loop_(loop)
    {

    }

    Connector::~Connector()
    {

    }

    void Connector::ConnectTo( const NetAddress & addr )
    {
        int fd = socket( AF_INET, SOCK_STREAM, 0 );
        PCHECK( fd >= 0 ) << "Create socket failed.";
        socketop::SetNonblocking(fd);

        sockaddr_in peer_addr = addr.ToSockAddr();

        bool connected;
        int ret = ::connect(fd, reinterpret_cast<sockaddr*>(&peer_addr), sizeof(peer_addr) );
        if( ret == 0 )
        {
            ccb_(fd, true);
        }
        else if( ret == -1 && errno == EINPROGRESS )
        {
            ccb_(fd, false);
        }
        else
        {
            close(fd);
            PLOG(WARNING) << "connect failed, ret = " << ret;
            if( cecb_ ) cecb_( addr.ip_addr(), addr.port() );
        }
    }

    void Connector::ConnectTo(const std::string& ip_addr, int port)
    {
        ConnectTo( NetAddress(ip_addr, port) );
    }
}
