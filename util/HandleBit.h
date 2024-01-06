#ifndef XQH_HANDLE_BIT_H
#define XQH_HANDLE_BIT_H
#include <cstdint>
#include <memory>
#include <queue>
#include <string>

namespace xqh
{
    //write operator
    //大端序 高位存储在低地址 低位存储在高地址
    void WriteUint32BE(char* p, uint32_t value);
    //小端序 低位存储在低地址 高位存储在高地址
    void WriteUint32LE(char* p, uint32_t value);
    void WriteUint24BE(char* p, uint32_t value);
    void WriteUint24LE(char* p, uint32_t value);
    void WriteUint16BE(char* p, uint16_t value);
    void WriteUint16LE(char* p, uint16_t value);

    //read operator
    uint32_t ReadUint32BE(char* data);
    uint32_t ReadUint32LE(char* data);
    uint32_t ReadUint24BE(char* data);
    uint32_t ReadUint24LE(char* data);
    uint16_t ReadUint16BE(char* data);
    uint16_t ReadUint16LE(char* data);
}
#endif