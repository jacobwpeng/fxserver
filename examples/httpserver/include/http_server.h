/*
 * =====================================================================================
 *       Filename:  http_server.h
 *        Created:  16:52:15 Apr 24, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __HTTP_SERVER_H__
#define  __HTTP_SERVER_H__

#include "fx_event_loop.h"
#include "fx_net_address.h"
#include "fx_tcp_connection.h"
#include "fx_tcp_server.h"
#include "http_defines.h"
#include "http_codec.h"
#include "http_module_request_validator.h"
#include "http_module_request_rewrite.h"
#include "http_module_file_accessor.h"

#include <vector>
#include <boost/scoped_ptr.hpp>

class HTTPRequest;
class HTTPResponse;
using std::vector;
using fx::TcpConnectionPtr;
using fx::TcpConnectionWeakPtr;
using fx::EventLoop;
using fx::NetAddress;
using fx::TcpServer;

typedef vector<PreProcessingFunc> PreProcessingFuncList;
typedef vector<ProcessingFunc> ProcessingFuncList;
typedef vector<PostProcessingFunc> PostProcessingFuncList;

class HTTPServer
{
    public:
        HTTPServer(EventLoop * loop, const NetAddress & bind_addr, size_t thread_num);
        RetCode Run();
        void OnRequest(TcpConnectionPtr conn, const HTTPRequest & req );
        void OnParseHeaderError(TcpConnectionPtr conn, const HTTPResponse & res );

        void RegisterPreProcessing(PreProcessingFunc f)
        { pre_processing_funcs_.push_back( f ); }

        void RegisterProcessing(ProcessingFunc f)
        { processing_funcs_.push_back(f); }

        void RegisterPostProcessing(PostProcessingFunc f)
        { post_processing_funcs_.push_back(f); }

    private:
        RetCode InitModules();
        void WriteResponseAndClose(TcpConnectionPtr conn, const HTTPResponse& res);
        void WriteResponseDone(TcpConnectionWeakPtr weak_conn);
    private:
        EventLoop * loop_;
        NetAddress addr_;
        size_t thread_num_;
        boost::scoped_ptr<TcpServer> tcp_server_;
        boost::scoped_ptr<HTTPCodec> codec_;
        boost::scoped_ptr<HTTPModuleRequestValidator> request_validator_;
        boost::scoped_ptr<HTTPModuleRequestRewrite> request_rewriter_;
        boost::scoped_ptr<HTTPModuleFileAccessor> file_accessor_;

        PreProcessingFuncList pre_processing_funcs_;
        ProcessingFuncList processing_funcs_;
        PostProcessingFuncList post_processing_funcs_;
};

#endif   /* ----- #ifndef __HTTP_SERVER_H__----- */
