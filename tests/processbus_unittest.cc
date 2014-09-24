/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  10:23:27 Apr 22, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  test of NetAddress
 *
 * =====================================================================================
 */

#include "bus/process_bus.h"
#include <memory>

#include <gtest/gtest.h>

using fx::base::bus::ProcessBus;

class ProcessBusTest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
};

TEST_F(ProcessBusTest, TestCreateFrom)
{
    std::unique_ptr<ProcessBus> bus = std::move(fx::base::bus::ProcessBus::CreateFrom("/tmp/bus_10001.mmap", 1048576));
    EXPECT_TRUE (bus != NULL);
    EXPECT_EQ (bus->size(), 0u);
    int len;
    char * buf = bus->Read(&len);
    EXPECT_TRUE (buf == NULL);
}

TEST_F(ProcessBusTest, TestConnectTo)
{
    std::string msg("Long live the queen!");
    {
        std::unique_ptr<ProcessBus> bus = std::move(fx::base::bus::ProcessBus::ConnectTo("/tmp/bus_10002.mmap", 1 << 16));
        ASSERT_TRUE (bus != NULL);
        ASSERT_EQ (bus->size(), 0u);
        int len;
        char * buf = bus->Read(&len);
        EXPECT_TRUE (buf == NULL);
        bool ok = bus->Write(msg.data(), msg.size());
        ASSERT_TRUE (ok);
    }

    {
        std::unique_ptr<ProcessBus> bus = std::move(fx::base::bus::ProcessBus::ConnectTo("/tmp/bus_10002.mmap", 1 << 16));
        ASSERT_TRUE (bus != NULL);
        ASSERT_EQ (bus->size(), 1u);
        int len;
        char * buf = bus->Read(&len);
        ASSERT_TRUE (buf != NULL);
        EXPECT_EQ (std::string(buf, len), msg);
    }
}

TEST_F(ProcessBusTest, TestRestoreFrom)
{
    std::string msg("Long live the queen!");
    {
        std::unique_ptr<ProcessBus> bus = std::move(fx::base::bus::ProcessBus::CreateFrom("/tmp/bus_10003.mmap", 1 << 16));
        ASSERT_TRUE (bus != NULL);
        ASSERT_EQ (bus->size(), 0u);
        bool ok = bus->Write(msg.data(), msg.size());
        ASSERT_TRUE (ok);
    }
    {
        std::unique_ptr<ProcessBus> bus = std::move(fx::base::bus::ProcessBus::RestoreFrom("/tmp/bus_10003.mmap", 1 << 16));
        EXPECT_TRUE (bus != NULL);
        EXPECT_EQ (bus->size(), 1u);
        int len;
        char * buf = bus->Read(&len);
        ASSERT_TRUE (buf != NULL);
        EXPECT_EQ (std::string(buf, len), msg);
    }
}
