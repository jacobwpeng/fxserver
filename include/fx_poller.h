/*
 * =====================================================================================
 *       Filename:  fx_poller.h
 *        Created:  10:46:13 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  Poller for IO multiplexing
 *
 * =====================================================================================
 */

#ifndef  __FX_POLLER_H__
#define  __FX_POLLER_H__

#include "fx_base.h"
#include "fx_channel.h"
#include <vector>
#include <map>

struct epoll_event;

namespace fx
{
    using base::TimeStamp;

    class Poller
    {
        public:
            Poller();
            ~Poller();

            TimeStamp Poll(int timeout_ms, ChannelList * active_channels);

            void UpdateChannel( Channel * channel );
            void RemoveChannel( Channel * channel );

        private:
            void FillActiveChannels(int nevents, ChannelList * active_channels);

        private:
            int epoll_fd_;
            static const int max_fd_count_;
            typedef std::vector<epoll_event> EventList;
            typedef std::map< int, Channel* > ChannelMap;
            ChannelMap channels_;
            EventList events_;
    };
};

#endif   /* ----- #ifndef __FX_POLLER_H__----- */
