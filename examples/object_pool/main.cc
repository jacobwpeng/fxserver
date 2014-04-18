/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  13:45:53 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  example usage of fx::ObjectPool
 *
 * =====================================================================================
 */

#include "fx_object_pool.hpp"
#include "fx_base.h"
#include <sys/time.h>
#include <iostream>
using namespace fx;
using fx::base::ProgressTimer;

const unsigned kMaxTimes = 1 << 22;

void test_object_pool()
{
    int ** nums = new int*[kMaxTimes];
    std::cout << __PRETTY_FUNCTION__ << '\n';
    {
        ProgressTimer t;
        ObjectPool<int> int_pool;
        for( unsigned idx = 0; idx != kMaxTimes; ++idx )
        {
            nums[idx] = int_pool.Construct();
            *nums[idx] = idx + 1;
        }

        for( unsigned idx = 0; idx != kMaxTimes; ++idx )
        {
            int_pool.Destroy( nums[idx] );
        }
    }
    delete [] nums;
}

void test_new_delete()
{
    int ** nums = new int*[kMaxTimes];
    std::cout << __PRETTY_FUNCTION__ << '\n';
    {
        ProgressTimer t;
        for( unsigned idx = 0; idx != kMaxTimes; ++idx )
        {
            nums[idx] = new int;
            *nums[idx] = idx + 1;
        }

        for( unsigned idx = 0; idx != kMaxTimes; ++idx )
        {
            delete nums[idx];
        }
    }
    delete [] nums;
}

int main(int argc, char * argv[])
{
    (void)argc; (void)argv;
    test_object_pool();
    test_new_delete();
}
