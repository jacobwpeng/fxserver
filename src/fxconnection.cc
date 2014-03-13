/*
 * =====================================================================================
 *       Filename:  fxconnection.cc
 *        Created:  14:44:18 Mar 12, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_predefine.h"

#include "fxbuffer.h"
#include "fxconnection.h"
#include "fxtimer.h"
#include "fx_timer_mgr.h"

#include "fxserver.h"

FXConnection::FXConnection(int fd, FXServer * server)
    :fd_(fd), server_(server)
{

}

FXConnection::~FXConnection()
{

}

bool FXConnection::connected() const
{
    return connected_;
}

int FXConnection::FileDescriptor() const { return this->fd_; }

int FXConnection::Write(const char * ptr)
{
    return this->Write( ptr, strlen(ptr) );
}

int FXConnection::Write(const char * ptr, size_t len)
{
    this->write_buffer_.Append( ptr, len );
    this->NotifyWriteEvents();
    return 0;
}

void FXConnection::Close()
{
    this->connected_ = false;
    this->server_->CloseConnection(fd_);
}

std::string FXConnection::PeerAddress()
{
    if( peer_addr_.empty() )
    {
        sockaddr_in sa;
        socklen_t len = sizeof(sa);
        if( 0 == getpeername(fd_, (struct sockaddr *)&sa, &len) )
        {
            this->peer_addr_ = (boost::format("%s:%d") % inet_ntoa(sa.sin_addr) % ntohs(sa.sin_port)).str();
        }
    }

    return peer_addr_;
}

void FXConnection::NotifyWriteEvents()
{
    this->server_->NotifyWriteEvents( this->fd_ );
}

void FXConnection::IgnoreWriteEvents()
{
    this->server_->IgnoreWriteEvents( this->fd_ );
}

const FXBuffer & FXConnection::ReadBuffer() { return this->read_buffer_; }

const FXBuffer & FXConnection::WriteBuffer() { return this->write_buffer_; }

FXBuffer * FXConnection::MutableReadBuffer() { return &read_buffer_; }

FXBuffer * FXConnection::MutableWriteBuffer() { return &write_buffer_; }
