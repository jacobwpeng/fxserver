/*
 * =====================================================================================
 *
 *       Filename:  channel.h
 *        Created:  08/22/14 14:24:36
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __CHANNEL_H__
#define  __CHANNEL_H__

#include <vector>
#include <string>
#include <functional>

namespace fx
{
    namespace net
    {
        class EventLoop;
        class Channel
        {
            public:
                typedef std::function< void(void) > ReadCallback;
                typedef std::function< void(void) > WriteCallback;
                typedef std::function< void(void) > ErrorCallback;

            public:
                Channel(EventLoop * loop, int fd);
                ~Channel();

                EventLoop* loop() const { return loop_; }
                int fd() const { return fd_; }
                int events() const { return events_; }

                void Remove();
                void HandleEvents();
                std::string ReadableEvents() const;

                void set_revents(int revents) { revents_ = revents; }
                void EnableReading() { events_ |= kReadEvents; Update(); }
                void DisableReading() { events_ &= ~kReadEvents; Update(); }
                void EnableWriting() { events_ |= kWriteEvents; Update(); }
                void DisableWriting() { events_ &= ~kWriteEvents; Update(); }
                void DisableAll() { events_ = kNoneEvents; Update(); }

                void set_read_callback(const ReadCallback & rcb) { rcb_ = rcb; }
                void set_write_callback(const WriteCallback & wcb) { wcb_ = wcb; }
                void set_error_callback(const ErrorCallback & ecb) { ecb_ = ecb; }

            private:
                void Update();

            private:
                static const int kReadEvents;
                static const int kWriteEvents;
                static const int kNoneEvents;

                EventLoop * loop_;
                const int fd_;

                int events_;
                int revents_;
                ReadCallback rcb_;
                WriteCallback wcb_;
                ErrorCallback ecb_;
        };
        typedef std::vector<Channel*> ChannelList;
    }
}

#endif   /* ----- #ifndef __CHANNEL_H__  ----- */
