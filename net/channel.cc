/*
 * =====================================================================================
 *
 *       Filename:  channel.cc
 *        Created:  08/22/14 15:13:44
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "channel.h"

#include <sys/epoll.h>
#include <glog/logging.h>
#include <sstream>

#include "event_loop.h"

namespace fx
{
    namespace net
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
        }

        void Channel::Remove()
        {
            loop_->RemoveChannel(this);
        }

        void Channel::Update()
        {
            loop_->UpdateChannel(this);
        }

        void Channel::HandleEvents()
        {
            if (revents_ & (EPOLLERR | EPOLLHUP))
            {
                if (ecb_) ecb_();
                else
                {
                    LOG(WARNING) << "Got error for fd = " << fd_;
                }
            }

            if (revents_ & Channel::kWriteEvents)
            {
                if (wcb_) wcb_();
            }

            if (revents_ & Channel::kReadEvents)
            {
                if (rcb_) rcb_();
            }
        }

        std::string Channel::ReadableEvents() const
        {
            std::ostringstream oss;
            if (revents_ & EPOLLIN) oss << "EPOLLIN ";
            if (revents_ & EPOLLOUT) oss << "EPOLLOUT ";
            if (revents_ & EPOLLRDHUP) oss << "EPOLLRDHUP ";
            if (revents_ & EPOLLPRI) oss << "EPOLLPRI ";
            if (revents_ & EPOLLERR) oss << "EPOLLERR ";
            if (revents_ & EPOLLHUP) oss << "EPOLLHUP ";
            if (revents_ & EPOLLET) oss << "EPOLLET ";

            return oss.str();
        }
    }
}
