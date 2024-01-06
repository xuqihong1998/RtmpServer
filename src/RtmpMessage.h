#include <cstdint>
#include <memory>

namespace xqh {
    /*
    定义rtmp message header
    message header的结构和rtmp basic header的fmt类型有关
    */
    struct RtmpMessageHeader
    {
        uint8_t timestamp[3];
        uint8_t length[3];
        uint8_t type_id;
        uint8_t stream_id[4]; 
    };

    struct RtmpMessage
    {
        //rtmp header

        //timestamp
        uint32_t timestamp = 0;
        uint32_t length = 0;
        uint8_t type_id = 0;
        uint32_t stream_id = 0;

        //额外的timestamp
        uint32_t extend_timestamp = 0;
        //真实的timestamp ==> timestamp + extend_timestamp
        uint64_t _timestamp = 0;
        uint8_t codecId = 0;
        uint8_t csid = 0;
        uint32_t index = 0;
        std::shared_ptr<char> payload = nullptr;

        void Clear()
        {
            index = 0;
            timestamp = 0;
            extend_timestamp = 0;
            if(length > 0)
            {
                payload.reset(new char[length], std::default_delete<char[]>());
            }
        }

        bool IsCompleted() const
        {
            if(index == length && length > 0 && payload != nullptr)
            {
                return true;
            }
            return false;
        }
    };
}