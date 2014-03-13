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

#include "fx_predefine.h"

#include "fxbuffer.h"
#include "fxconnection.h"
#include "fxtimer.h"
#include "fx_timer_mgr.h"
#include "fxserver.h"

void MyMessageCallback( FXConnectionPtr & conn )
{
    std::string msg;
    LOG(INFO) << "read from client, addr=[" << conn->PeerAddress() << "], msg=[" << msg << "]";
    conn->Write("Not in Service\n");
    conn->Close();
}

void MyConnectionCallback( FXConnectionPtr & conn )
{
    LOG(INFO) << "New Connection, addr=" << conn->PeerAddress();
}

int main(int argc, char * argv[])
{
    if( argc != 2 ) return -1;
    google::InitGoogleLogging(argv[0]);

    unsigned port = 0;
    try { port = boost::lexical_cast<unsigned>(argv[1]); } catch(...) { return -2; }

    FXServer svrd;
    int ret = svrd.Init( port );
    if( ret != 0 ) return ret;

    svrd.SetConnectionCallback( MyConnectionCallback );
    svrd.SetMessageCallback( MyMessageCallback );

    LOG(INFO) << "Init Done";
    svrd.Run();

    return 0;
}
