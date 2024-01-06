#ifndef XQH_RTMP_CONNECTION_H
#define XQH_RTMP_CONNECTION_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/poller/EPollPoller.h>
#include <muduo/net/Channel.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "amf.h"
#include "rtmp.h"
#include "RtmpSink.h"
#include "RtmpChunk.h"
#include "RtmpHandshake.h"
#include "RtmpServer.h"
#include <vector>

namespace xqh
{
class RtmpServer;
class RtmpSession;
class RtmpConnection : public RtmpSink, public std::enable_shared_from_this<RtmpConnection>
{
public:
    using PlayCallback = std::function<void(uint8_t* payload, uint32_t length, uint8_t codecId, uint32_t timestamp)>;

    enum ConnectionState
    {
        HANDSHAKE,
        START_CONNECT,
        START_CREATE_STREAM,
        START_DELETE_STREAM,
        START_PLAY,
        START_PUBLISH,
    };

    enum ConnectionMode
    {
        RTMP_SERVER,
        RTMP_PUBLISHER,
        RTMP_CLIENT
    };

    //创建server连接
    RtmpConnection(std::shared_ptr<RtmpServer> server, muduo::net::TcpConnectionPtr conn);
    virtual ~RtmpConnection();

    std::string GetStreamPath() const
    { return stream_path_; }

    std::string GetStreamName() const
    { return app_;}

    AmfObjects GetMetaData() const
    { return meta_data_;}

    virtual bool IsPlayer() override
    { return connection_state_ == START_PLAY;}

    virtual bool IsPublisher() override
    { return connection_state_ == START_PUBLISH;}

    virtual bool IsPlaying() override
    { return is_playing_;}

    virtual bool IsPublishing() override
    { return is_publishing_;}

    
    virtual uint32_t GetId() override
    {
        return sockfd;
    }
    

   std::string GetStatus()
   {
       if(status_ == "")
       {
        return "unknown error";
       }
       return status_;
   }

private:
    friend class RtmpSession;
    friend class RtmpServer;

    //RtmpConnection(muduo::net::EventLoop* loop, int sockfd, Rtmp* rtmp);
    RtmpConnection(muduo::net::TcpConnectionPtr conn, Rtmp* rtmp);

    bool OnRead(muduo::net::Buffer& buffer);
    void OnClose();

    bool HandleChunk(muduo::net::Buffer& buffer);
    bool HandleMessage(RtmpMessage& rtmp_msg);
    bool HandleInvoke(RtmpMessage& rtmp_msg);
    bool HandleNotify(RtmpMessage& rtmp_msg);
    bool HandleVideo(RtmpMessage& rtmp_msg);
    bool HandleAudio(RtmpMessage& rtmp_msg);

    bool Handshake();
	bool Connect();
	bool CretaeStream();
	bool Publish();
	bool Play();
	bool DeleteStream();

    bool HandleConnect();
    bool HandleCreateStream();
    bool HandlePublish();
    bool HandlePlay();
    bool HandlePlay2();
    bool HandleDeleteStream();
	bool HandleResult(RtmpMessage& rtmp_msg);
	bool HandleOnStatus(RtmpMessage& rtmp_msg);

    void SetPeerBandwidth();
    void SendAcknowledgement();
    void SetChunkSize();
	void SetPlayCB(const PlayCallback& cb);

    bool SendInvokeMessage(uint32_t csid, std::shared_ptr<char> payload, uint32_t payload_size);
    bool SendNotifyMessage(uint32_t csid, std::shared_ptr<char> payload, uint32_t payload_size);   
	bool IsKeyFrame(std::shared_ptr<char> payload, uint32_t payload_size);
    void SendRtmpChunks(uint32_t csid, RtmpMessage& rtmp_msg);

    virtual bool SendMetaData(AmfObjects metaData);
	virtual bool SendMediaData(uint8_t type, uint64_t timestamp, std::shared_ptr<char> payload, uint32_t payload_size);
	virtual bool SendVideoData(uint64_t timestamp, std::shared_ptr<char> payload, uint32_t payload_size);
	virtual bool SendAudioData(uint64_t timestamp, std::shared_ptr<char> payload, uint32_t payload_size);

    std::weak_ptr<RtmpServer> rtmp_server_;
    std::weak_ptr<RtmpSession> rtmp_session_;

	std::shared_ptr<RtmpHandshake> handshake_;
	std::shared_ptr<RtmpChunk> rtmp_chunk_;
	ConnectionMode connection_mode_;
	ConnectionState connection_state_;

    std::shared_ptr<muduo::net::Poller> poll_;
	std::shared_ptr<muduo::net::Channel> channel_;

    muduo::net::TcpConnectionPtr conn_;
    int sockfd;

	uint32_t peer_bandwidth_ = 5000000;
	uint32_t acknowledgement_size_ = 5000000;
	uint32_t max_chunk_size_ = 128;
	uint32_t max_gop_cache_len_ = 0;
	uint32_t stream_id_ = 0;
	uint32_t number_ = 0;
	std::string app_;
	std::string stream_name_;
	std::string stream_path_;
	std::string status_;

    AmfObjects meta_data_;
	AmfDecoder amf_decoder_;
	AmfEncoder amf_encoder_;

	bool is_playing_ = false;
	bool is_publishing_ = false;
	bool has_key_frame_ = false;
	std::shared_ptr<char> avc_sequence_header_;
	std::shared_ptr<char> aac_sequence_header_;
	uint32_t avc_sequence_header_size_ = 0;
	uint32_t aac_sequence_header_size_ = 0;
	PlayCallback play_cb_;
};
}



#endif