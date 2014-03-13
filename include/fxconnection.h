/*
 * =====================================================================================
 *       Filename:  fxconnection.h
 *        Created:  14:10:32 Mar 12, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  connection
 *
 * =====================================================================================
 */

#ifndef  __FXCONNECTION_H__
#define  __FXCONNECTION_H__

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "fxbuffer.h"

class FXServer;

class FXConnection : boost::noncopyable
{
    public:
        FXConnection(int fd, FXServer * server);
        ~FXConnection();

        bool connected() const;

        int FileDescriptor() const;
        int Write(const char * ptr);
        int Write(const char * ptr, size_t len);
        void Close();

        void NotifyWriteEvents();
        void IgnoreWriteEvents();

        const FXBuffer & ReadBuffer();
        const FXBuffer & WriteBuffer();
        FXBuffer * MutableReadBuffer();
        FXBuffer * MutableWriteBuffer();

    private:
        int fd_;
        bool connected_;
        FXServer * server_;
        FXBuffer read_buffer_;
        FXBuffer write_buffer_;
};

typedef boost::shared_ptr<FXConnection> FXConnectionPtr;

#endif   /* ----- #ifndef __FXCONNECTION_H__----- */
