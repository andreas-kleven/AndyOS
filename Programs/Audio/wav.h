#pragma once
#include <sys/types.h>

struct WAVE_CHUNK
{
    char id[4];
    uint32_t size;
} __attribute__((packed));

struct WAVE_HEADER
{
    char checksum[4];
    uint32_t chunksize;
    char format[4];
} __attribute__((packed));

struct WAVE_FORMAT_HEADER
{
    char id[4];
    uint32_t size;
    uint16_t audioformat;
    uint16_t numchannels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t blockalign;
    uint16_t bitspersample;
} __attribute__((packed));

struct WAVE_DATA_HEADER
{
    char id[4];
    uint32_t size;
} __attribute__((packed));
