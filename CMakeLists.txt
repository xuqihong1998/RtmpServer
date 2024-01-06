# CMake 最低版本要求
cmake_minimum_required(VERSION 3.12)

# 项目名称
project(RtmpServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(BASE_LIB_PATH /home/parallels/Desktop/Parallels\ Shared\ Folders/MediaServer/3rdparty/build/release-cpp11/lib)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 添加头文件目录
include_directories(3rdparty/muduo)
include_directories(3rdparty/muduo/muduo)

# 添加源文件目录
add_subdirectory(src)

# 生成可执行文件
add_executable(rtmpserver 
main.cpp
src/amf.cpp
src/RtmpChunk.cpp
src/RtmpConnection.cpp
src/RtmpHandshake.cpp
src/RtmpMessage.cpp
src/RtmpServer.cpp
src/RtmpSession.cpp
util/HandleBit.cpp
)

# 链接库
target_link_libraries(rtmpserver
${BASE_LIB_PATH}/libace_logging_proto.a
${BASE_LIB_PATH}/libecho_proto.a
${BASE_LIB_PATH}/libmuduo_base.a
${BASE_LIB_PATH}/libmuduo_http.a
${BASE_LIB_PATH}/libmuduo_inspect.a
${BASE_LIB_PATH}/libmuduo_net.a
${BASE_LIB_PATH}/libmuduo_protobuf_codec.a
${BASE_LIB_PATH}/libmuduo_protorpc_wire.a
${BASE_LIB_PATH}/libmuduo_protorpc.a
${BASE_LIB_PATH}/libmuduo_pubsub.a
${BASE_LIB_PATH}/libprotobuf_codec.a
${BASE_LIB_PATH}/libquery_proto.a
${BASE_LIB_PATH}/libsudoku_proto.a
)
