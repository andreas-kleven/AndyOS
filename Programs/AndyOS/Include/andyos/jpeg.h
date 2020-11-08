#pragma once
#include <andyos/image.h>
#include <sys/types.h>

struct DHT_TABLE;
struct JPEG_CHANNEL;
struct JPEG_QTABLE;
struct jpeg_frame;

struct JPEG : public IMAGE
{
    ~JPEG();

    bool Update(void *data, size_t length);

    static JPEG *Parse(void *data, size_t length);

  private:
    jpeg_frame *frame = 0;
};
