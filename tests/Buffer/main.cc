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

#include "fx_buffer.h"

#include <string>
#include <gtest/gtest.h>

using fx::Buffer;

TEST(BufferTest, TestConstructor)
{
    size_t buf_len = 1 << 18;
    Buffer buf( buf_len );
    EXPECT_EQ( buf_len, buf.ByteSize() );
}

TEST(BufferTest, TestRead)
{
    std::string content( "this is a very very very very long sentence" );
    Buffer buf;
    buf.Append( content.c_str(), content.length() );

    EXPECT_EQ( buf.BytesToRead(), content.length() );

    buf.ConsumeBytes( 10 );
    EXPECT_EQ( buf.BytesToRead(), content.length() - 10 );

    std::string left( buf.ReadBegin(), buf.BytesToRead() );
    EXPECT_EQ( left, "very very very very long sentence" );

    std::string all = buf.ReadAndClear();
    EXPECT_EQ( left, all );

    EXPECT_EQ( buf.BytesToRead(), 0u );
}

TEST(BufferTest, TestWrite)
{
    std::string sentence("me again!!!");
    std::string content( "another very very very very long sentence" );
    size_t buf_len = 1024;
    Buffer buf( buf_len );
    buf.Append( content.c_str(), content.length() );

    EXPECT_EQ( buf.BytesCanWrite(), buf_len - content.length() );

    memcpy( buf.WriteBegin(), sentence.c_str(), sentence.length() );
    buf.AddBytes( sentence.length() );

    EXPECT_EQ( buf.BytesCanWrite(), buf_len - content.length() - sentence.length() );

    std::string useless_bytes( 1 << 16, 'c' );
    buf.Append( useless_bytes.c_str(), useless_bytes.length() );

    EXPECT_GT( buf.ByteSize(), buf_len );

    buf.Clear();

    EXPECT_EQ( buf.BytesToRead(), 0u );
    EXPECT_EQ( buf.BytesCanWrite(), buf.ByteSize() );
}
