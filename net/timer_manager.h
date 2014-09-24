/*
 * =====================================================================================
 *
 *       Filename:  timer_manager.h
 *        Created:  09/23/14 14:37:38
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __TIMER_MANAGER_H__
#define  __TIMER_MANAGER_H__

#include <vector>
#include <memory>
#include <functional>
#include "system_time.h"

namespace fx
{
    namespace net
    {
        class TimerManager
        {
            public:
                typedef std::function< void(void) > TimerCallback;
                typedef std::vector<TimerCallback> TimerCallbackList;
                typedef uint64_t TimerId;

            public:
                TimerManager();
                ~TimerManager();

                TimerId RunAfter(uint64_t milliseconds, const TimerCallback & cb);
                TimerId RunAt(fx::base::time::TimeStamp time, const TimerCallback & cb);
                bool Remove(TimerId id);
                TimerCallbackList Step(fx::base::time::TimeStamp now);

            private:
                class Impl;
                std::unique_ptr<Impl> impl_;
        };
    }
}

#endif   /* ----- #ifndef __TIMER_MANAGER_H__  ----- */
