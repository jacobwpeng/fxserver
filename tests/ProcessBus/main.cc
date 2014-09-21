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

#include "process_bus.h"
#include <iostream>
#include <vector>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gtest/gtest.h>

using fx::base::ProcessBus;

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

TEST_F(ProcessBusTest, TestCreateFromPtree)
{
    using boost::property_tree::ptree;
    ptree pt;

    std::string xml = "<ProcessBus>"
                          "<Bus id='10004' size='3000000' path='/tmp' />"
                      "</ProcessBus>";

    std::istringstream iss(xml);
    boost::property_tree::read_xml(iss, pt);

    ProcessBus bus(pt.get_child("ProcessBus.Bus"));
    int ret = bus.Listen();
    EXPECT_TRUE (ret == 0);
    EXPECT_TRUE (bus.size() == 0);
}

TEST_F(ProcessBusTest, TestListen)
{
    ProcessBus bus(10001, 10 * (1<<20), "/tmp");
    int ret = bus.Listen();
    EXPECT_TRUE (ret == 0);
    EXPECT_TRUE (bus.size() == 0);
}

TEST_F(ProcessBusTest, TestConnect)
{
    {
        ProcessBus bus(10002, 2000000, "/tmp");
        ASSERT_TRUE (bus.Listen() == 0);
        char c;
        ASSERT_EQ (true, bus.Write(&c, sizeof(c)));
    }
    ProcessBus bus(10002, 2000000, "/tmp");
    int ret = bus.Connect();
    EXPECT_TRUE (ret == 0);
    EXPECT_TRUE (bus.size() == 1);
}
