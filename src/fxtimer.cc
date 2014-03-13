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
#include "fx_predefine.h"
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

uint64_t FXTimer::ExpireTime() const
{
    return trigger_time_;
}
