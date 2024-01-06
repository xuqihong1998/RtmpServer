#include "RtmpHandshake.h"
#include "rtmp.h"
#include <muduo/base/Logging.h>
#include <cstdint>
#include <random>

using namespace xqh;

RtmpHandshake::RtmpHandshake(State state)
{
    handshake_state_ = state;
}

RtmpHandshake::~RtmpHandshake()
{

}

int RtmpHandshake::Parse(muduo::net::Buffer& buffer, char* res_buf, uint32_t res_buf_size)
{
    uint8_t *buf = (uint8_t*)buffer.peek();
    uint32_t buf_size = buffer.readableBytes();
    uint32_t pos = 0;
    uint32_t res_size = 0;
    std::random_device rd;

    if(handshake_state_ == HANDSHAKE_S0S1S2)
    {
        if(buf_size < (1 + 1536 + 1536))
        {
            return res_size;
        }

        if(buf[0] != RTMP_VERSION)
        {
            LOG_ERROR << "unsupport rtmp version: " << buf[0] << "\n";
        }

        pos += 1 + 1536 + 1536;
        res_size = 1536;
        memcpy(res_buf,buf+1,1536);
        handshake_state_ = HANDSHAKE_COMPLETE;
    }
    else if(handshake_state_ == HANDSHAKE_C0C1)
    {
        if(buf_size < 1537)
        {
            return res_size;
        }
        else
        {
            if(buf[0] != RTMP_VERSION)
            {
                return -1;
            }

            pos += 1537;
            res_size = 1+1536+1536;
            memset(res_buf,0,1537);
            res_buf[0] = RTMP_VERSION;

            char* p = res_buf;
            p += 9;
            for(int i = 0; i < 1528; i++)
            {
                *p++ = rd();
            }
            memcpy(p, buf+1,1536);
            handshake_state_ = HANDSHAKE_C2;
        }
    }
    else if(handshake_state_ == HANDSHAKE_C2)
    {
        if(buf_size < 1536)
        {
            return res_size;
        }
        else
        {
            pos = 1536;
            handshake_state_ = HANDSHAKE_COMPLETE;
        }
    }
    else
    {
        return -1;
    }
    buffer.retrieve(pos);
    return res_size;
}

int RtmpHandshake::BuildC0C1(char* buf, uint32_t buf_size)
{
    uint32_t size = 1 + 1536;
    memset(buf,0,1537);
    buf[0] = RTMP_VERSION;
    std::random_device rd;
    uint8_t *p = (uint8_t *)buf;
    p+=9;
    for(int i = 0; i < 1528; i++)
    {
        *p++ = rd();
    }
    return size;
}