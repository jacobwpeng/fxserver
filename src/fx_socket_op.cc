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
    }
}
