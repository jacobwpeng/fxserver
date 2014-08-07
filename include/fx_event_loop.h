/*
 * =====================================================================================
 *       Filename:  fx_event_loop.h
 *        Created:  10:35:15 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  EventLoop for event dispatching
 *
 * =====================================================================================
 */

#ifndef  __FX_EVENT_LOOP_H__
#define  __FX_EVENT_LOOP_H__

#include <vector>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/atomic.hpp>

#include "fx_timer_wheel.h"

namespace fx
{
    class Poller;
    class Channel;
    class TimerWheel;
    class EventLoop : boost::noncopyable
    {
        public:
            typedef boost::function< void(void) > PendingFunctor;
        public:
            EventLoop();
            ~EventLoop();

            void AssertInLoopThread();

            void Run();
            void Quit();
            void WakeUp();
            void UpdateChannel( Channel * channel );
            void RemoveChannel( Channel * channel );

            void RunInLoop( const PendingFunctor & f );
            void QueueInLoop( const PendingFunctor & f );

            void RemoveTimer( TimerId id );
            TimerId RunAfter( int interval, const TimerCallback & cb );

        private:
            void CallPendingFunctors();
            bool InLoopThread() const;
            void ProcessWakeUp();

        private:
            bool calling_functors_;
            boost::thread::id thread_id_;
            boost::scoped_ptr<Poller> poller_;
            boost::mutex call_functors_mutex_;
            typedef std::vector<PendingFunctor> FunctorList;
            FunctorList functors_;

            boost::scoped_ptr<Channel> wakeup_channel_;
            int wakeup_fds_[2];
            boost::atomic<bool> quit_;
            boost::scoped_ptr<TimerWheel> timer_mgr_;

            bool started_;
            uint64_t iteration_;

            static const int one;
    };
}

#endif   /* ----- #ifndef __FX_EVENT_LOOP_H__----- */
