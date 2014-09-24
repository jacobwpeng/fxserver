/*
 * =====================================================================================
 *
 *       Filename:  timermanager_unittest.cc
 *        Created:  09/24/14 15:12:50
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "timer_manager.h"
#include <functional>
#include <gtest/gtest.h>
#include "system_time.h"

using fx::net::TimerManager;

class TimerManagerTest : public ::testing::Test
{
    public:
        static void TimerCallback()
        {
            ++TimerManagerTest::times_;
        }

    protected:
        virtual void SetUp()
        {
            TimerManagerTest::times_ = 0;
        }

        virtual void TearDown()
        {
        }

        static int times_;
        TimerManager mgr_;
};

int TimerManagerTest::times_ = 0;

TEST_F(TimerManagerTest, TestRunAfter)
{
    const uint64_t kMilliseconds = 300;
    TimerManager::TimerCallback cb(TimerManagerTest::TimerCallback);
    TimerManager::TimerId id = mgr_.RunAfter(kMilliseconds, cb);
    ::usleep(kMilliseconds * 1000);
    TimerManager::TimerCallbackList callbacks = mgr_.Step(fx::base::time::Now());
    EXPECT_EQ(1u, callbacks.size());
}

TEST_F(TimerManagerTest, TestRunAt)
{
    fx::base::time::TimeStamp expire = fx::base::time::Now();
    const uint64_t kMilliseconds = 100;
    expire += kMilliseconds;

    TimerManager::TimerCallback cb(TimerManagerTest::TimerCallback);
    TimerManager::TimerId id = mgr_.RunAt(expire, cb);

    ::usleep(kMilliseconds * 1000);
    TimerManager::TimerCallbackList callbacks = mgr_.Step(fx::base::time::Now());
    EXPECT_EQ(1u, callbacks.size());
}

TEST_F(TimerManagerTest, TestRemove)
{
    const uint64_t kMilliseconds = 300;
    TimerManager::TimerCallback cb(TimerManagerTest::TimerCallback);
    TimerManager::TimerId id = mgr_.RunAfter(kMilliseconds, cb);
    bool ok = mgr_.Remove(id);
    EXPECT_TRUE (ok);
    ::usleep(kMilliseconds * 1000);
    TimerManager::TimerCallbackList callbacks = mgr_.Step(fx::base::time::Now());
    EXPECT_EQ(0u, callbacks.size());
}

TEST_F(TimerManagerTest, TestStep)
{
    const uint64_t kMilliseconds = 50;
    const int kLoopTimes = 20;
    int times = 0;
    mgr_.RunAfter(kMilliseconds, TimerManagerTest::TimerCallback);
    while (times != kLoopTimes)
    {
        ::usleep(kMilliseconds * 1000);
        auto callbacks = mgr_.Step(fx::base::time::Now());
        for (auto & cb : callbacks) { cb(); }
        mgr_.RunAfter(kMilliseconds, TimerManagerTest::TimerCallback);
        ++times;
    }
    EXPECT_EQ (TimerManagerTest::times_, kLoopTimes);
}
