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

    void Connector::set_connect_callback(ConnectCallback ccb)
    {
        ccb_ = ccb;
    }

    void Connector::ConnectTo(const std::string& ip_addr, int port)
    {
        int fd = socket( AF_INET, SOCK_STREAM, 0 );
        PCHECK( fd >= 0 ) << "Create socket failed."; /* TODO : 错误处理 */

        socketop::SetNonblocking(fd);

        sockaddr_in peer_addr;
        memset( &peer_addr, 0x0, sizeof(peer_addr) );

        peer_addr.sin_family = AF_INET;
        peer_addr.sin_port = htons(port);
        peer_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());

        bool connected;
        int ret = ::connect(fd, reinterpret_cast<sockaddr*>(&peer_addr), sizeof(peer_addr) );
        if( ret == 0 )
        {
            ccb_(fd, true);
        }
        else if( ret == -1 && EINPROGRESS )
        {
            ccb_(fd, false);
        }
        else
        {
            close(fd);
            PLOG(WARNING) << "connect failed, ret = " << ret;
            /* TODO : 错误回调 */
        }
    }
}
