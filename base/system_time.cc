/*
 * =====================================================================================
 *
 *       Filename:  system_time.cc
 *        Created:  08/22/14 20:43:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "system_time.h"
#include <sys/time.h>
#include <cassert>

namespace fx
{
    namespace base
    {
        namespace time
        {
            TimeStamp Now()
            {
                timeval tv;
                int ret = ::gettimeofday(&tv, 0);
                assert (ret == 0);
                (void) ret;

                return static_cast<TimeStamp>(tv.tv_sec) * 1000 + static_cast<TimeStamp>(static_cast<double>(tv.tv_usec) / 1000);
            }

            time_t StringToTime(const char * time)
            {
                const char * fmt = "%Y-%m-%d %H:%M:%S";
                struct tm tm;
                char * end = ::strptime(time, fmt, &tm);
                if (end == NULL || *end != '\0') { return -1; }
                return ::mktime(&tm);
            }
        }
    }
}
