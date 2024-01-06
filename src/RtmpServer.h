#ifndef XQH_RTMP_SERVER_H
#define XQH_RTMP_SERVER_H
#include <string>
#include <mutex>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include "rtmp.h"
#include "RtmpSession.h"
#include "RtmpConnection.h"

namespace xqh
{
typedef std::shared_ptr<RtmpConnection> RtmpConnectionPtr;
class RtmpServer:public Rtmp, public std::enable_shared_from_this<RtmpServer>
{
public:
    using EventCallback = std::function<void(std::string event_type, std::string stream_path)>;
    static std::shared_ptr<RtmpServer> Create(muduo::net::EventLoop* event_loop, const muduo::net::InetAddress& listenAddr);
    ~RtmpServer();
    void SetEventCallbck(EventCallback event_cb);
    //muduo::net::EventLoop* GetEventLoop();
    void start();
private:
    friend class RtmpConnection;
    
    RtmpServer(muduo::net::EventLoop* event_loop, const muduo::net::InetAddress& listenAddr);
    void AddSession(std::string stream_path);
    void RemoveSession(std::string stream_path);

    RtmpSession::Ptr GetSession(std::string stream_path);
    bool HasSession(std::string stream_path);
    bool HasPublisher(std::string stream_path);

    void NotifyEvent(std::string event_type, std::string stream_path);

    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    muduo::net::EventLoop* event_loop_;
    std::mutex mutex_;
    std::unordered_map<std::string,RtmpSession::Ptr> rtmp_sessions_;


    //存储RtmpConnection和TcpConnection的map
    std::unordered_map<muduo::net::TcpConnectionPtr,RtmpConnectionPtr> rtmp_connection_maps;
    std::vector<EventCallback> event_callbacks_;

    muduo::net::TcpServer server_; 
};
}

#endif