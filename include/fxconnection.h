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
        int Write(const std::string& msg);
        void Close();

        std::string PeerAddress();
        std::string PeerIp();
        uint16_t PeerPort();

        FXServer * Server();

        const FXBuffer & ReadBuffer();
        const FXBuffer & WriteBuffer();
        FXBuffer * MutableReadBuffer();
        FXBuffer * MutableWriteBuffer();

        void NotifyWriteEvents();
        void IgnoreWriteEvents();

    private:
        int fd_;
        bool connected_;
        std::string peer_addr_;
        FXServer * server_;
        FXBuffer read_buffer_;
        FXBuffer write_buffer_;
};

typedef boost::shared_ptr<FXConnection> FXConnectionPtr;

#endif   /* ----- #ifndef __FXCONNECTION_H__----- */
