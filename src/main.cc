/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  11:30:15 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <glog/logging.h>
#include "fxserver.h"

int main(int argc, char * argv[])
{
    google::InitGoogleLogging(argv[0]);
    FXServer svrd;
    int ret = svrd.Init( 9026 );
    if( ret != 0 ) return ret;

    LOG(INFO) << "Init Done";
    svrd.Run();
}
