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

#include <string>
#include <glog/logging.h>
#include "fxserver.h"
#include "fxconnection.h"

void MyMessageCallback( FXConnectionPtr & conn )
{
    std::string msg( conn->ReadBuffer().Read(), conn->ReadBuffer().BytesToRead() );
    LOG(INFO) << "read from client, msg=[" << msg << "]";
    conn->Write("Not in Service\n");
    conn->Close();
}

void MyConnectionCallback( FXConnectionPtr & conn )
{
    LOG(INFO) << "New Connection, fd=" << conn->FileDescriptor();
}

int main(int argc, char * argv[])
{
    google::InitGoogleLogging(argv[0]);
    FXServer svrd;
    int ret = svrd.Init( 9026 );
    if( ret != 0 ) return ret;

    svrd.SetConnectionCallback( MyConnectionCallback );
    svrd.SetMessageCallback( MyMessageCallback );

    LOG(INFO) << "Init Done";
    svrd.Run();
}
