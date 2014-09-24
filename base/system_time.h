/*
 * =====================================================================================
 *
 *       Filename:  system_time.h
 *        Created:  08/22/14 20:41:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SYSTEM_TIME_H__
#define  __SYSTEM_TIME_H__

#include <stdint.h>
#include <ctime>

namespace fx
{
    namespace base
    {
        namespace time
        {
            typedef uint64_t TimeStamp;

            //Unix timestamp in ms
            TimeStamp Now();

            /*-----------------------------------------------------------------------------
             *  format -> 2014-08-22 01:23:45
             *-----------------------------------------------------------------------------*/
            time_t StringToTime(const char * time);
        }
    }
}

#endif   /* ----- #ifndef __SYSTEM_TIME_H__  ----- */
