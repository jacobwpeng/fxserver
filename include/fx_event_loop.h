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

namespace fx
{
    class Poller;
    class Channel;
    typedef boost::function< void(void) > PendingFunctor;
    class EventLoop : boost::noncopyable
    {
        public:
            EventLoop();
            ~EventLoop();

            void AssertInLoopThread();

            void Run();
            void WakeUp();
            void UpdateChannel( Channel * channel );
            void RemoveChannel( Channel * channel );

            void RunInLoop( const PendingFunctor & f );
            void QueueInLoop( const PendingFunctor & f );

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

            static const int one;

    };
}

#endif   /* ----- #ifndef __FX_EVENT_LOOP_H__----- */
