/*
 * =====================================================================================
 *       Filename:  fx_event_loop_thread_pool.h
 *        Created:  14:22:09 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_EVENT_LOOP_THREAD_POOL_H__
#define  __FX_EVENT_LOOP_THREAD_POOL_H__

#include <vector>
#include <boost/scoped_array.hpp>

namespace fx
{
    class EventLoop;
    class EventLoopThread;
    class EventLoopThreadPool
    {
        public:
            EventLoopThreadPool(EventLoop * base_loop);
            ~EventLoopThreadPool();

            void SetLoopThreadCount(unsigned loop_thread_count);
            void Start();

            EventLoop * NextLoop();

        private:
            EventLoop * base_loop_;
            unsigned loop_thread_count_;
            unsigned next_;

            std::vector< EventLoop* > loops_;
            boost::scoped_array< EventLoopThread > threads_;
    };
}

#endif   /* ----- #ifndef __FX_EVENT_LOOP_THREAD_POOL_H__----- */
