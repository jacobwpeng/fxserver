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

#include <boost/bind.hpp>
#include <glog/logging.h>

#include "fx_event_loop.h"
#include "fx_acceptor.h"
#include "fx_blocking_queue.hpp"
#include "fx_tcp_server.h"

using namespace fx;

void ThreadFunc(int port)
{
    EventLoop loop;

    Acceptor acceptor(&loop);
    acceptor.BindOrAbort( "0.0.0.0", port );

    loop.RunInLoop( boost::bind( &Acceptor::Listen, &acceptor) );

    loop.Run();
}

int main(int argc, char * argv[])
{
    google::InitGoogleLogging(argv[0]);
    const size_t thread_count = 4;
    (void)thread_count;

    EventLoop loop;

    TcpServer s( &loop, "0.0.0.0", 9026 );
    s.SetThreadNum( thread_count );
    s.Start();

    loop.Run();

    //boost::thread_group event_loop_threads;
    //int base_port = 9026;
    //for( size_t idx = 0; idx != thread_count; ++idx )
    //{
    //    event_loop_threads.create_thread( boost::bind(ThreadFunc, base_port) );
    //}
    //event_loop_threads.join_all();
    return 0;
}
