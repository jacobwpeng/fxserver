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

#include "fx_slice.h"

#include <string>
#include <gtest/gtest.h>

using fx::Slice;

TEST(SliceTest, TestConstructorUsingCString)
{
    const char * s = "vasdnfi2389h23\n\r\t9h3fj290";
    Slice slice(s, strlen(s)) ;

    EXPECT_EQ(slice.empty(), false);
    EXPECT_EQ(slice.data(), s);
    EXPECT_EQ(slice.size(), strlen(s));
}

TEST(SliceTest, TestConstructorUsingStdString)
{
    std::string s("20748623489ndjasbvjabdja");
    Slice slice(s) ;

    EXPECT_EQ(slice.empty(), false);
    EXPECT_EQ(slice.data(), s.c_str());
    EXPECT_EQ(slice.size(), s.size());
}

TEST(SliceTest, Testto_string)
{
    std::string s("learn yourself a haskell for a great good!!!");
    Slice slice(s);

    EXPECT_TRUE( Slice::to_string(slice) == s );
}

TEST(SliceTest, TestOperatorEqualAndEquals)
{
    const char * src = "i dont wanna talk to you...";
    std::string s(src);
    Slice slice(src, strlen(src));

    EXPECT_FALSE( slice == Slice(s) );
    EXPECT_TRUE( slice.Equals(Slice(s)) );
}

TEST(SliceTest, Testsubslice)
{
    const char * src = "let there be light!";

    Slice slice(src, strlen(src));

    Slice sub = slice.subslice( 0, 3 );

    EXPECT_TRUE( Slice::to_string(sub) == "let" );

    Slice left = slice.subslice( slice.find("there be") );
    EXPECT_TRUE( Slice::to_string(left) == "there be light!" );
}

TEST(SliceTest, Testassign)
{
    std::string s("The garbage collection handbook is awesome!!!!!");
    Slice slice;
    EXPECT_TRUE( slice.empty() );

    slice.assign(s);

    EXPECT_FALSE( slice.empty() );
}

TEST(SliceTest, TestFind)
{
    std::string src("BBC ABCDAB ABCDABCDABDE");

    Slice slice(src);

    string p("ABCDABD");

    EXPECT_TRUE( src.find(p) == slice.find(p.c_str()) );
    slice.clear();

    EXPECT_TRUE( slice.empty() );

    src = "1212121312121214";
    p = "12121214";
    slice.assign( src );

    EXPECT_TRUE( src.find(p) == slice.find(p.c_str()) );
}
