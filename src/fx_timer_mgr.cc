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

#include "fx_timer_mgr.h"
#include <sys/time.h>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/typeof/typeof.hpp>
#include <glog/logging.h>
#include "fxutils.hpp"

FXTimerMgr::FXTimerMgr()
    :current_id_(0), started_(false)
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
    BOOST_AUTO(e, time_to_timer_.upper_bound(now) );
    BOOST_AUTO(iter, time_to_timer_.begin() );

    std::vector<uint64_t> keys_to_remove;

    unsigned num = 0;

    while( iter != e )
    {
        BOOST_FOREACH(FXTimerSharedPtr& ptr_timer, iter->second)
        {
            ++num;
            ptr_timer->Trigger();
            id_to_timer_.erase( ptr_timer->Id() );
        }
        keys_to_remove.push_back( iter->first );
        ++iter;
    }

    BOOST_FOREACH(uint64_t key, keys_to_remove)
    {
        time_to_timer_.erase(key);
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
    uint64_t trigger_time = NowInMilliSeconds() + milliseconds;
    TimerId id = current_id_;
    uint64_t timer_time = started_ ? trigger_time : milliseconds;

    FXTimerSharedPtr ptr_timer( new FXTimer(id, timer_time, cb) );

    FXTimerWeakPtr weak_ptr_timer(ptr_timer);

    ++current_id_;

    this->id_to_timer_[ ptr_timer->Id() ] = weak_ptr_timer;
    this->time_to_timer_[ timer_time ].push_back( ptr_timer );

    return id;
}
