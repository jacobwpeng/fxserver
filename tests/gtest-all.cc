/*
 * =====================================================================================
 *       Filename:  gtest-all.cc
 *        Created:  10:39:48 Apr 22, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <gtest/gtest.h>
#include <glog/logging.h>

int main(int argc, char * argv[])
{
    ::google::InitGoogleLogging(argv[0]);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
