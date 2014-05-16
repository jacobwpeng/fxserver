/*
 * =====================================================================================
 *
 *       Filename:  http_module_request_validator.cc
 *        Created:  04/27/14 19:23:37
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_module_request_validator.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <glog/logging.h>

#include "http_server.h"
#include "http_utilities.h"

HTTPModuleRequestValidator::HTTPModuleRequestValidator(HTTPServer* server)
    :server_(server)
{
    Init();
}

HTTPModuleRequestValidator::~HTTPModuleRequestValidator()
{

}

HTTPResponsePtr HTTPModuleRequestValidator::Process(HTTPRequestPtr req)
{
    assert( req );
    HTTPResponsePtr res;
    if( req->minor_version() > 1 or req->major_version() > 1 )
    {
        LOG(WARNING) << "invalid http version -> " << res->HTTPVersion();
        return MakeResponse(400);
    }

    if( req->minor_version() == 1 and req->major_version() == 1 )
    {
        OptionalString opt_str = req->GetHeader("host");
        if( not opt_str ) 
        {
            LOG(WARNING) << "no Host header";
            return MakeResponse(400);
        }
    }

    string rpath = req->original_request_path();
    if( rpath.find("..") != string::npos or rpath.find("...") != string::npos )
    {
        LOG(WARNING) << "invalid path -> " << rpath;
        return MakeResponse(400);
    }

    if( req->request_type() != "GET" ) return MakeResponse(501);
    return res;
}

RetCode HTTPModuleRequestValidator::Init()
{
    server_->RegisterPreProcessing( 
            boost::bind(&HTTPModuleRequestValidator::Process, this, _1) );

    return kOk;
}
