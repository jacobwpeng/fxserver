/*
 * =====================================================================================
 *       Filename:  fx_acceptor.h
 *        Created:  14:07:44 Apr 09, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_ACCEPTOR_H__
#define  __FX_ACCEPTOR_H__

#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "fx_channel.h"

namespace fx
{
    class Channel;
    class EventLoop;
    class TcpConnection;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map<int, TcpConnectionPtr> TcpConnectionMap;
    class Acceptor
    {
        public:
            Acceptor(EventLoop * loop);
            ~Acceptor();

            void BindOrAbort(const std::string& addr, int port);       /* TODO : ʹ�÷�װ�ṹ���ݵ�ַ */
            void Listen();

        private:
            void NewConnetion();
            void SetNonblocking(int fd);
            void OnCloseConnection(int fd);

        private:
            EventLoop * loop_;
            int listen_fd_;
            boost::scoped_ptr<Channel> listen_channel_;
            ChannelList channels_;
            TcpConnectionMap connections_;
    };
}

#endif   /* ----- #ifndef __FX_ACCEPTOR_H__----- */