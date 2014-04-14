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

#include <unistd.h>
#include <fcntl.h>

#include <glog/logging.h>

namespace fx
{
    const int EventLoop::one = 1;
    EventLoop::EventLoop()
        :calling_functors_(false), quit_(false)
    {
        thread_id_ = boost::this_thread::get_id();
        poller_.reset( new Poller() );

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
        ChannelList channels;

        while( not quit_ )
        {
            TimeStamp now = poller_->Poll( -1, &channels );
            (void)now;

            for( ChannelList::iterator iter = channels.begin(); iter != channels.end(); ++iter )
            {
                (*iter)->HandleEvents();
            }
            channels.clear();

            /* 调用pending functors */
            CallPendingFunctors();
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
            boost::mutex::scoped_lock lock(call_functors_mutex_); /* 其它线程塞进来必须加锁 */
            functors_.push_back( f );
        }
        if( InLoopThread() == false or calling_functors_ ) WakeUp();
    }

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
        PCHECK( write( wakeup_fds_[1], &one, sizeof(one) ) == sizeof(one) ) << "wakup failed!" ;
    }

    void EventLoop::ProcessWakeUp()
    {
        int val;
        PCHECK( read( wakeup_fds_[0], &val, sizeof(val) ) == sizeof(one) ) << "ProcessWakeUp failed!" ;
        LOG(INFO) << "wakeup, thread id = " << thread_id_;
    }
}
