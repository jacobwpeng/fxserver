/*
 * =====================================================================================
 *       Filename:  http_module_file_accessor.h
 *        Created:  10:59:02 May 12, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_MODULE_FILE_ACCESSOR_H__
#define  __HTTP_MODULE_FILE_ACCESSOR_H__

#include "http_defines.h"
class HTTPServer;

class HTTPModuleFileAccessor
{
    public:
        HTTPModuleFileAccessor(HTTPServer *server, const std::string& base_path);
        ~HTTPModuleFileAccessor();

        HTTPResponsePtr PreProcessing(HTTPRequestPtr req);
        void ReadFile(const HTTPRequestPtr& req, HTTPResponsePtr res);

    private:
        RetCode Init();
        RetCode CheckFileCanRead(const std::string& file_path, 
                                        off_t *byte_size = NULL);
        void OnZeroCopyWriteFinished(int fd);

    private:
        HTTPServer *server_;
        const std::string base_path_;
        const size_t zero_copy_file_min_size_;        /* in bytes */
};

#endif   /* ----- #ifndef __HTTP_MODULE_FILE_ACCESSOR_H__----- */
