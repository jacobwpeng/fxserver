/*
 * =====================================================================================
 *       Filename:  http_module_request_rewrite.h
 *        Created:  10:44:00 May 13, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_MODULE_REQUEST_REWRITE_H__
#define  __HTTP_MODULE_REQUEST_REWRITE_H__

#include "http_defines.h"

class HTTPServer;
class HTTPModuleRequestRewrite
{
    public:
        HTTPModuleRequestRewrite(HTTPServer *server);
        ~HTTPModuleRequestRewrite();

        HTTPResponsePtr ResolveRequestPathCandidates(HTTPRequestPtr req); 

    private:
        RetCode Init();

    private:
        HTTPServer *server_;
        StringList dir_access_suffixes_;
        StringList compressed_file_extensions_;
};

#endif   /* ----- #ifndef __HTTP_MODULE_REQUEST_REWRITE_H__----- */
