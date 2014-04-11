/*
 * =====================================================================================
 *       Filename:  fx_event_loop_thread_pool.cc
 *        Created:  14:26:08 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_event_loop_thread_pool.h"
#include "fx_event_loop_thread.h"
#include <glog/logging.h>

namespace fx
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop * base_loop)
        :base_loop_(base_loop), loop_thread_count_(0), next_(0)
    {

    }

    EventLoopThreadPool::~EventLoopThreadPool()
    {
    }

    void EventLoopThreadPool::SetLoopThreadCount(unsigned loop_thread_count)
    {
        loop_thread_count_ = loop_thread_count;
    }
    
    void EventLoopThreadPool::Start()
    {
        if( loop_thread_count_ != 0 )
        {
            threads_.reset( new EventLoopThread[ loop_thread_count_ ] );

            for( unsigned idx = 0; idx < loop_thread_count_; ++idx )
            {
                loops_.push_back( threads_[idx].StartLoop() );
            }
        }
    }

    EventLoop * EventLoopThreadPool::NextLoop()
    {
        if( loop_thread_count_ == 0 ) return base_loop_;
        else
        {
            unsigned idx = next_;
            ++next_;
            if( next_ == loop_thread_count_ ) next_ = 0;
            assert( idx < loops_.size() );
            return loops_[idx];
        }
    }
}
