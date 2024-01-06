#ifndef RTMP_CHUNK_H
#define RTMP_CHUNK_H

#include "../util/HandleBit.h"
#include <muduo/net/Buffer.h>
#include "RtmpMessage.h"
#include "amf.h"
#include <map>

namespace xqh { 
class RtmpChunk
{
public:
    enum State
    {
        PARSE_HEADER,
        PARSE_BODY,
    };
    RtmpChunk();
    virtual ~RtmpChunk();
    int Parse(muduo::net::Buffer& in_buffer, RtmpMessage& out_rtmp_msg);
    int CreateChunk(uint32_t csid, RtmpMessage& rtmp_msg, char* buf, uint32_t buf_size);


    void SetInChunkSize(uint32_t in_chunk_size) 
    { 
        in_chunk_size_ = in_chunk_size; 
    }

    void SetOutChunkSize(uint32_t out_chunk_size)
    {
        out_chunk_size_ = out_chunk_size;
    }

    void Clear()
    {
        rtmp_message_.clear();
    }

    int GetStreamId() const
    {
        return stream_id_;
    }
private:
    int ParseChunkHeader(muduo::net::Buffer& buffer);
    int ParseChunkBody(muduo::net::Buffer& buffer);
    int CreateBasicHeader(uint8_t fmt, uint32_t csid, char* buf);
    int CreateMessageHeader(uint8_t fmt, RtmpMessage& rtmp_msg, char* buf);

    State state_;
    int chunk_stream_id_ = 0;
    int stream_id_ = 0;
    uint32_t in_chunk_size_ = 128;
    uint32_t out_chunk_size_ = 128;
    std::map<int, RtmpMessage> rtmp_message_;

    const int kDefaultStreamId = 1;
    //根据fmt类型得到message header包的长度，进行message header包头的解析
    const int kChunkMessageHeaderLen[4] = {11,7,3,0};
};

}

#endif