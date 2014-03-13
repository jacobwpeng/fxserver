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
#include "fxutils.h"
#include "fxserver.h"

static const unsigned interval = 1000;


using std::string;
using std::vector;

void MyTimerCallback( const FXConnectionPtr & conn, const string& msg )
{
    conn->Write(msg);
    conn->Server()->TimerManager().RunAfter(interval, boost::bind(MyTimerCallback, conn, msg) );
}

void MyMessageCallback( const FXConnectionPtr & conn )
{
    string msg( conn->MutableReadBuffer()->ReadAndClear() );
    string key;
    if( boost::ends_with(msg, "\r\n") )
    {
        key = msg.substr( 0, msg.length() - 2 );
    }
    LOG(INFO) << "read from client, addr=[" << conn->PeerAddress() << "], msg=[" << msg << "]";
    if( key == "quit" )
    {
        conn->Write("Bye Bye.\r\n");
        conn->Close();
    }
    else if( key == "repeat" )
    {
        conn->Server()->TimerManager().RunAfter(interval, boost::bind(MyTimerCallback, conn, msg) );
    }
    else
    {
        conn->Write(msg);
    }
}

void MyConnectionCallback( const FXConnectionPtr & conn )
{
    LOG(INFO) << "New Connection, addr=" << conn->PeerAddress() << ", fd = " << conn->FileDescriptor();
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
