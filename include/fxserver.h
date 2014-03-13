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

typedef boost::function<void(const FXConnectionPtr&) > MessageCallback;
typedef boost::function<void(const FXConnectionPtr&) > ConnectionCallback;

class FXServer : boost::noncopyable
{
    public:
        FXServer();
        ~FXServer();
        void SetConnectionCallback(const ConnectionCallback & cb);
        void SetMessageCallback(const MessageCallback & cb);
        int Init(int port);
        void Run();

        void CloseConnection(int fd);

        void NotifyWriteEvents(int fd);
        void IgnoreWriteEvents(int fd);

        FXTimerMgr & TimerManager();

    private:
        void OnConnect(int fd);
        void OnMessage(int fd);
        void OnWritable(int fd);
        void OnClose(int fd);

        void SetNonblocking(int fd);
        void UpdateEvents( int fd, uint32_t events );

    private:
        static const int max_fd_count = 100;
        FXTimerMgr timer_mgr_;
        std::map<int, FXConnectionPtr > conn_map_;
        int listen_fd_;
        int epoll_fd_;

        ConnectionCallback conn_cb_;
        MessageCallback msg_cb_;
};

#endif   /* ----- #ifndef __FXSERVER_H__----- */
