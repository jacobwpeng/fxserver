/*
 * =====================================================================================
 *       Filename:  fx_event_loop_thread.cc
 *        Created:  14:07:08 Apr 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_event_loop_thread.h"
#include "fx_event_loop.h"
#include <glog/logging.h>

namespace fx
{
    EventLoopThread::EventLoopThread()
        :loop_(NULL)
    {

    }

    EventLoopThread::~EventLoopThread()
    {

    }

    EventLoop * EventLoopThread::StartLoop()
    {
        assert( thread_ == NULL );
        thread_.reset( new boost::thread( boost::bind(&EventLoopThread::CreateLoopAndRun, this) ) ); 

        {
            boost::mutex::scoped_lock lock( mutex_ );
            while( loop_ == NULL )
            {
                cond_.wait( lock );
            }
        }
        return loop_;
    }

    boost::thread * EventLoopThread::thread()
    {
        return thread_.get();
    }

    void EventLoopThread::CreateLoopAndRun()
    {
        assert( loop_ == NULL );
        EventLoop loop;
        {
            boost::mutex::scoped_lock lock( mutex_ );
            loop_ = &loop;
        }
        cond_.notify_one();
        loop_->Run();
    }
}
