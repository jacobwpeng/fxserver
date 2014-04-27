/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  15:27:51 Apr 23, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "http_server.h"

using std::string;
using fx::EventLoop;
using fx::NetAddress;

void Daemonize()
{
    pid_t id = fork();
    if( id < 0 ) { exit(EXIT_FAILURE); }
    else if( id > 0 ) { exit(EXIT_SUCCESS); }
    else
    {
        id = fork();
        if( id < 0 ) { exit(EXIT_FAILURE); }
        else if( id > 0 ) { exit(EXIT_SUCCESS); }
        else{}
    }
}

int main(int argc, char * argv[])
{
    //Daemonize();
    if( argc != 2 ) return 0;
    int port = boost::lexical_cast<int>( argv[1] );
    google::InitGoogleLogging(argv[0]);
    EventLoop loop;
    HTTPServer http_server(&loop, NetAddress("0.0.0.0", port), 8 );

    http_server.Run();
    loop.Run();
    return 0;
}
