#include "src/RtmpServer.h"
#include <muduo/net/EventLoop.h>
#include <thread>

using namespace muduo::net;
/*
	����
	ffmpeg -re -i test.mp4 -vcodec h264 -acodec aac -f flv rtmp://127.0.0.1/live/test

	����
	ffplay -i rtmp://127.0.0.1:1935/live/test

*/

int main(int argc, char **argv)
{
    int port = 1935;
    printf("rtmpServer rtmp://127.0.0.1:%d\n", port);
    muduo::net::EventLoop eventLoop;
    InetAddress listenAddr(port);
    auto rtmp_server = xqh::RtmpServer::Create(&eventLoop, listenAddr);
    rtmp_server->SetChunkSize(60000);
    rtmp_server->SetEventCallbck([](std::string type, std::string stream_path)
    {
        printf("[Event] %s, stream path: %s\n\n", type.c_str(), stream_path.c_str());
    });

    rtmp_server->start();
    eventLoop.loop();

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}