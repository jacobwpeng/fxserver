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
#include <iosfwd>

using std::map;
using std::string;
using std::ostream;
using boost::optional;

typedef std::vector<string> StringList;

class HTTPRequest
{
    public:
        HTTPRequest();

        bool AddHeader(const string& key, const string& val);
        string HTTPVersion() const;
        unsigned major_version() const { return major_version_; }
        unsigned minor_version() const { return minor_version_; }
        StringList HeaderNames() const;
        optional<string> GetHeader(const string& header_name) const;

        unsigned header_length() const { return header_length_; }
        unsigned body_length() const { return body_length_; }
        string request_type() const { return request_type_; }
        string original_request_path() const { return original_request_path_; }
        string real_request_path() const { return real_request_path_; }
        const StringList& request_path_candidates() const 
        { return request_path_candidates_; }

        void set_header_length( unsigned header_length ) 
        { header_length_ = header_length; }

        void set_body_length( unsigned body_length ) 
        { body_length_ = body_length; }

        void set_body( const char * buf, size_t len ) 
        { body_.assign( buf, len ); }

        void set_major_version( unsigned major_version ) 
        { major_version_ = major_version; }

        void set_minor_version( unsigned minor_version ) 
        { minor_version_ = minor_version; }

        void set_request_type( const string& type ) { request_type_ = type; }
        void set_real_request_path(const string& path)
        { real_request_path_ = path; }

        void set_original_request_path( const string& path ) 
        { original_request_path_ = path; }

        void set_static_gzipped() { static_gzipped = true; }
        void set_dynamic_gzipped() { dynamic_gzipped = true; }
        void add_request_path_candidate(const string& path)
        { request_path_candidates_.push_back( path ); }

    private:
        unsigned header_length_;
        unsigned body_length_;
        unsigned major_version_;
        unsigned minor_version_;
        string body_;
        string request_type_;
        string original_request_path_;
        string real_request_path_;
        StringList request_path_candidates_;
        map<string, string> headers_;

        bool static_gzipped;
        bool dynamic_gzipped;
};

typedef boost::optional<HTTPRequest> OptionalHTTPRequest;
ostream & operator<< (ostream& os, const HTTPRequest & req );

#endif   /* ----- #ifndef __HTTP_REQUEST_H__----- */
