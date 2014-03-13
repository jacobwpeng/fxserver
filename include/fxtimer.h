/*
 * =====================================================================================
 *       Filename:  fxtimer.h
 *        Created:  11:12:03 Mar 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  timer class
 *
 * =====================================================================================
 */

#ifndef  __FXTIMER_H__
#define  __FXTIMER_H__

class FXTimer
{
    public:
        typedef boost::function<void(void)> TimerCallback;
        typedef uint64_t TimerId;
        ~FXTimer();
        void Trigger();
        TimerId Id() const;
        uint64_t ExpireTime() const;

    private:
        FXTimer(TimerId id, uint64_t trigger_time, TimerCallback cb);
        TimerId id_;
        uint64_t trigger_time_;
        TimerCallback cb_;

        friend class FXTimerMgr;
};

#endif   /* ----- #ifndef __FXTIMER_H__----- */
