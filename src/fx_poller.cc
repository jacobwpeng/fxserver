/*
 * =====================================================================================
 *       Filename:  fx_poller.cc
 *        Created:  11:00:45 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_poller.h"

#include <sys/epoll.h>
#include <cassert>

namespace fx
{
    const int Poller::max_fd_count_ = 100;

    Poller::Poller()
    {
        epoll_fd_ = epoll_create(max_fd_count_);
        events_.resize( 20 );                   /* Ĭ�ϴ�С20 */
    }

    Poller::~Poller()
    {
        close( epoll_fd_ );
    }

    TimeStamp Poller::Poll(int timeout_ms, ChannelList * active_channels)
    {
        assert( active_channels != NULL );

        int nevents = epoll_wait(epoll_fd_, &events_[0], events_.size(), timeout_ms);
        TimeStamp now = base::Now();
        if( nevents > 0 )
        {
            FillActiveChannels( nevents, active_channels );
            if( static_cast<unsigned>(nevents) == events_.size() )
            {
                /* �������� */
                events_.resize( events_.size() * 2 );
            }
        }
        else if( nevents == 0 )
        {
            /* ɶ��û������Ŀ�ⳬʱ */
        }
        else
        {
            /* �������� */
            assert( false );
        }
        return now;
    }

    void Poller::UpdateChannel(Channel * channel)
    {
        ChannelMap::iterator iter = channels_.find( channel->fd() );

        if( iter == channels_.end() )
        {
            /* ����µ�channel */
            epoll_event ev;
            ev.data.fd = channel->fd();
            ev.events = channel->events();
            epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &ev);

            channels_[ channel->fd() ] = channel;
        }
        else
        {
            /* �����Ѿ����ڵ�channel */
            epoll_event ev;
            ev.data.fd = channel->fd();
            ev.events = channel->events();

            epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->fd(), &ev);
        }
    }

    void Poller::RemoveChannel( Channel * channel )
    {
        ChannelMap::iterator iter = channels_.find( channel->fd() );
        assert( iter != channels_.end() );

        channels_.erase( iter );

        epoll_event ev;
        ev.data.fd = channel->fd();
        epoll_ctl( epoll_fd_, EPOLL_CTL_DEL, channel->fd(), &ev );
    }

    void Poller::FillActiveChannels(int nevents, ChannelList * active_channels)
    {
        for( int idx = 0; idx < nevents; ++idx )
        {
            int fd = events_[idx].data.fd;
            ChannelMap::iterator iter = channels_.find( fd );
            assert( iter != channels_.end() );

            Channel * channel = iter->second;
            channel->set_revents( events_[idx].events );

            active_channels->push_back( channel );
        }
    }
};
