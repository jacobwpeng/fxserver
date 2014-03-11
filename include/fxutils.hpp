/*
 * =====================================================================================
 *       Filename:  fxutils.hpp
 *        Created:  12:19:00 Mar 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  utils for fx framework
 *
 * =====================================================================================
 */

#ifndef  __FXUTILS_H__
#define  __FXUTILS_H__

#include <sys/time.h>

inline uint64_t Timeval2Milliseconds(timeval tv)
{
    return static_cast<uint64_t>(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

inline uint64_t NowInMilliSeconds()
{
    timeval now;
    gettimeofday(&now, NULL);
    return Timeval2Milliseconds(now);
}

#endif   /* ----- #ifndef __FXUTILS_H__----- */

