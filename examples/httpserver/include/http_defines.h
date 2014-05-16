/*
 * =====================================================================================
 *
 *       Filename:  http_defines.h
 *        Created:  04/27/14 19:26:27
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  type defines and const defs
 *
 * =====================================================================================
 */

#ifndef  __HTTP_DEFINES__
#define  __HTTP_DEFINES__

#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>

class HTTPRequest;
class HTTPResponse;

#define UNUSED(x) (void)(x)

typedef std::vector<std::string> StringList;
typedef boost::optional<std::string> OptionalString;

typedef boost::shared_ptr<HTTPRequest> HTTPRequestPtr;
typedef boost::shared_ptr<HTTPResponse> HTTPResponsePtr;

typedef boost::function< HTTPResponsePtr(HTTPRequestPtr) > PreProcessingFunc;
typedef boost::function< void(const HTTPRequestPtr&, HTTPResponsePtr) > ProcessingFunc;
typedef boost::function< void(const HTTPRequestPtr&, HTTPResponsePtr) > PostProcessingFunc;
typedef boost::function< void(int fd) > ZeroCopyWriteCallback;

enum RetCode 
{
    kOk = 0,
    kForbidden = 1,
    kNotFound = 2,
    kNotFile = 3,
    kInternalError = 4
};

#endif   /* ----- #ifndef __HTTP_DEFINES__  ----- */
