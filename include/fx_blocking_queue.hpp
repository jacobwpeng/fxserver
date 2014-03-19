/*
 * =====================================================================================
 *       Filename:  fx_blocking_queue.hpp
 *        Created:  17:46:59 Mar 18, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  ADT for Producer and consumer Design Mode which is thread safe
 *
 * =====================================================================================
 */

#ifndef  __FX_BLOCKING_QUEUE_HPP__
#define  __FX_BLOCKING_QUEUE_HPP__

#include "fx_predefine.h"

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

template<typename T>
class FXBlockingQueue : boost::noncopyable
{
    public:
        FXBlockingQueue();
        ~FXBlockingQueue();

        T Pop();
        void Push(const T& );

    private:
        std::queue<T> internal_queue_;
        boost::mutex queue_op_mutex_;
        boost::condition_variable cond_var_;
};

template<typename T>
FXBlockingQueue<T>::FXBlockingQueue()
{
}

template<typename T>
FXBlockingQueue<T>::~FXBlockingQueue()
{
}

template<typename T>
T FXBlockingQueue<T>::Pop()
{
    boost::mutex::scoped_lock lock(queue_op_mutex_);
    while( internal_queue_.empty() ) cond_var_.wait(lock);

    T tmp(internal_queue_.front());
    internal_queue_.pop();

    return tmp;
}

template<typename T>
void FXBlockingQueue<T>::Push(const T& t)
{
    {
        boost::mutex::scoped_lock lock(queue_op_mutex_);
        internal_queue_.push( t );
    }
    cond_var_.notify_one();
}

#endif   /* ----- #ifndef __FX_BLOCKING_QUEUE_HPP__----- */
