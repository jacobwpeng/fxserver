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
#include <iostream>

using fx::Slice;

void PrintMatchPos(const string& str, const string& pattern)
{
    Slice s(str);
    size_t pos = s.find(pattern.c_str());
    if( pos != Slice::npos )
    {
        std::cout << str << '\n';
        std::cout << string(pos, ' ') + '|' << '\n';
        std::cout << string(pos, ' ') + pattern << '\n';
    }
}

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
    string p("ABCDABD");

    Slice slice(src);
    
    PrintMatchPos(src, p);

    EXPECT_TRUE( src.find(p) == slice.find(p.c_str()) );
    slice.clear();

    EXPECT_TRUE( slice.empty() );

    src = "ABAAAABAAAAAAAAA";
    p = "BAAAAAAAAA";
    slice.assign( src );

    PrintMatchPos(src, p);
    EXPECT_TRUE( src.find(p) == slice.find(p.c_str()) );

    src = "agctagcagctagct";
    p = "agctagct";
    slice.assign( src );

    PrintMatchPos(src, p);
    EXPECT_EQ( src.find(p), slice.find(p.c_str()) );

    src = "12345";
    p = "6789";
    slice.assign( src );

    PrintMatchPos(src, p);
    EXPECT_TRUE( slice.find("6789") == Slice::npos );
}
