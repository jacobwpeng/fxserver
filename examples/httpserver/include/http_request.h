/*
 * =====================================================================================
 *       Filename:  http_request.h
 *        Created:  16:25:29 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */
#ifndef  __HTTP_REQUEST_H__
#define  __HTTP_REQUEST_H__

#include <map>
#include <string>
#include <vector>
#include <boost/optional/optional_fwd.hpp>

using std::map;
using std::string;
using boost::optional;

typedef std::vector<string> StringList;

class HTTPRequest
{
    public:
        HTTPRequest();

        bool AddHeader(const string& key, const string& val);
        string HTTPVersion() const;
        StringList HeaderNames() const;
        optional<string> GetHeader(const string& header_name) const;

        unsigned HeaderLength() const { return len_; }
        string request_type() const { return request_type_; }
        string request_path() const { return request_path_; }

        void set_header_length( unsigned len ) { len_ = len; }
        void set_major_version( unsigned major_version ) { major_version_ = major_version; }
        void set_minor_version( unsigned minor_version ) { minor_version_ = minor_version; }
        void set_request_type( const string& type ) { request_type_ = type; }
        void set_request_path( const string& path ) { request_path_ = path; }

    private:
        unsigned len_;
        unsigned major_version_;
        unsigned minor_version_;
        string request_type_;
        string request_path_;
        map<string, string> headers_;
};

typedef boost::optional<HTTPRequest> OptionalHTTPRequest;

#endif   /* ----- #ifndef __HTTP_REQUEST_H__----- */
