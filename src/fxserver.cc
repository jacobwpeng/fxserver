/*
 * =====================================================================================
 *       Filename:  fxserver.cc
 *        Created:  10:52:03 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  fast and extendable tcp server
 *
 * =====================================================================================
 */

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cassert>
#include <algorithm>
#include <boost/typeof/typeof.hpp>
#include <glog/logging.h>

#include "fxserver.h"

FXServer::FXServer()
{

}

FXServer::~FXServer()
{

}

int FXServer::Init(int port)
{
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    int enable_reuse_addr = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable_reuse_addr, sizeof(enable_reuse_addr) );

    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(listen_fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout) );

    sockaddr_in server_addr;
    memset(&server_addr, 0x0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(listen_fd_, (sockaddr *)(&server_addr), sizeof(server_addr) );
    if( ret < 0 )
    {
        LOG(ERROR) << "bind failed, ret=" << ret << ", errno=" << errno << ", msg=" << strerror(errno);
        close(listen_fd_);
        return ret;
    }

    this->SetNonblocking(listen_fd_);

    epoll_fd_ = epoll_create(max_fd_count);

    epoll_event ev;
    ev.data.fd = listen_fd_;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &ev);

    ret = listen(listen_fd_, SOMAXCONN);
    if( ret < 0 )
    {
        LOG(ERROR) << "listen failed, ret=" << ret << ", errno=" << errno << ", msg=" << strerror(errno);
        close(listen_fd_);
        close(epoll_fd_);
        return ret;
    }
    return 0;
}

void FXServer::Run()
{
    assert( this->epoll_fd_ != 0 );
    assert( this->listen_fd_ != 0 );
    LOG(INFO) << "epoll fd=" << epoll_fd_
        << ", listen_fd=" << listen_fd_;

    const int max_events = 20;
    epoll_event events[max_events];
    sockaddr_in clt_addr;
    const int len = sizeof(clt_addr);
    while(1)
    {
        int nfds = epoll_wait(epoll_fd_, events, max_events, -1);
        LOG(INFO) << "epoll_wait return " << nfds;
        if( nfds < 0 )
        {
            LOG(ERROR) << "epoll_wait failed, ret="
                       << nfds
                       << ", msg="
                       << strerror(errno);
            exit(1);
        }
        for( int i = 0; i < nfds; ++i )
        {
            if( events[i].data.fd == listen_fd_ )
            {
                int client_fd = accept(listen_fd_, (sockaddr *)&clt_addr, (unsigned*)&len );
                this->OnConnect(client_fd);
            }
            else if( events[i].events & EPOLLIN )
            {
                this->OnMessage(events[i].data.fd);
            }
            else if( events[i].events & EPOLLOUT )
            {
                this->OnWritable(events[i].data.fd);
            }
        }
    }
    close(epoll_fd_);
    close(listen_fd_);
    return;
}

void FXServer::OnConnect(int fd)
{
    this->SetNonblocking(fd);
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);

    LOG(INFO) << "new connection, fd=" << fd;
}

void FXServer::OnMessage(int fd)
{
    LOG(INFO) << "OnMessage fd=" << fd;
    int nread = 0;
    const size_t buf_size = 1 << 20;
    char buf[buf_size];
    FXBuffer & internal_buf = this->buf_map_[fd];
    do
    {
        nread = recv(fd, &buf, buf_size-1, 0);
        LOG(INFO) << "recv return " << nread ;
        if( nread < 0 )
        {
            if( errno != EAGAIN )
            {
                LOG(ERROR) << "recv failed, fd=" << fd 
                           << ", ret=" << nread 
                           << ", msg=" << strerror(errno);
                exit(-2);
            }
            else
            {
                LOG(INFO) << "Get EAGAIN";
                break;
            }
        }
        else if( nread == 0 )
        {
            this->OnClose(fd);
            return;
        }
        else
        {
            LOG(INFO) << "read " << nread << " bytes.";
            internal_buf.Append( buf, nread );
        }
    }while(nread > 0);
    LOG(INFO) << "out loop";
    if( internal_buf.BytesToRead() != 0 )
        this->NotifyWritable(fd);
}

void FXServer::OnWritable(int fd)
{
    FXBuffer & internal_buf = this->buf_map_[fd];
    assert( internal_buf.BytesToRead() != 0 );

    LOG(INFO) << "try send " << internal_buf.BytesToRead() << " bytes to client, fd=" << fd;
    int ret = send(fd, internal_buf.Read(), internal_buf.BytesToRead(), 0);
    if( ret < 0 )
    {
        LOG(ERROR) << "send failed, ret=" << ret << ", msg=[" << strerror(errno) << "]";
        exit(-2);
    }
    LOG(INFO) << "send return " << ret;
    internal_buf.Clear();
    this->NotifyReadable(fd);
}

void FXServer::OnClose(int fd)
{
    LOG(INFO) << "client disconnected, fd=" << fd;
    epoll_event ev;
    ev.data.fd = fd;

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
    close(fd);

    BOOST_AUTO(iter, this->buf_map_.find(fd) );
    if( iter != this->buf_map_.end() )
    {
        this->buf_map_.erase(iter);
    }
}

void FXServer::SetNonblocking(int fd)
{
    int opts;
    opts = fcntl(fd, F_GETFL);
    if(opts < 0){
        LOG(ERROR) << "fcntl Get flag error.";
        exit(1);
    }

    opts = opts | O_NONBLOCK;

    if(fcntl(fd, F_SETFL, opts) < 0){
        LOG(ERROR) << "fcntl Set flag error.";
        exit(1);
    }
}

void FXServer::NotifyWritable(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLOUT;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

void FXServer::NotifyReadable(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}
