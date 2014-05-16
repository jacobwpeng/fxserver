/*
 * =====================================================================================
 *
 *       Filename:  http_utilities.h
 *        Created:  04/27/14 19:00:17
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  utilities
 *
 * =====================================================================================
 */

#ifndef  __HTTP_UTILITIES__
#define  __HTTP_UTILITIES__

#include <string>
#include <vector>
#include "http_defines.h"

using std::string;
using std::vector;

namespace boost { namespace posix_time { class ptime; } }

typedef vector<string> StringList;

void SplitString( const string& src, const string& sep, StringList * res);
string Rfc1123TimeNow();
HTTPResponsePtr MakeResponse( unsigned http_ret_code );

#endif   /* ----- #ifndef __HTTP_UTILITIES__  ----- */
