/*
 * =====================================================================================
 *       Filename:  fx_event_loop_pool.cc
 *        Created:  09:42:56 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_event_loop_pool.h"
#include "fx_event_loop.h"

#include <glog/logging.h>

namespace fx
{
    EventLoopPool::EventLoopPool( EventLoop * base_loop )
        :base_loop_( base_loop ), loop_count_( 0 ), next_(0)
    {
    }

    EventLoopPool::~EventLoopPool()
    {

    }

    void EventLoopPool::SetLoopCount( unsigned loop_count )
    {
        if( loop_count == 0 ) return;
        else
        {
            loops_.reset( new EventLoop[loop_count] );
            loop_count_ = loop_count;
        }
    }

    EventLoop * EventLoopPool::NextLoop()
    {
        if( loop_count_ == 0 ) return base_loop_;
        else
        {
            EventLoop * loop = &loops_[ next_ ];
            ++next_;
            if( next_ == loop_count_ ) next_ = 0;

            return loop;
        }
    }
}
