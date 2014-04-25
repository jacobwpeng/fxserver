/*
 * =====================================================================================
 *       Filename:  http_request_parsing_state.h
 *        Created:  14:00:44 Apr 25, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  State machine for receving HTTP Request
 *
 * =====================================================================================
 */


#ifndef  __HTTP_REQUEST_PARSING_STATE_H__
#define  __HTTP_REQUEST_PARSING_STATE_H__

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include "http_response.h"

namespace fx{ class Buffer; }

enum ParseResult
{
    kParseOK = 0,
    kParseNeedMore,
    kParseError
};

class HTTPRequest;
class HTTPResponse;
struct HTTPRequestParsingState;
typedef boost::optional<HTTPResponse> OptionalHTTPResponse;
typedef boost::function< ParseResult( HTTPRequestParsingState*, fx::Buffer*) > StateFunc;

enum StateType
{
    kInit = 0,
    kParsingRequestLine,
    kParsingRequestHeader,
    kParsingRequestBody,
    kParsingDone
};

struct HTTPRequestParsingState
{
    StateType status;
    StateFunc func;
    HTTPRequest req;
    OptionalHTTPResponse res;
};

#endif   /* ----- #ifndef __HTTP_REQUEST_PARSING_STATE_H__----- */
