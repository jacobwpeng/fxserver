/*
 * =====================================================================================
 *       Filename:  http_module_request_rewrite.cc
 *        Created:  10:48:04 May 13, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_module_request_rewrite.h"

#include <boost/foreach.hpp>
#include "http_server.h"

HTTPModuleRequestRewrite::HTTPModuleRequestRewrite(HTTPServer *server)
    :server_(server)
{
    Init();
}

HTTPModuleRequestRewrite::~HTTPModuleRequestRewrite()
{
}

HTTPResponsePtr HTTPModuleRequestRewrite::ResolveRequestPathCandidates(
                                                            HTTPRequestPtr req)
{
    HTTPResponsePtr res;
    /* if original_request_path ends with a slash
     * then we consider it as a directory access
     * */
    string original_request_path = req->original_request_path();
    assert( not original_request_path.empty() );

    char last_char = *original_request_path.rbegin();
    if( last_char == '/' )
    {
        BOOST_FOREACH(const string& suffix, dir_access_suffixes_)
        {
            /* TODO : add all suffixes in one call ? */
            req->add_request_path_candidate(suffix);
        }
        /* TODO : add compressed extensions for dir access files */
    }
    else
    {
        /* TODO : avoid add extension when request pre-compressed files */
        BOOST_FOREACH(const string& extension, compressed_file_extensions_)
        {
            req->add_request_path_candidate( 
                    req->original_request_path() + extension );
        }
        req->add_request_path_candidate( req->original_request_path() );
    }
    return res;
}

RetCode HTTPModuleRequestRewrite::Init()
{
    /* TODO : more default dir access suffixes */
    dir_access_suffixes_.push_back( "/index.html" );
    /* compressed file extensions */
    compressed_file_extensions_.push_back(".gz");

    server_->RegisterPreProcessing( 
            boost::bind(&HTTPModuleRequestRewrite::ResolveRequestPathCandidates, 
                            this, _1) );
    return kOk;
}
