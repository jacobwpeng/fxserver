/*
 * =====================================================================================
 *       Filename:  fx_base.cc
 *        Created:  16:43:07 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_base.h"
#include <sys/time.h>
#include <cassert>

namespace fx
{
    namespace base
    {
        /* In milliseconds */
        TimeStamp Now()
        {
            timeval tv;
            int ret = gettimeofday(&tv, 0);
            assert( ret == 0 );

            return static_cast<TimeStamp>(
                    static_cast<double>(
                        static_cast<TimeStamp>(tv.tv_sec) * 1000000 + static_cast<TimeStamp>(tv.tv_usec)
                        )/1000
                    );
        }
    }
}
