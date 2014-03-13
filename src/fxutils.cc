/*
 * =====================================================================================
 *       Filename:  fxutils.cc
 *        Created:  11:22:59 Mar 13, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  utilities implement
 *
 * =====================================================================================
 */

#include "fx_predefine.h"

#include "fxutils.h"

uint64_t Timeval2Milliseconds(timeval tv)
{
    return static_cast<uint64_t>(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

uint64_t NowInMilliSeconds()
{
    timeval now;
    gettimeofday(&now, NULL);
    return Timeval2Milliseconds(now);
}
