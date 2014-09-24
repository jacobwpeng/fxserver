/*
 * =====================================================================================
 *
 *       Filename:  event_loop.h
 *        Created:  08/22/14 14:34:28
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __EVENT_LOOP_H__
#define  __EVENT_LOOP_H__

#include <signal.h>
#include <memory>
#include <functional>
#include "system_time.h"
#include "timer_manager.h"

namespace fx
{
    namespace net
    {
        class Poller;
        class Channel;

        class EventLoop
        {
            public:
                typedef std::function< bool(fx::base::time::TimeStamp, uint64_t) > WakeupRoutine;

            public:
                EventLoop();
                ~EventLoop();

                void Run();
                void Quit();
                void UpdateChannel(Channel * channel);
                void RemoveChannel(Channel * channel);
                TimerManager::TimerId RunAt(fx::base::time::TimeStamp time, const TimerManager::TimerCallback & cb);
                TimerManager::TimerId RunAfter(uint64_t milliseconds, const TimerManager::TimerCallback & cb);

                void SetWakeupRoutine(const WakeupRoutine& routine) { wakeup_routine_ = routine; }

            private:
                //void AssertInLoopThread();
                //bool InLoopThread() const;

            private:
                std::unique_ptr<Poller> poller_;
                std::unique_ptr<TimerManager> timer_mgr_;
                WakeupRoutine wakeup_routine_;
                sig_atomic_t quit_;
                uint64_t iteration_;
                int wait_time_;
                int idle_time_;
        };
    }
}

#endif   /* ----- #ifndef __EVENT_LOOP_H__  ----- */
