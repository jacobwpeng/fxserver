/*
 * =====================================================================================
 *
 *       Filename:  http_module_request_validator.h
 *        Created:  04/27/14 19:14:11
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  validator for http request
 *                  check protocal related headers only
 *
 * =====================================================================================
 */


#ifndef  __HTTP_MODULE_REQUEST_VALIDATOR__
#define  __HTTP_MODULE_REQUEST_VALIDATOR__

#include "http_defines.h"
class HTTPServer;

class HTTPModuleRequestValidator
{
    public:
        HTTPModuleRequestValidator(HTTPServer * server);
        ~HTTPModuleRequestValidator();

        HTTPResponsePtr Process(HTTPRequestPtr req);

    private:
        void Init();

    private:
        HTTPServer * server_;
};

#endif   /* ----- #ifndef __HTTP_MODULE_REQUEST_VALIDATOR__  ----- */
