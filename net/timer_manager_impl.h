/*
 * =====================================================================================
 *
 *       Filename:  timer_manager_impl.h
 *        Created:  09/24/14 12:57:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __TIMER_MANAGER_IMPL_H__
#define  __TIMER_MANAGER_IMPL_H__

#include "timer_manager.h"
#include <map>

namespace fx
{
    namespace net
    {
        using fx::base::time::TimeStamp;
        class TimerManager::Impl
        {
            public:
                Impl();
                ~Impl();

                TimerManager::TimerId RunAt(TimeStamp time, const TimerManager::TimerCallback & cb);
                bool Remove(TimerManager::TimerId id);
                TimerCallbackList Step(TimeStamp now);

            private:
                struct Timer
                {
                    TimerManager::TimerId id;
                    TimeStamp expire;
                    TimerManager::TimerCallback cb;
                };
                typedef std::unique_ptr<Timer> TimerPtr;

            private:
                TimerManager::TimerId current_;
                std::map<TimeStamp, TimerPtr> timers_;
                std::map<TimerManager::TimerId, TimeStamp> expire_time_;
        };
    }
}

#endif   /* ----- #ifndef __TIMER_MANAGER_IMPL_H__  ----- */
