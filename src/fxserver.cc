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

#include "fx_predefine.h"

#include "fxutils.h"
#include "fxbuffer.h"
#include "fxconnection.h"
#include "fxtimer.h"
#include "fx_timer_mgr.h"

#include "fxserver.h"

FXServer::FXServer()
{

}

FXServer::~FXServer()
{

}

void FXServer::SetConnectionCallback(const ConnectionCallback & cb)
{
    conn_cb_ = cb;
}
void FXServer::SetMessageCallback(const MessageCallback & cb)
{
    msg_cb_ = cb;
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
    assert( this->msg_cb_ );
    assert( this->conn_cb_ );

    timer_mgr_.Run();

    int time_to_sleep = -1;
    boost::optional<uint64_t> opt_latest_time = timer_mgr_.GetLatestExpireTime();
    if( opt_latest_time )
    {
        time_to_sleep = opt_latest_time.get() - NowInMilliSeconds();
    }

    const int max_events = 20;

    epoll_event events[max_events];
    sockaddr_in clt_addr;
    const int len = sizeof(clt_addr);

    while(1)
    {
        int nfds = epoll_wait(epoll_fd_, events, max_events, time_to_sleep);
        if( nfds < 0 )
        {
            LOG(ERROR) << "epoll_wait failed, ret="
                       << nfds
                       << ", msg="
                       << strerror(errno);
            exit(1);
        }
        else if( nfds == 0 )
        {
            timer_mgr_.TriggerExpiredTimers( NowInMilliSeconds() );
        }
        else
        {
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
            timer_mgr_.TriggerExpiredTimers( NowInMilliSeconds() );
        }

        opt_latest_time = timer_mgr_.GetLatestExpireTime();
        if( opt_latest_time )
        {
            time_to_sleep = opt_latest_time.get() - NowInMilliSeconds();
        }
        else
        {
            time_to_sleep = -1;
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

    FXConnectionPtr conn = boost::make_shared<FXConnection>(fd, this);
    this->conn_map_.insert( std::make_pair(fd, conn) );
    /* connection callback */
    /* this->connection_callback_(conn); */
    conn_cb_(conn);

}

void FXServer::CloseConnection(int fd)
{
    BOOST_AUTO(iter, conn_map_.find(fd) );
    assert( iter != conn_map_.end() );

    if( iter->second->WriteBuffer().BytesToRead() != 0 ) /* still got something to write */
    {
        shutdown(fd, SHUT_RD);
        epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLOUT;

        epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
    }
    else
    {
        this->OnClose(fd);
    }
}

void FXServer::OnMessage(int fd)
{
    BOOST_AUTO(iter, conn_map_.find(fd) );
    assert( iter != conn_map_.end() );
    
    FXConnectionPtr & conn = iter->second;

    int nread = 0;
    const size_t buf_size = 1 << 20;
    char local_buf[buf_size];
    FXBuffer * buf = conn->MutableReadBuffer();
    buf->Clear();
    do
    {
        nread = recv(fd, local_buf, buf_size-1, 0);
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
            buf->Append( local_buf, nread );
        }
    }while(nread > 0);
    /* message callback */
    /* this->message_cb_(conn); */
    msg_cb_(conn);
}

void FXServer::OnWritable(int fd)
{
    BOOST_AUTO(iter, conn_map_.find(fd) );
    FXConnectionPtr & conn = iter->second;

    int ret = send(fd, conn->WriteBuffer().Read(), conn->WriteBuffer().BytesToRead(), 0);
    if( ret < 0 )
    {
        LOG(ERROR) << "send failed, ret=" << ret << ", msg=[" << strerror(errno) << "]";
        exit(-2);
    }
    if( conn->connected() == false ) { this->OnClose(conn->FileDescriptor() ); }
    else conn->IgnoreWriteEvents();
}

void FXServer::OnClose(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
    close(fd);

    BOOST_AUTO(iter, conn_map_.find(fd) );
    assert( iter != conn_map_.end() );

    conn_map_.erase(iter);
}

void FXServer::UpdateEvents(int fd, uint32_t events)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
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

void FXServer::NotifyWriteEvents(int fd)
{
    this->UpdateEvents(fd, EPOLLIN | EPOLLOUT);
}

void FXServer::IgnoreWriteEvents(int fd)
{
    this->UpdateEvents(fd, EPOLLIN);
}
