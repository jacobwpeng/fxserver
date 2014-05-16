/*
 * =====================================================================================
 *
 *       Filename:  http_utilities.cc
 *        Created:  04/27/14 19:04:49
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_utilities.h"
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "http_response.h"

void SplitString( const string& src, const string& sep, StringList * res)
{
    typedef boost::split_iterator<string::const_iterator> StringSplitIterator;

    StringSplitIterator iter = boost::make_split_iterator( 
                                    src, boost::first_finder(sep, boost::is_equal() )
                                );
    StringSplitIterator end;

    while( iter != end )
    {
        res->push_back( boost::copy_range<string>(*iter) );
        ++iter;
    }
}

string Rfc1123TimeNow()
{
    std::stringstream ss;
    using namespace boost::posix_time;
    time_facet *facet = new time_facet("%a, %d %b %Y %H:%M:%S GMT");
    ss.imbue( std::locale( ss.getloc(), facet ) );
    ss << second_clock::universal_time();
    return ss.str();
}

HTTPResponsePtr MakeResponse( unsigned http_ret_code )
{
    return boost::make_shared<HTTPResponse>(http_ret_code);
}
