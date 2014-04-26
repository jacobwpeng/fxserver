/*
 * =====================================================================================
 *
 *       Filename:  fx_socket_op.h
 *        Created:  04/13/2014 12:34:01 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_SOCKET_OP__
#define  __FX_SOCKET_OP__

#include <string>

namespace fx
{
    namespace socketop
    {
        void SetNonblocking(int fd);
        int GetAndClearError( int fd );
        void DisableReading( int fd );
        void DisableWriting( int fd );
        void DisableReadingAndWriting( int fd );
    }
}

#endif   /* ----- #ifndef __FX_SOCKET_OP__  ----- */
