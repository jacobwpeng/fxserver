/*
 * =====================================================================================
 *
 *       Filename:  poller.h
 *        Created:  08/22/14 14:40:03
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __POLLER_H__
#define  __POLLER_H__

#include "channel.h"
#include "system_time.h"
#include <memory>

namespace fx
{
    namespace net
    {
        class Poller
        {
            public:
                Poller();
                ~Poller();
                fx::base::time::TimeStamp Poll(int timeout, ChannelList * active_channels);

                void UpdateChannel(Channel * channel);
                void RemoveChannel(Channel * channel);

            private:
                class Impl;
                std::unique_ptr<Impl> impl_;
        };
    }
}

#endif   /* ----- #ifndef __POLLER_H__  ----- */
