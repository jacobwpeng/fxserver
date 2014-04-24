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

#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "http_server.h"

using std::string;
using fx::EventLoop;
using fx::NetAddress;


int main(int argc, char * argv[])
{
    if( argc != 2 ) return 0;
    int port = boost::lexical_cast<int>( argv[1] );
    google::InitGoogleLogging(argv[0]);
    EventLoop loop;
    HTTPServer http_server(&loop, NetAddress("0.0.0.0", port), 8 );

    http_server.Run();
    loop.Run();
    return 0;
}
