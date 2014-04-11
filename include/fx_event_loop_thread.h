/*
 * =====================================================================================
 *       Filename:  fx_event_loop_thread.h
 *        Created:  14:03:04 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_EVENT_LOOP_THREAD_H__
#define  __FX_EVENT_LOOP_THREAD_H__

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace fx
{
    class EventLoop;
    class EventLoopThread
    {
        public:
            EventLoopThread();
            ~EventLoopThread();

            EventLoop * StartLoop();
            boost::thread * thread();

        private:
            void CreateLoopAndRun();

        private:
            EventLoop * loop_;
            boost::thread * thread_;
            boost::mutex mutex_;
            boost::condition_variable cond_;
    };
}

#endif   /* ----- #ifndef __FX_EVENT_LOOP_THREAD_H__----- */
