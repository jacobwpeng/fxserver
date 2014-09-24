/*
 * =====================================================================================
 *
 *       Filename:  timer_manager_impl.cc
 *        Created:  09/24/14 12:58:58
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "timer_manager_impl.h"
#include <cassert>

namespace fx
{
    namespace net
    {
        TimerManager::TimerManager()
            :impl_(new TimerManager::Impl)
        {
        }

        TimerManager::~TimerManager()
        {
        }

        TimerManager::TimerId TimerManager::RunAfter(uint64_t milliseconds, const TimerCallback & cb)
        {
            fx::base::time::TimeStamp now = fx::base::time::Now();
            return RunAt(milliseconds + now, cb);
        }

        TimerManager::TimerId TimerManager::RunAt(fx::base::time::TimeStamp time, const TimerCallback & cb)
        {
            return impl_->RunAt(time, cb);
        }

        bool TimerManager::Remove(TimerManager::TimerId id)
        {
            return impl_->Remove(id);
        }

        TimerManager::TimerCallbackList TimerManager::Step(fx::base::time::TimeStamp now)
        {
            return impl_->Step(now);
        }


        /*-----------------------------------------------------------------------------
         *  Implementation details
         *-----------------------------------------------------------------------------*/

        TimerManager::Impl::Impl()
            :current_(0)
        {
        }

        TimerManager::Impl::~Impl()
        {
        }

        TimerManager::TimerId TimerManager::Impl::RunAt(fx::base::time::TimeStamp time, const TimerManager::TimerCallback & cb)
        {
            TimerPtr timer(new Timer);
            timer->id = current_;
            timer->cb = cb;
            timer->expire = time;
            timers_[time] = std::move(timer);
            expire_time_[current_] = time;
            return current_++;
        }

        bool TimerManager::Impl::Remove(TimerManager::TimerId id)
        {
            auto expire_iter = expire_time_.find(id);
            if (expire_iter == expire_time_.end()) return false;
            auto expire = expire_iter->second;
            assert (timers_.find(expire) != timers_.end());

            timers_.erase(expire);
            expire_time_.erase(expire_iter);
            return true;
        }

        TimerManager::TimerCallbackList TimerManager::Impl::Step(fx::base::time::TimeStamp now)
        {
            auto end = timers_.lower_bound(now);
            TimerManager::TimerCallbackList callbacks;
            for (auto iter = timers_.begin(); iter != end; ++iter)
            {
                callbacks.push_back(iter->second->cb);
                expire_time_.erase(iter->second->id);
            }
            timers_.erase(timers_.begin(), end);
            return callbacks;
        }
    }
}
