/*
 * =====================================================================================
 *       Filename:  fx_base.h
 *        Created:  16:41:38 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  base function for fx framework
 *
 * =====================================================================================
 */

#ifndef  __FX_BASE_H__
#define  __FX_BASE_H__

#include <stdint.h>
#include <iostream>

namespace fx
{
    namespace base
    {
        typedef uint64_t TimeStamp;
        TimeStamp Now();

        struct ProgressTimer
        {
            ProgressTimer(std::ostream & os = std::cout );
            ~ProgressTimer();

            private:
            TimeStamp static Now();

            private:
                TimeStamp s_;
                std::ostream& os_;
        };
    }
}

#endif   /* ----- #ifndef __FX_BASE_H__----- */
