/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  10:23:27 Apr 22, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  test of NetAddress
 *
 * =====================================================================================
 */

#include "net_address.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <gtest/gtest.h>

using fx::net::NetAddress;

TEST(NetAddressTest, ConstructorUsingAddrAndPort)
{
    NetAddress addr("0.0.0.0", 9026);
    EXPECT_EQ( addr.port(), 9026 );
    EXPECT_EQ( addr.ip_addr(), "0.0.0.0" );
    EXPECT_EQ( addr.ToString(), "0.0.0.0:9026" );
}

TEST(NetAddressTest, ConstructorUsingsockaddr_in)
{
    sockaddr_in sa;
    memset(&sa, 0x0, sizeof(sa));

    sa.sin_family = AF_INET;
    sa.sin_port = htons(9999);
    sa.sin_addr.s_addr = inet_addr("1.2.4.8");

    NetAddress addr( sa );

    EXPECT_EQ( addr.port(), 9999 );
    EXPECT_EQ( addr.ip_addr(), "1.2.4.8" );
    EXPECT_EQ( addr.ToString(), "1.2.4.8:9999" );
}

TEST(NetAddressTest, Tosockaddr_in)
{
    NetAddress addr( "114.114.114.114", 53 );

    sockaddr_in sa;
    memset(&sa, 0x0, sizeof(sa));

    sa.sin_family = AF_INET;
    sa.sin_port = htons(53);
    inet_pton( AF_INET, "114.114.114.114", &sa.sin_addr);

    sockaddr_in out = addr.ToSockAddr();

    EXPECT_EQ( out.sin_port, sa.sin_port );
    EXPECT_EQ( out.sin_addr.s_addr, sa.sin_addr.s_addr );
}
