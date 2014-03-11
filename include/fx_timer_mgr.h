/*
 * =====================================================================================
 *       Filename:  fx_timer_mgr.h
 *        Created:  11:27:22 Mar 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  timer manager
 *
 * =====================================================================================
 */

#ifndef  __FX_TIMER_MGR_H__
#define  __FX_TIMER_MGR_H__

#include "fxtimer.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>

typedef FXTimer::TimerId TimerId;
typedef FXTimer::TimerCallback TimerCallback;
typedef boost::shared_ptr<FXTimer> FXTimerSharedPtr;
typedef boost::weak_ptr<FXTimer> FXTimerWeakPtr;

class FXTimerMgr
{
    public:
        TimerId RunAfter(uint64_t milliseconds, TimerCallback cb);
        FXTimerMgr();

        void Run();
        unsigned TriggerExpiredTimers(uint64_t now);
        boost::optional<uint64_t> GetLatestExpireTime() const;

    private:
        void AdjustAllTimers();

        uint64_t current_id_;
        std::map<TimerId, FXTimerWeakPtr> id_to_timer_;
        std::map<uint64_t, std::vector<FXTimerSharedPtr> > time_to_timer_;

        bool started_;
};

#endif   /* ----- #ifndef __FX_TIMER_MGR_H__----- */
