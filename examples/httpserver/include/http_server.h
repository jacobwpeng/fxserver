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

#include <vector>
#include <boost/scoped_ptr.hpp>

class HTTPRequest;
class HTTPResponse;
class HTTPModuleRequestValidator;
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
        void Run();
        void OnRequest(TcpConnectionPtr conn, const HTTPRequest & req );
        void OnParseHeaderError(TcpConnectionPtr conn, const HTTPResponse & res );

        void RegisterPreProcessing(PreProcessingFunc f)
        { pre_processing_funcs_.push_back( f ); }

        void RegisterProcessing(ProcessingFunc f)
        { processing_funcs_.push_back(f); }

        void RegisterPostProcessing(PostProcessingFunc f)
        { post_processing_funcs_.push_back(f); }

    private:
        void WriteResponseAndClose(TcpConnectionPtr conn, const HTTPResponse& res);
        void WriteResponseDone(TcpConnectionWeakPtr weak_conn);
    private:
        EventLoop * loop_;
        NetAddress addr_;
        size_t thread_num_;
        boost::scoped_ptr<TcpServer> tcp_server_;
        boost::scoped_ptr<HTTPCodec> codec_;
        boost::scoped_ptr<HTTPModuleRequestValidator> request_validator_;

        PreProcessingFuncList pre_processing_funcs_;
        ProcessingFuncList processing_funcs_;
        PostProcessingFuncList post_processing_funcs_;
};

#endif   /* ----- #ifndef __HTTP_SERVER_H__----- */
