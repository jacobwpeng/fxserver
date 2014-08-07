/*
 * =====================================================================================
 *       Filename:  fx_timer_wheel.h
 *        Created:  15:59:33 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_TIMER_WHEEL_H__
#define  __FX_TIMER_WHEEL_H__

#include <map>
#include <vector>
#include <queue>
#include <boost/function.hpp>
#include <boost/intrusive/list.hpp>

#include "fx_base.h"
#include "fx_object_pool.hpp"
namespace fx
{
    namespace bi = boost::intrusive;
    typedef bi::list_base_hook< bi::link_mode< bi::auto_unlink> > auto_unlink_hook;
    typedef uint64_t TimerId;
    typedef boost::function<void(void)> TimerCallback;
    typedef std::vector< TimerCallback > TimerCallbackList;

    using base::TimeStamp;

    namespace detail
    {
        struct Timer : public auto_unlink_hook
        {
            TimerId id;
            TimeStamp expire_time;
            TimerCallback cb;
        };
        typedef std::pair<unsigned, unsigned> SlotPos;
    }

    typedef bi::list<detail::Timer, bi::constant_time_size<false> > TimerList;
    typedef std::map<TimerId, detail::Timer*> TimerMap;

    class EventLoop;
    class TimerWheel
    {
        public:
            TimerWheel(EventLoop * loop);
            ~TimerWheel();

            int NextTimeout( TimeStamp now ); /* in milliseconds */

            void Start( TimeStamp now );
            void Step( TimeStamp now, TimerCallbackList * callbacks );

            TimerId RunAfter( int interval, TimerCallback cb );
            TimerId RunAt( TimeStamp expire_time, TimerCallback cb );
            void RemoveTimer( TimerId id );

        private:
            detail::SlotPos FindPos( int interval, bool update_hands = false, bool * overflow = NULL);
            void SwitchHands();

        private:

            static const unsigned kWheelCount = 5;
            static const unsigned kWheelSlotOffset[ kWheelCount ];
            static const unsigned kWheelSlotCount[ kWheelCount ];

            ObjectPool<detail::Timer> timer_pool_;
            EventLoop * loop_;
            unsigned hands_idx_;
            unsigned * hands_;
            unsigned hands_copy_[2][ kWheelCount ];
            TimerId last_timer_id_;
            TimeStamp last_expire_time_;
            TimerList * wheels_[kWheelCount];
            TimerMap timers_;
            typedef std::priority_queue<TimeStamp, std::vector<TimeStamp>, std::greater<TimeStamp> > ExpireTimeQueue;
            ExpireTimeQueue expire_time_queue_;
    };

}

#endif   /* ----- #ifndef __FX_TIMER_WHEEL_H__----- */
