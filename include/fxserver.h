/*
 * =====================================================================================
 *       Filename:  fxserver.h
 *        Created:  10:47:41 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  fast and extendable tcp server
 *
 * =====================================================================================
 */

#ifndef  __FXSERVER_H__
#define  __FXSERVER_H__

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "fxbuffer.h"

class FXServer : boost::noncopyable
{
    public:
        FXServer();
        ~FXServer();
        int Init(int port);
        void Run();

        void OnConnect(int fd);
        void OnMessage(int fd);
        void OnWritable(int fd);
        void OnClose(int fd);

    private:
        void SetNonblocking(int fd);
        void NotifyWritable(int fd);
        void NotifyReadable(int fd);

    private:
        static const int max_fd_count = 100;
        std::map<int, FXBuffer > buf_map_;
        int listen_fd_;
        int epoll_fd_;
};

#endif   /* ----- #ifndef __FXSERVER_H__----- */
