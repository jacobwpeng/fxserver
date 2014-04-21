/*
 * =====================================================================================
 *
 *       Filename:  fx_socket_op.cc
 *        Created:  04/13/2014 12:34:42 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_socket_op.h"

#include <fcntl.h>
#include <sys/socket.h>

#include <glog/logging.h>

namespace fx
{
    namespace socketop
    {
        void SetNonblocking(int fd)
        {
            int opts;
            PCHECK( (opts = fcntl(fd, F_GETFL)) >= 0 ) << "get opts failed!" ;

            opts = opts | O_NONBLOCK;
            PCHECK( fcntl(fd, F_SETFL, opts) >= 0 ) << "set opts failed!";
        }

        void GetAndClearError( int fd )
        {
            int so_error;
            socklen_t slen = sizeof(so_error);
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
            if( so_error != 0 )
            {
                LOG(WARNING) << "Socket Error : " << strerror(so_error)
                    << ", fd = " << fd;
            }
        }

        void DisableReading( int fd )
        {
            PCHECK( shutdown( fd, SHUT_RD ) == 0 ) << "DisableReading failed";
        }

        void DisableWriting( int fd )
        {
            PCHECK( shutdown( fd, SHUT_WR ) == 0 ) << "DisableWriting failed";
        }

        void DisableReadingAndWriting( int fd )
        {
            PCHECK( shutdown( fd, SHUT_RDWR ) == 0 ) << "DisableReadingAndWriting failed";
        }
    }
}
