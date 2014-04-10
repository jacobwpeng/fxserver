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

namespace fx
{
    const int Channel::kReadEvents = EPOLLIN;
    const int Channel::kWriteEvents = EPOLLOUT;
    const int Channel::kNoneEvents = 0;

    Channel::Channel(EventLoop * loop, int fd)
        :loop_(loop), fd_(fd)
    {

    }

    Channel::~Channel()
    {
        Remove();
    }

    void Channel::HandleEvents()
    {
        if( revents_ & EPOLLERR )
        {
            if( ecb_ ) ecb_();
        }

        if( revents_ & EPOLLIN )
        {
            if( rcb_ ) rcb_();
        }

        if( revents_ & EPOLLOUT )
        {
            if( wcb_ ) wcb_();
        }
    }

    void Channel::Update()
    {
        loop_->UpdateChannel( this );
    }

    void Channel::Remove()
    {
        loop_->RemoveChannel( this );
    }
}
