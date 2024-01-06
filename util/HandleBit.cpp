#include "HandleBit.h"
namespace xqh
{
void WriteUint32BE(char* p, uint32_t value)
{
    p[0] = value >> 24;
    p[1] = value >> 16;
    p[2] = value >> 8;
    p[3] = value & 0xff;
}
void WriteUint32LE(char* p, uint32_t value)
{
    p[0] = value & 0xff;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}
void WriteUint24BE(char* p, uint32_t value)
{
    p[0] = value >> 16;
    p[1] = value >> 8;
    p[2] = value & 0xff;
}
void WriteUint24LE(char* p, uint32_t value)
{
    p[0] = value & 0xff;
    p[1] = value >> 8;
    p[2] = value >> 16;
}
void WriteUint16BE(char* p, uint16_t value)
{
    p[0] = value >> 8;
    p[1] = value & 0xff;
}
void WriteUint16LE(char* p, uint16_t value)
{
    p[0] = value & 0xff;
    p[1] = value >> 8;
}
uint32_t ReadUint32BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    return value;
}
uint32_t ReadUint32LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
    return value;
}
uint32_t ReadUint24BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[0] << 16) | (p[1] << 8) | p[2];
    return value;
}
uint32_t ReadUint24LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[2] << 16) | (p[1] << 8) | p[0];
    return value;
}
uint16_t ReadUint16BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint16_t value = (p[0] << 8) | p[1];
    return value;
}
uint16_t ReadUint16LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint16_t value = (p[1] << 8) | p[0];
    return value;
}
}