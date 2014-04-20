/*
 * =====================================================================================
 *       Filename:  fx_channel.cc
 *        Created:  11:11:33 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_channel.h"

#include <sys/epoll.h>
#include "fx_event_loop.h"
#include <glog/logging.h>

namespace fx
{
    const int Channel::kReadEvents = EPOLLIN;
    const int Channel::kWriteEvents = EPOLLOUT;
    const int Channel::kNoneEvents = 0;

    Channel::Channel(EventLoop * loop, int fd)
        :loop_(loop), fd_(fd), events_(0), revents_(0)
    {

    }

    Channel::~Channel()
    {
        Remove();
    }

    void Channel::HandleEvents()
    {
        if( revents_ & (EPOLLERR|EPOLLHUP) )
        {
            LOG(WARNING) << "Got error for fd = " << fd_;
            if( ecb_ ) ecb_();
        }

        if( revents_ & (EPOLLOUT|EPOLLWRNORM) )
        {
            if( wcb_ ) wcb_();
        }

        if( revents_ & (EPOLLIN|EPOLLRDNORM) )
        {
            if( rcb_ ) rcb_();
        }
        PrintEvents(revents_);
        revents_ = 0;
    }

    void Channel::Update()
    {
        loop_->UpdateChannel( this );
    }

    void Channel::Remove()
    {
        loop_->RemoveChannel( this );
    }

    void Channel::PrintEvents(int events_mask)
    {
        std::string s;
        if (events_mask & EPOLLIN) s += "EPOLLIN ";
        if (events_mask & EPOLLPRI) s += "EPOLLPRI ";
        if (events_mask & EPOLLOUT) s += "EPOLLOUT ";
        if (events_mask & EPOLLRDNORM) s += "EPOLLRDNORM ";
        if (events_mask & EPOLLRDBAND) s += "EPOLLRDBAND ";
        if (events_mask & EPOLLWRNORM) s += "EPOLLWRNORM ";
        if (events_mask & EPOLLWRBAND) s += "EPOLLWRBAND ";
        if (events_mask & EPOLLMSG) s += "EPOLLMSG ";
        if (events_mask & EPOLLERR) s += "EPOLLERR ";
        if (events_mask & EPOLLHUP) s += "EPOLLHUP ";
        if (events_mask & EPOLLONESHOT) s += "EPOLLONESHOT ";
        if (events_mask & EPOLLET) s += "EPOLLET ";
        LOG(INFO) << "events: " << s;
    }
}
