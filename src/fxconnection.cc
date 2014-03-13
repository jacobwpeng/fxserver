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

#include "fxserver.h"
#include "fxconnection.h"

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
