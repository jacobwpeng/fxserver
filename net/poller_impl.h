/*
 * =====================================================================================
 *
 *       Filename:  poller_impl.h
 *        Created:  09/22/14 21:45:12
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  Poller implementation
 *
 * =====================================================================================
 */

#include <map>
#include <vector>
#include <boost/noncopyable.hpp>

#include "poller.h"

struct epoll_event;

namespace fx
{
    namespace net
    {
        class Poller::Impl : boost::noncopyable
        {
            public:
                Impl();
                ~Impl();

                fx::base::time::TimeStamp Poll(int timeout, ChannelList * active_channels);

                void UpdateChannel(Channel * channel);
                void RemoveChannel(Channel * channel);

            private:
                typedef std::vector<epoll_event> EventList;
                typedef std::map<int, Channel*> ChannelMap;
                void FillActiveChannels(int nevents, ChannelList * active_channels);

            private:
                const static int kMaxFdCount = 100;
                int epoll_fd_;
                ChannelMap channels_;
                EventList events_;
        };
    }
}
