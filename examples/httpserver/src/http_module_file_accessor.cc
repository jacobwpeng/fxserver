/*
 * =====================================================================================
 *       Filename:  http_module_file_accessor.cc
 *        Created:  11:03:48 May 12, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "http_module_file_accessor.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <glog/logging.h>

#include "http_server.h"
#include "http_utilities.h"

HTTPModuleFileAccessor::HTTPModuleFileAccessor(HTTPServer *server, 
                                                const string& base_path)
    :server_(server), base_path_(base_path), 
    zero_copy_file_min_size_( 1<<20 )           /* 1M */
{
    Init();
}

HTTPModuleFileAccessor::~HTTPModuleFileAccessor()
{
}

HTTPResponsePtr HTTPModuleFileAccessor::PreProcessing(HTTPRequestPtr req)
{
    assert( req );
    HTTPResponsePtr res;

    const StringList& candidates = req->request_path_candidates();
    assert( not candidates.empty() );

    bool candidate_found = false; 
    BOOST_FOREACH(const string& path, candidates)
    {
        RetCode ret = CheckFileCanRead( path );
        LOG(INFO) << "Check path(" << path << ") return " << ret;
        switch( ret )
        {
            case kOk:
                candidate_found = true;
                req->set_real_request_path(path);
                break;
            case kNotFound:
                /* in this case we continue searching */
                break;                          /* only break switch */
            case kForbidden:
                return MakeResponse(403);
            case kNotFile:
                return MakeResponse(400);           /* bad request */
            case kInternalError:
                return MakeResponse(500);
            default:
                assert(false);
        }
        if( candidate_found ) break;
    }
    if( not candidate_found ) return MakeResponse(404);
    else return res;
}

void HTTPModuleFileAccessor::ReadFile(const HTTPRequestPtr& req, 
        HTTPResponsePtr res)
{
    UNUSED(req);
    UNUSED(res);

    off_t byte_size = 0;
    RetCode ret = CheckFileCanRead(req->real_request_path(), &byte_size);
    assert( ret == kOk );

    string abs_path = base_path_ + req->real_request_path();
    int fd = open( abs_path.c_str(), O_RDONLY);
    if( fd < 0 )
    {
        PLOG(WARNING) << "open " << abs_path;
        return ;
    }

    if( byte_size >= static_cast<int>(zero_copy_file_min_size_) )
    {
        res->set_zero_copy_fd(fd);
        res->set_zero_copy_write_callback( boost::bind(
                    &HTTPModuleFileAccessor::OnZeroCopyWriteFinished, this, _1) );
    }
    else
    {
        char * buf = res->GetBodyBuf(byte_size);
        ssize_t s = pread(fd, buf, byte_size, 0u); /* for later cache fd */
        if( s < 0 )
        {
            PLOG(WARNING) << "read " << abs_path;
        }
        else
        {
            /* assume reading local file DOES NOT block*/
            /* and we read all in one call */
            assert( s == byte_size );
        }
        /* close fd after reading */
        close(fd);
        return;
    }
}

RetCode HTTPModuleFileAccessor::Init()
{
    server_->RegisterPreProcessing( 
            boost::bind(&HTTPModuleFileAccessor::PreProcessing, this, _1) );

    server_->RegisterProcessing(
            boost::bind(&HTTPModuleFileAccessor::ReadFile, this, _1, _2) );

    return kOk;
}

RetCode HTTPModuleFileAccessor::CheckFileCanRead(const std::string& file_path,
                                                off_t *byte_size)
{
    struct stat sb;
    string abs_path = base_path_ + file_path;
    LOG(INFO) << "abs_path = " << abs_path;
    int ret = stat(abs_path.c_str(), &sb);
    if( ret != 0 )
    {
        switch( errno )
        {
            case ENOENT:
            case ENAMETOOLONG:
            case ENOTDIR:
                return kNotFound;
            case EACCES:
                return kForbidden;
            default:
                return kInternalError;
        }
        PLOG(WARNING) << "stat";                /* for other errno */
    }

    if( S_ISREG(sb.st_mode) == false ) return kNotFile;

    uid_t uid = getuid();
    gid_t gid = getgid();

    mode_t read_mode_mask;
    if( uid == sb.st_uid ) read_mode_mask = S_IRUSR;
    else if( gid == sb.st_gid ) read_mode_mask = S_IRGRP;
    else read_mode_mask = S_IROTH;

    if( byte_size ) *byte_size = sb.st_size;

    return sb.st_mode & read_mode_mask ? kOk : kForbidden;
}

void HTTPModuleFileAccessor::OnZeroCopyWriteFinished(int fd)
{
    close(fd);
}
