#include "RtmpSession.h"
#include "RtmpConnection.h"

using namespace xqh;

RtmpSession::RtmpSession()
{

}

RtmpSession::~RtmpSession()
{

}

void RtmpSession::SendMetaData(AmfObjects& metaData)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto iter = rtmp_sinks_.begin(); iter != rtmp_sinks_.end();)
    {
        auto conn = iter->second.lock();
        if(conn == nullptr)
        {
            rtmp_sinks_.erase(iter++);
        }
        else
        {
            if(conn->IsPlayer())
            {
                conn->SendMetaData(metaData);
            }
            iter++;
        }
    }
}

void RtmpSession::SendMediaData(uint8_t type, uint64_t timestamp, std::shared_ptr<char> data, uint32_t size)
{
    //to do 锁的粒度太大了
    std::lock_guard<std::mutex> lock(mutex_);

    if(this->max_gop_cache_len_ > 0)
    {
        this->SaveGop(type, timestamp, data, size);
    }

    for(auto iter = rtmp_sinks_.begin(); iter != rtmp_sinks_.end();)
    {
        auto conn = iter->second.lock();
        if(conn == nullptr)
        {
            rtmp_sinks_.erase(iter++);
        }
        else
        {
            if(conn->IsPlayer())
            {
                if(!conn->IsPlaying())
                {
                    conn->SendMetaData(meta_data_);
                    conn->SendMediaData(RTMP_AVC_SEQUENCE_HEADER, 0, this->avc_sequence_header_, this->avc_sequence_header_size_);
                    conn->SendMediaData(RTMP_AAC_SEQUENCE_HEADER, 0, this->aac_sequence_header_, this->aac_sequence_header_size_);
                    SendGop(conn);
                }
                conn->SendMediaData(type, timestamp, data, size);
            }
            iter++;
        }
    }
    return;
}

void RtmpSession::SaveGop(uint8_t type, uint64_t timestamp, std::shared_ptr<char> data, uint32_t size)
{
    //to do 
}

void RtmpSession::SendGop(std::shared_ptr<RtmpSink> sink)
{
    //to do
}

void RtmpSession::AddSink(std::shared_ptr<RtmpSink> sink)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rtmp_sinks_[sink->GetId()] = sink;
    if(sink->IsPublisher())
    {
        avc_sequence_header_ = nullptr;
        aac_sequence_header_ = nullptr;
        avc_sequence_header_size_ = 0;
        aac_sequence_header_size_ = 0;
        gop_cache_.clear();
        gop_index_ = 0;
        has_publisher_ = true;
        publisher_ = sink;
    }
    return;
}

void RtmpSession::RemoveSink(std::shared_ptr<RtmpSink> sink)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(sink->IsPublisher())
    {
        avc_sequence_header_ = nullptr;
        aac_sequence_header_ = nullptr;
        avc_sequence_header_size_ = 0;
        aac_sequence_header_size_ = 0;
        gop_cache_.clear();
        gop_index_ = 0;
        has_publisher_ = false;
    }
    rtmp_sinks_.erase(sink->GetId());
}

int RtmpSession::GetClients()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int clients = 0;
    for(auto iter : rtmp_sinks_)
    {
        auto conn = iter.second.lock();
        if(conn != nullptr)
        {
            clients += 1;
        }
    }
    return clients;
}

std::shared_ptr<RtmpConnection> RtmpSession::GetPublisher()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto publisher = publisher_.lock();
    if(publisher)
    {
        return std::dynamic_pointer_cast<RtmpConnection>(publisher);
    }
    return nullptr;
}