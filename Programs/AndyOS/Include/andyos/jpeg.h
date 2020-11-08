#pragma once
#include <sys/types.h>

struct DHT_TABLE;
struct JPEG_CHANNEL;
struct JPEG_QTABLE;
struct jpeg_frame;

struct JPEG
{
    uint32_t *pixels = 0;
    int width = 0;
    int height = 0;

    ~JPEG();

    bool Update(void *data, size_t length);

    static JPEG *Parse(void *data, size_t length);

  private:
    jpeg_frame *frame = 0;
};
