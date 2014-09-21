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

#include "process_bus_manager.h"
#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

using fx::base::ProcessBusManager;
const char * filename = "/tmp/ProcessBusManagerTest.TestInit.data";

class ProcessBusManagerTest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            manager_ = new ProcessBusManager();
        }

        virtual void TearDown()
        {
            delete manager_;
            manager_ = NULL;
        }

        int Init()
        {
            std::string xml = "<ProcessBus>"
                                  "<Bus id='10001' size='1000000' path='/tmp' />"
                                  "<Bus id='10002' size='2000000' path='/tmp' />"
                                  "<Bus id='10003' size='3000000' path='/tmp' />"
                                  "<Bus id='10004' size='4000000' path='/tmp' />"
                              "</ProcessBus>";

            std::ofstream ofs(filename, std::ios::binary | std::ios::out | std::ios::trunc);

            ofs << xml;
            ofs.close();

            return manager_->Init(filename);
        }

        ProcessBusManager * manager_;
};

TEST_F(ProcessBusManagerTest, TestInit)
{
    int ret = Init();
    EXPECT_TRUE (ret == 0);
}

TEST_F(ProcessBusManagerTest, TestListen)
{
    int ret = Init();
    ASSERT_TRUE (ret == 0);

    ret = manager_->Listen(10001);
    EXPECT_TRUE (ret == 0);

    ret = manager_->Listen(0);
    EXPECT_TRUE (ret != 0);
}

TEST_F(ProcessBusManagerTest, TestConnect)
{
    int ret = Init();
    ASSERT_TRUE (ret == 0);

    ret = manager_->Connect(10001);
    EXPECT_TRUE (ret == 0);

    ret = manager_->Connect(10005);
    EXPECT_TRUE (ret != 0);
}

TEST_F(ProcessBusManagerTest, TestWriteAndRead)
{
    int ret = Init();
    ASSERT_TRUE (ret == 0);

    ret = manager_->Listen(10001);
    ASSERT_TRUE (ret == 0);

    std::string message("Long live the queen!");
    ret = manager_->Write(10001, message.data(), message.size());
    ASSERT_TRUE (ret == 0);

    char * data = NULL;
    int len;

    ret = manager_->Read(10001, &data, &len);
    ASSERT_TRUE (data != NULL);
    EXPECT_TRUE ((size_t)len == message.size());
    std::string received(data, len);

    EXPECT_TRUE (received == message);
}

TEST_F(ProcessBusManagerTest, TestRecover)
{
    std::string message("for the lich king!!");
    {
        ProcessBusManager mgr;
        ASSERT_TRUE (0 == mgr.Init(filename));
        int ret = mgr.Listen(10003);
        ASSERT_TRUE (0 == ret);

        ASSERT_TRUE (0 == mgr.Write(10003, message.data(), message.size()));
    }
    int ret = Init();
    ASSERT_TRUE (ret == 0);

    ret = manager_->TryRecover(10003);
    ASSERT_TRUE (ret == 0);

    int len;
    char * data;
    ASSERT_TRUE(0 == manager_->Read(10003, &data, &len));
    ASSERT_TRUE (data != NULL);
    std::string received(data, len);
    EXPECT_TRUE (received == message);
}
