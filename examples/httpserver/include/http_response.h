/*
 * =====================================================================================
 *       Filename:  http_response.h
 *        Created:  16:33:19 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_RESPONSE_H__
#define  __HTTP_RESPONSE_H__

#include <string>
#include <map>

using std::map;
using std::string;

class HTTPResponse
{
    public:
        HTTPResponse(unsigned status);

        //void SetHTTPVersion( unsigned major_version, unsigned minor_version )
        //{
        //    major_version_ = major_version;
        //    minor_version_ = minor_version;
        //}
        bool AddHeader(const string& key, const string& val);

        string HTTPVersion() const;

    private:
        friend class HTTPCodec;

    private:
        unsigned status_;
        unsigned major_version_;
        unsigned minor_version_;
        map<string, string> headers_;
};

#endif   /* ----- #ifndef __HTTP_RESPONSE_H__----- */
