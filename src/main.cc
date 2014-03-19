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
#include "fx_blocking_queue.hpp"
#include "fxserver.h"

#include <boost/thread.hpp>

static const unsigned interval = 1000;


using std::string;
using std::vector;

typedef FXBlockingQueue<unsigned> UQueue;

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

void ConsumerThreadFunc( UQueue & q )
{
    while(1)
    {
        LOG(INFO) << " id = " << boost::this_thread::get_id() << " val = " << q.Pop();
    }
}

void ProducerThreadFunc( UQueue & q )
{
    unsigned num = 0;
    while( num < 50 ) q.Push( num++ );
}

int RunServer(int argc, char * argv[])
{
    if( argc != 2 ) return -1;
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

int RunBlockingQueueTest(int argc, char * argv[])
{
    static const unsigned consumer_count = 8;
    static const unsigned producer_count = 4;
    boost::thread_group consumer_threads;
    boost::thread_group producer_threads;

    UQueue q;

    for( unsigned i = 0; i != consumer_count; ++i )
    {
        consumer_threads.create_thread( boost::bind(ConsumerThreadFunc, boost::ref(q)) );
    }

    for( unsigned i = 0; i != producer_count; ++i )
    {
        producer_threads.create_thread( boost::bind(ProducerThreadFunc, boost::ref(q)) );
    }

    consumer_threads.join_all();
    producer_threads.join_all();
    return 0;
}

int main(int argc, char * argv[])
{
    google::InitGoogleLogging(argv[0]);

    return RunBlockingQueueTest(argc, argv);
}
