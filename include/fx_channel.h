/*
 * =====================================================================================
 *       Filename:  fx_channel.h
 *        Created:  10:44:49 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  event channel
 *
 * =====================================================================================
 */

#ifndef  __FX_CHANNEL_H__
#define  __FX_CHANNEL_H__

#include <vector>
#include <boost/function.hpp>

namespace fx
{
    class EventLoop;
    typedef boost::function< void(void) > ChannelReadCallback;
    typedef boost::function< void(void) > ChannelWriteCallback;
    typedef boost::function< void(void) > ChannelErrorCallback;
    class Channel
    {
        public:
            Channel(EventLoop * loop, int fd);
            ~Channel();

            EventLoop* loop() const { return loop_; }
            int fd() const { return fd_; }
            int events() const { return events_; }

            void HandleEvents();

            /* 事件相关 */
            void set_revents( int revents ) { revents_ = revents; }
            void EnableReading() { events_ |= kReadEvents; Update(); }
            void DisableReading() { events_ &= ~kReadEvents; Update(); }
            void EnableWriting() { events_ |= kWriteEvents; Update(); }
            void DisableWriting() { events_ &= ~kWriteEvents; Update(); }
            void DisableAll() { events_ = kNoneEvents; Update(); }

            /* 回调相关 */
            void set_read_callback( ChannelReadCallback rcb ) { rcb_ = rcb; }
            void set_write_callback( ChannelWriteCallback wcb ) { wcb_ = wcb; }
            void set_error_callback( ChannelErrorCallback ecb ) { ecb_ = ecb; }

        private:
            void Update();
            void Remove();

        private:
            EventLoop * loop_;
            const int fd_;

            static const int kReadEvents;
            static const int kWriteEvents;
            static const int kNoneEvents;

            int events_;
            int revents_;
            ChannelReadCallback rcb_;
            ChannelWriteCallback wcb_;
            ChannelErrorCallback ecb_;
    };
    typedef std::vector<Channel*> ChannelList;
}

#endif   /* ----- #ifndef __FX_CHANNEL_H__----- */
