#ifndef RTMP_HANDSHAKE_H
#define RTMP_HANDSHAKE_H

#include "../util/HandleBit.h"
#include <muduo/net/Buffer.h>


namespace xqh {
class RtmpHandshake
{
public:
    enum State
    {
        HANDSHAKE_C0C1,
        HANDSHAKE_S0S1S2,
        HANDSHAKE_C2,
        HANDSHAKE_COMPLETE
    };

    RtmpHandshake(State state);
    virtual ~RtmpHandshake();
    int Parse(muduo::net::Buffer& buffer, char* res_buf, uint32_t res_buf_size);
    int BuildC0C1(char* buf, uint32_t buf_size);
    bool IsCompleted() const
    {
        return handshake_state_ == HANDSHAKE_COMPLETE;
    }
private:
    State handshake_state_;
};
}

#endif