/*
 * =====================================================================================
 *       Filename:  fx_event_loop.cc
 *        Created:  10:53:43 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_event_loop.h"
#include "fx_poller.h"
#include "fx_channel.h"
#include "fx_timer_wheel.h"

#include <unistd.h>
#include <fcntl.h>

#include <glog/logging.h>

namespace fx
{
    const int EventLoop::one = 1;
    EventLoop::EventLoop()
        :calling_functors_(false), quit_(false), started_(false), iteration_(0)
    {
        thread_id_ = boost::this_thread::get_id();
        poller_.reset( new Poller() );
        timer_mgr_.reset( new TimerWheel(this) );

        PCHECK( pipe(wakeup_fds_) == 0 ) << "make pipe failed!";
        fcntl( wakeup_fds_[0], F_SETFD, O_NONBLOCK);
        fcntl( wakeup_fds_[1], F_SETFD, O_NONBLOCK);

        wakeup_channel_.reset( new Channel( this, wakeup_fds_[0] ) );
        wakeup_channel_->set_read_callback( boost::bind( &EventLoop::ProcessWakeUp, this ) );

        wakeup_channel_->EnableReading();
    }

    EventLoop::~EventLoop()
    {
        close( wakeup_fds_[0] );
        close( wakeup_fds_[1] );
    }

    void EventLoop::Run()
    {
        AssertInLoopThread();
        started_ = true;

        ChannelList channels;
        TimerCallbackList timer_callbacks;

        TimeStamp start_time = base::Now();

        timer_mgr_->Start( start_time );
        InitiallyAdjustTimers();

        int timeout = timer_mgr_->NextTimeout( start_time );

        while( not quit_ )
        {
            TimeStamp now = poller_->Poll( timeout, &channels );
            ++iteration_;
            LOG(INFO) << "iteration_ = " << iteration_ 
                << ", channels.size() = " << channels.size();

            for( ChannelList::iterator iter = channels.begin(); iter != channels.end(); 
                    ++iter )
            {
                (*iter)->HandleEvents();
            }
            channels.clear();

            /* 调用pending functors */
            CallPendingFunctors();

            timer_mgr_->Step( now , &timer_callbacks );
            for( size_t idx = 0; idx != timer_callbacks.size(); ++idx )
            {
                timer_callbacks[idx]();
            }
            timer_callbacks.clear();

            timeout = timer_mgr_->NextTimeout( now );
        }
        LOG(INFO) << "EventLoop exiting...";
    }

    void EventLoop::Quit()
    {
        quit_ = true;
        if( not InLoopThread() ) WakeUp();
    }

    void EventLoop::RunInLoop( const PendingFunctor& f )
    {
        if( InLoopThread() )
        {
            f();
        }
        else
        {
            QueueInLoop(f);
        }
    }

    void EventLoop::QueueInLoop( const PendingFunctor & f )
    {
        if( InLoopThread() )
        {
            functors_.push_back( f );
        }
        else
        {
            /* 其它线程塞进来必须加锁 */
            boost::mutex::scoped_lock lock(call_functors_mutex_); 
            functors_.push_back( f );
        }
        if( InLoopThread() == false or calling_functors_ or not started_) WakeUp();
    }

    void EventLoop::RemoveTimer( TimerId id )
    {
        AssertInLoopThread();
        timer_mgr_->RemoveTimer( id );
        if( not started_ )
        {
            std::vector<TimerId>::iterator iter = non_adjusted_timers_.end();
            while( iter != non_adjusted_timers_.end() && *iter != id ) ++iter;

            assert( iter != non_adjusted_timers_.end() );
            non_adjusted_timers_.erase( iter ); /* TODO : better performance */
        }
    }

    TimerId EventLoop::RunAfter( int interval, const TimerCallback & cb )
    {
        AssertInLoopThread();
        assert( interval >= 0 );
        if( started_ )
        {
            TimeStamp expire_time = base::Now() + interval;
            return timer_mgr_->RunAt( expire_time , cb );
        }
        else
        {
            TimerId id = timer_mgr_->RunAfter( interval, cb );
            non_adjusted_timers_.push_back(id);
            return id;
        }
    }
    //TimerId EventLoop::RunAt( TimeStamp ts, const TimerCallback & cb )
    //{
    //    return timer_mgr_->RunAt( ts, cb );
    //}

    void EventLoop::UpdateChannel(Channel * channel)
    {
        channel->loop()->AssertInLoopThread();
        poller_->UpdateChannel( channel );
    }

    void EventLoop::RemoveChannel( Channel * channel )
    {
        channel->loop()->AssertInLoopThread();
        poller_->RemoveChannel( channel );
    }

    void EventLoop::AssertInLoopThread()
    {
        assert( InLoopThread() );
    }

    void EventLoop::CallPendingFunctors()
    {
        calling_functors_ = true;
        FunctorList functors;
        {
            boost::mutex::scoped_lock lock(call_functors_mutex_);
            functors.swap( functors_ );
        }

        for( FunctorList::iterator iter = functors.begin(); iter != functors.end(); ++iter )
        {
            (*iter)();
        }
        calling_functors_ = false;
    }

    bool EventLoop::InLoopThread() const
    {
        return thread_id_ == boost::this_thread::get_id();
    }

    void EventLoop::WakeUp()
    {
        PCHECK( write( wakeup_fds_[1], &one, sizeof(one) ) == sizeof(one) ) 
            << "wakup failed!" ;
    }

    void EventLoop::ProcessWakeUp()
    {
        int val;
        PCHECK( read( wakeup_fds_[0], &val, sizeof(val) ) == sizeof(one) ) << "ProcessWakeUp failed!" ;
    }

    void EventLoop::InitiallyAdjustTimers()
    {
        timer_mgr_->AdjustTimer( non_adjusted_timers_ );
    }
}
