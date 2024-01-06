#include "RtmpServer.h"
#include "RtmpConnection.h"
#include <muduo/base/Logging.h>

using namespace xqh;
using namespace muduo;
using namespace muduo::net;

RtmpServer::RtmpServer(muduo::net::EventLoop* event_loop,
                       const InetAddress& listenAddr)
    :server_(event_loop, listenAddr, "RtmpServer")
    ,event_loop_(event_loop)
    ,event_callbacks_(10)
{
    event_loop_->runEvery(30000,[this]{
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto iter = rtmp_sessions_.begin(); iter != rtmp_sessions_.end();)
        {
            if(iter->second->GetClients() == 0)
            {
                rtmp_sessions_.erase(iter++);
            }
            else
            {
                iter++;
            }
        }
        return true;
    }
    );
    server_.setConnectionCallback(
        std::bind(&RtmpServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&RtmpServer::onMessage, this, _1, _2, _3));
}

RtmpServer::~RtmpServer()
{

}

void RtmpServer::start()
{
    server_.start();
}

void RtmpServer::SetEventCallbck(EventCallback event_cb)
{
    std::lock_guard<std::mutex> lock(mutex_);
    event_callbacks_.push_back(event_cb);
}

void RtmpServer::NotifyEvent(std::string event_type, std::string stream_path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto event_cb : event_callbacks_)
    {
        if(event_cb)
        {
            event_cb(event_type, stream_path);
        }
    }
}

void RtmpServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << "RtmpServer -" << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    //to do 
    RtmpConnectionPtr rtmpconn_ptr = std::make_shared<RtmpConnection>(shared_from_this(),conn);
    rtmp_connection_maps[conn] = rtmpconn_ptr;
}

void RtmpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buf,
                            muduo::Timestamp time)
{
    //找到对应的conn
    if(rtmp_connection_maps.find(conn) != rtmp_connection_maps.end())
    {
        rtmp_connection_maps[conn]->OnRead(*buf);
    }
}

std::shared_ptr<RtmpServer> RtmpServer::Create(muduo::net::EventLoop* event_loop, const muduo::net::InetAddress& listenAddr)
{
    std::shared_ptr<RtmpServer> server(new RtmpServer(event_loop, listenAddr));
    return server;
}

void RtmpServer::AddSession(std::string stream_path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(rtmp_sessions_.find(stream_path) == rtmp_sessions_.end())
    {
        rtmp_sessions_[stream_path] = std::make_shared<RtmpSession>();
    }
}

void RtmpServer::RemoveSession(std::string stream_path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rtmp_sessions_.erase(stream_path);
}

bool RtmpServer::HasSession(std::string stream_path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return (rtmp_sessions_.find(stream_path)!=rtmp_sessions_.end());
}

RtmpSession::Ptr RtmpServer::GetSession(std::string stream_path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(rtmp_sessions_.find(stream_path) == rtmp_sessions_.end())
    {
        rtmp_sessions_[stream_path] = std::make_shared<RtmpSession>();
    }
    return rtmp_sessions_[stream_path];
}

bool RtmpServer::HasPublisher(std::string stream_path)
{
    auto session = GetSession(stream_path);
    if(session == nullptr)
    {
        return false;
    }
    return (session->GetPublisher()!=nullptr);
}
/*
muduo::net::EventLoop* GetEventLoop()
{
    return event_loop_;
}
*/