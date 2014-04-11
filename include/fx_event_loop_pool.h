/*
 * =====================================================================================
 *       Filename:  fx_event_loop_pool.h
 *        Created:  09:40:39 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_EVENT_LOOP_POOL_H__
#define  __FX_EVENT_LOOP_POOL_H__

#include <boost/scoped_array.hpp>

namespace fx
{
    class EventLoop;
    class EventLoopPool
    {
        public:
            EventLoopPool( EventLoop * base_loop );
            ~EventLoopPool();

            void SetLoopCount( unsigned loop_count );

            EventLoop * NextLoop();

            EventLoop * loop(unsigned idx) { assert( idx < loop_count_ ); return &loops_[idx]; }

        private:
            EventLoop * base_loop_;
            unsigned loop_count_;
            unsigned next_;

            boost::scoped_array<EventLoop> loops_;
    };
}

#endif   /* ----- #ifndef __FX_EVENT_LOOP_POOL_H__----- */
