/*
 * =====================================================================================
 *
 *       Filename:  http_file_accessor.h
 *        Created:  04/26/14 19:25:34
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_FILE_ACCESSOR__
#define  __HTTP_FILE_ACCESSOR__

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include "fx_slice.h"

using std::string;
using fx::Slice;

typedef boost::shared_ptr<Slice> SlicePtr;

enum AccessState
{
    kAccessOK = 0,
    kNotFound = 1,
    kCannotAccess = 2,
};

class FileAccessor
{
    public:
        FileAccessor(const string& base_path, size_t max_cacheable_size = kMaxCacheableSize);

        /* relative path based on base_path_ */
        AccessState CheckFile(const string& rpath);
        AccessState CheckDir(const string& rpath);

        bool HasFileCache(const string& rpath);
        SlicePtr GetFileCache(const string& rpath);
        SlicePtr GetFile(const string& rpath);

    private:
        bool TryUpdateFileCache(const string& rpath, const SlicePtr& slice);
        SlicePtr ReadFile(const string& rpath);

    private:
        static const size_t kMaxCacheableSize = 1 << 20;

        string base_path_;
        /* rpath -> cache */
        map<string, SlicePtr> cache_map_;
};

const string& rpath = req.GetPath();
if( file_accessor_->HasFileCache(rpath) )
{
    SlicePtr cache = file_accessor_->GetFileCache(rpath);
    WriteResponse(conn, cache);
}
else
{
    boost::bind( &HTTPServer::ReadFileInWorkerThread, this, rpath, conn );
}

//<HTTPRequest, HTTPResponse> ctx = conn->context()

//void ReadFileInWorkerThread(const string& rpath, TcpConnectionPtr conn)
//{
//    SlicePtr cache = file_accessor_->GetFile(rpath);
//    <HTTPRequestPtr, HTTPResponsePtr> ctx = conn->context();
//    ctx.second->set_body(cache);
//    conn->loop()->RunInLoop( boost::bind( &HTTPServer::WriteResponse, conn) );
//}
//
//void WriteResponse(TcpConnectionPtr conn, SlicePtr cache)
//{
//    HTTPResponse res;
//    res.set_body( cache.data(), cache.size() );
//
//    conn->Write( codec_->EncodeResponse(res) );
//}

#endif   /* ----- #ifndef __HTTP_FILE_ACCESSOR__  ----- */

