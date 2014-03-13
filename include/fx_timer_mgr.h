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
        typedef std::map<TimerId, FXTimerWeakPtr> Id2TimerMap;
        typedef std::map<uint64_t, std::vector<FXTimerSharedPtr> > Time2TimersMap;
        Id2TimerMap id_to_timer_;
        Time2TimersMap time_to_timer_;

        bool started_;
        bool swapping_timers_;
};

#endif   /* ----- #ifndef __FX_TIMER_MGR_H__----- */
