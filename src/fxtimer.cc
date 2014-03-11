/*
 * =====================================================================================
 *       Filename:  fxtimer.cc
 *        Created:  11:19:05 Mar 11, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  timer class
 *
 * =====================================================================================
 */

#include "fxtimer.h"

FXTimer::FXTimer(TimerId id, uint64_t trigger_time, TimerCallback cb)
    :id_(id), trigger_time_(trigger_time), cb_(cb)
{

}

FXTimer::~FXTimer()
{

}

void FXTimer::Trigger()
{
    cb_();
}

FXTimer::TimerId FXTimer::Id() const
{
    return id_;
}
