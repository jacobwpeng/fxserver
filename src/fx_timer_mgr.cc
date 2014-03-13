/*
 * =====================================================================================
 *       Filename:  fx_timer_mgr.cc
 *        Created:  11:29:51 Mar 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_predefine.h"

#include "fxutils.h"

#include "fxtimer.h"
#include "fx_timer_mgr.h"

FXTimerMgr::FXTimerMgr()
    :current_id_(0), started_(false), swapping_timers_(false)
{

}

void FXTimerMgr::Run()
{
    this->AdjustAllTimers();
    started_ = true;
}

unsigned FXTimerMgr::TriggerExpiredTimers(uint64_t now)
{
    assert( started_ = true );

    Time2TimersMap tmp;
    Time2TimersMap::iterator iter;
    Time2TimersMap::iterator e;
    std::vector<uint64_t> keys_to_remove;
    for( iter = time_to_timer_.begin(), e = time_to_timer_.upper_bound(now); iter != e; ++iter )
    {
        keys_to_remove.push_back( iter->first );
        tmp[iter->first].swap( iter->second );
    }

    BOOST_FOREACH(uint64_t key, keys_to_remove)
    {
        time_to_timer_.erase(key);
    }

    unsigned num = 0;

    for( iter = tmp.begin(), e = tmp.end(); iter != e; ++iter )
    {
        BOOST_FOREACH(const FXTimerSharedPtr& ptr_timer, iter->second)
        {
            ptr_timer->Trigger();
            ++num;
            id_to_timer_.erase( ptr_timer->Id() );
        }
    }

    return num;
}

boost::optional<uint64_t> FXTimerMgr::GetLatestExpireTime() const
{
    boost::optional<uint64_t> res;
    if( this->time_to_timer_.empty() == true ) return res;
    else return this->time_to_timer_.begin()->first;
}

void FXTimerMgr::AdjustAllTimers()
{
    assert( started_ == false );
    uint64_t now_in_milliseconds = NowInMilliSeconds();

    std::map<uint64_t, std::vector<FXTimerSharedPtr> > tmp;

    for( BOOST_AUTO(iter, time_to_timer_.begin());
                iter != time_to_timer_.end();
                ++iter
       )
    {
        tmp[iter->first + now_in_milliseconds].swap( iter->second );
    }

    time_to_timer_.swap( tmp );
}

TimerId FXTimerMgr::RunAfter(uint64_t milliseconds, TimerCallback cb)
{
    uint64_t now = NowInMilliSeconds();
    uint64_t trigger_time = now + milliseconds;
    TimerId id = current_id_;
    uint64_t timer_time = started_ ? trigger_time : milliseconds;

    FXTimerSharedPtr ptr_timer( new FXTimer(id, timer_time, cb) );

    FXTimerWeakPtr weak_ptr_timer(ptr_timer);

    ++current_id_;

    this->id_to_timer_[ ptr_timer->Id() ] = weak_ptr_timer;
    this->time_to_timer_[ timer_time ].push_back( ptr_timer );

    return id;
}
