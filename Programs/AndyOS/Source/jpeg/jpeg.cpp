#include "dht.h"
#include <AndyOS.h>
#include <andyos/jpeg.h>
#include <andyos/math.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DHT_Y  0
#define DHT_C  2
#define DHT_AC 1
#define DHT_DC 0

#define DHT_DC_Y (DHT_DC | DHT_Y)
#define DHT_DC_C (DHT_DC | DHT_C)

#define DHT_AC_Y (DHT_AC | DHT_Y)
#define DHT_AC_C (DHT_AC | DHT_C)

#define MARK_SOF0 0xFFC0 // Start Of Frame (Baseline DCT)
#define MARK_SOF2 0xFFC2 // Start Of Frame (Progressive DCT)
#define MARK_DHT  0xFFC4 // Define Huffman Tables
#define MARK_SOI  0xFFD8 // Start of image
#define MARK_EOI  0xFFD9 // End of image
#define MARK_SOS  0xFFDA // Start of scan
#define MARK_DQT  0xFFDB // Define Quantization Tables
#define MARK_DRT  0xFFDD // Define Restart Interval
#define MARK_RST0 0xFFD0 // Restart 0
#define MARK_RST7 0xFFD7 // Restart 7
#define MARK_COM  0xFFFE // Comment

#define INVALID_DECIMAL 0x10000000

struct JPEG_BLOCK
{
    int16_t dct[64];
    float result[64];
};

struct JPEG_QTABLE
{
    int id;
    int precision;
    int values[64];
};

struct JPEG_CHANNEL
{
    int id;
    int sx;
    int sy;
    int qtable;
    size_t block_index;
    size_t num_blocks;
    JPEG_BLOCK *blocks;
};

struct jpeg_frame
{
    int width = 0;
    int height = 0;
    int sample_x = 0;
    int sample_y = 0;
    int bx = 0;
    int by = 0;
    int restart_interval = 0;
    int precision = 0;
    int num_channels = 0;
    int total_blocks = 0;
    float dc[4] = {0};
    JPEG_CHANNEL *channels[4] = {0};
    JPEG_QTABLE *qtables[4] = {0};
    DHT_TABLE *dht[8] = {0};
};

struct jpeg_buffer
{
    uint8_t *ptr;
    size_t size;
    size_t position;
};

// clang-format off

static const int zigzag_map[64] = {
    0,  1,  8,  16, 9,  2,  3,  10,
    17, 24, 32, 25, 18, 11, 4,  5, 
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6,  7,  14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

static const float dct_matrix[8][8] = {
    { 0.35355339,  0.35355339,  0.35355339,  0.35355339,  0.35355339,  0.35355339,  0.35355339,  0.35355339 },
    { 0.49039264,  0.41573481,  0.27778512,  0.09754516, -0.09754516, -0.27778512, -0.41573481, -0.49039264 },
    { 0.46193977,  0.19134172, -0.19134172, -0.46193977, -0.46193977, -0.19134172,  0.19134172,  0.46193977 },
    { 0.41573481, -0.09754516, -0.49039264, -0.27778512,  0.27778512,  0.49039264,  0.09754516, -0.41573481 },
    { 0.35355339, -0.35355339, -0.35355339,  0.35355339,  0.35355339, -0.35355339, -0.35355339,  0.35355339 },
    { 0.27778512, -0.49039264,  0.09754516,  0.41573481, -0.41573481, -0.09754516,  0.49039264, -0.27778512 },
    { 0.19134172, -0.46193977,  0.46193977, -0.19134172, -0.19134172,  0.46193977, -0.46193977,  0.19134172 },
    { 0.09754516, -0.27778512,  0.41573481, -0.49039264,  0.49039264, -0.41573481,  0.27778512, -0.09754516 }
};

static const float dct_transpose[8][8] = {
    { 0.35355339,  0.49039264,  0.46193977,  0.41573481,  0.35355339,  0.27778512,  0.19134172,  0.09754516 },
    { 0.35355339,  0.41573481,  0.19134172, -0.09754516, -0.35355339, -0.49039264, -0.46193977, -0.27778512 },
    { 0.35355339,  0.27778512, -0.19134172, -0.49039264, -0.35355339,  0.09754516,  0.46193977,  0.41573481 },
    { 0.35355339,  0.09754516, -0.46193977, -0.27778512,  0.35355339,  0.41573481, -0.19134172, -0.49039264 },
    { 0.35355339, -0.09754516, -0.46193977,  0.27778512,  0.35355339, -0.41573481, -0.19134172,  0.49039264 },
    { 0.35355339, -0.27778512, -0.19134172,  0.49039264, -0.35355339, -0.09754516,  0.46193977, -0.41573481 },
    { 0.35355339, -0.41573481,  0.19134172,  0.09754516, -0.35355339,  0.49039264, -0.46193977,  0.27778512 },
    { 0.35355339, -0.49039264,  0.46193977, -0.41573481,  0.35355339, -0.27778512,  0.19134172, -0.09754516 }
};

// clang-format on

static inline uint8_t Read8(jpeg_buffer *buffer)
{
    if (buffer->position >= buffer->size)
        return 0;

    uint8_t value = buffer->ptr[buffer->position];
    buffer->position += 1;
    return value;
}

static inline uint16_t Read16(jpeg_buffer *buffer)
{
    if (buffer->position >= buffer->size - 1)
        return 0;

    uint16_t value = ntohs(*(uint16_t *)&buffer->ptr[buffer->position]);
    buffer->position += 2;
    return value;
}

static inline int NextBit(jpeg_buffer *buffer)
{
    int index = buffer->position / 8;
    int offset = buffer->position % 8;

    if (offset == 0) {
        if (index >= buffer->size)
            return 0;
    }

    uint8_t byte1 = buffer->ptr[index];

    if (byte1 == 0xFF) {
        if (offset == 0) {
            uint8_t byte2 = buffer->ptr[index + 1];

            if (byte2) {
                if (byte2 >= (MARK_RST0 & 0xFF) && byte2 <= (MARK_RST7 & 0xFF)) {
                    fprintf(stderr, "Reset %x\n", byte2);
                    buffer->position += 16;
                    return NextBit(buffer);
                } else {
                    fprintf(stderr, "-New marker %d: 0x%x\n", index, byte2);
                    return -1;
                }
            }
        } else if (offset == 7) {
            buffer->position += 8;
        }
    }

    buffer->position += 1;

    uint8_t value = byte1 >> (7 - offset);
    return value & 1;
}

static int ParseHuffman(DHT_TABLE *table, jpeg_buffer *buffer)
{
    int bit;

    while ((bit = NextBit(buffer)) != -1) {
        if (table->Next(bit)) {
            return table->GetValue();
        }
    }

    return INVALID_DECIMAL;
}

static int ParseSignedDecimal(int size, jpeg_buffer *buffer)
{
    int value = 0;
    int sign = 0;

    if (size > 0) {
        for (int i = 0; i < size; i++) {
            int bit;

            if ((bit = NextBit(buffer)) == -1)
                return -INVALID_DECIMAL;

            if (i == 0)
                sign = !bit;

            if (bit)
                value += 1 << (size - i - 1);
        }
    }

    if (sign)
        value -= (1 << (size)) - 1;

    return value;
}

static inline int GetPos420(int index, int width)
{
    int pos = index / 2;
    pos += (pos / width) * width;

    if ((index / 2) % 2 == 1)
        pos += width - 1;

    if (index % 2 == 1)
        pos += 1;

    return pos;
}

static inline int GetPos440(int index, int width)
{
    int pos = index / 2;
    pos += (pos / width) * width;

    if (index % 2 == 1)
        pos += width;

    return pos;
}

static bool ParseSOF0(jpeg_buffer *buffer, jpeg_frame *frame)
{
    frame->precision = Read8(buffer);
    frame->height = Read16(buffer);
    frame->width = Read16(buffer);
    frame->num_channels = Read8(buffer);

    int first_sx = 1;
    int first_sy = 1;

    for (int i = 0; i < frame->num_channels; i++) {
        uint8_t id = Read8(buffer);
        uint8_t sampling = Read8(buffer);
        uint8_t qtable = Read8(buffer);
        int sx = sampling >> 4;
        int sy = sampling & 0xF;
        int index = id - 1;

        if (index < 0 || index >= 4) {
            fprintf(stderr, "Invalid component id %d\n", id);
            return false;
        }

        if (id == 1) {
            frame->sample_x = sx;
            frame->sample_y = sy;
            frame->by = (frame->height + 7) / 8;
            frame->bx = (frame->width + 7) / 8;
            frame->bx += (frame->sample_x - frame->bx % frame->sample_x) % frame->sample_x;
            frame->by += (frame->sample_y - frame->by % frame->sample_y) % frame->sample_y;

            first_sx = sx;
            first_sy = sy;
            sx = 1;
            sy = 1;
        } else {
            sx = first_sx / sx;
            sy = first_sy / sy;
        }

        if (id == 1) {
            if (sx != 1 || sy != 1) {
                fprintf(stderr, "Invalid subsampling id:%d sx:%d sy:%d\n", id, sx, sy);
                return false;
            }
        } else {
            if (sx > 2 || sy > 2) {
                fprintf(stderr, "Invalid subsampling id:%d sx:%d sy:%d\n", id, sx, sy);
                return false;
            }
        }

        JPEG_CHANNEL *channel = new JPEG_CHANNEL();
        frame->channels[index] = channel;
        channel->id = id;
        channel->sx = sx;
        channel->sy = sy;
        channel->qtable = qtable;
        channel->num_blocks = 0;
        channel->block_index = 0;
        channel->blocks = 0;
    }

    return true;
}

static bool ParseDQT(jpeg_buffer *buffer, jpeg_frame *frame, uint16_t mark_length)
{
    while (buffer->position < mark_length) {
        uint8_t byte = Read8(buffer);
        uint8_t id = byte & 0xF;
        uint8_t precision = byte >> 4;

        if (id >= sizeof(frame->qtables) / sizeof(frame->qtables[0]))
            return false;

        JPEG_QTABLE *qtable = new JPEG_QTABLE();
        frame->qtables[id] = qtable;
        qtable->id = id;
        qtable->precision = precision ? 16 : 8;

        if (qtable->precision == 8) {
            for (int i = 0; i < 64; i++)
                qtable->values[i] = Read8(buffer);
        } else {
            for (int i = 0; i < 64; i++)
                qtable->values[i] = Read16(buffer);
        }
    }

    return true;
}

static bool ParseSOS(jpeg_buffer *buffer, jpeg_frame *frame, uint16_t mark_length)
{
    jpeg_buffer bit_buffer;
    bit_buffer.ptr = buffer->ptr + mark_length;
    bit_buffer.position = 0;
    bit_buffer.size = buffer->size - mark_length;

    // Allocate blocks
    for (int i = 0; i < frame->num_channels; i++) {
        JPEG_CHANNEL *channel = frame->channels[i];
        channel->num_blocks = frame->bx * frame->by / channel->sy / channel->sx;
        channel->blocks = new JPEG_BLOCK[channel->num_blocks];
        memset(channel->blocks, 0, sizeof(JPEG_BLOCK) * channel->num_blocks);
        frame->total_blocks += channel->num_blocks;
    }

    // Channels
    int channel_seq[6] = {0};
    int channel_seqlen;

    if (frame->num_channels == 1) {
        channel_seqlen = 1;
    } else if (frame->num_channels == 3) {
        if (frame->sample_x == 1 && frame->sample_y == 1) {
            channel_seq[1] = 1;
            channel_seq[2] = 2;
            channel_seqlen = 3;
        } else if ((frame->sample_x == 2 && frame->sample_y == 1) ||
                   (frame->sample_x == 1 && frame->sample_y == 2)) {
            channel_seq[2] = 1;
            channel_seq[3] = 2;
            channel_seqlen = 4;
        } else if (frame->sample_x == 2 && frame->sample_y == 2) {
            channel_seq[4] = 1;
            channel_seq[5] = 2;
            channel_seqlen = 6;
        } else {
            fprintf(stderr, "Invalid subsampling");
            return false;
        }
    } else {
        fprintf(stderr, "Number of channels not supported");
        return false;
    }

    // Start reading
    for (int bi = 0; bi < frame->total_blocks; bi++) {
        int ci = channel_seq[bi % channel_seqlen];
        int type = ci == 0 ? DHT_Y : DHT_C;
        JPEG_CHANNEL *channel = frame->channels[ci];

        int block_index = channel->block_index;
        int pos = block_index;

        if (ci == 0) {
            if (frame->sample_x == 2 && frame->sample_y == 2)
                pos = GetPos420(block_index, frame->bx);

            if (frame->sample_x == 1 && frame->sample_y == 2)
                pos = GetPos440(block_index, frame->bx);
        }

        JPEG_BLOCK *block = &channel->blocks[pos];
        JPEG_QTABLE *qtable = frame->qtables[channel->qtable];

        if (channel->block_index++ >= channel->num_blocks || pos >= channel->num_blocks) {
            fprintf(stderr, "Block error %lu %lu %d\n", channel->num_blocks,
                    channel->block_index - 1, pos);
            return false;
        }

        // Restart interval
        if (bi > 0 && frame->restart_interval &&
            (bi % (frame->restart_interval * channel_seqlen)) == 0) {
            memset(frame->dc, 0, sizeof(frame->dc));

            int index = bit_buffer.position / 8;

            if (bit_buffer.ptr[index] == 0xFF && bit_buffer.ptr[index + 1] == 0)
                bit_buffer.position += 8;

            bit_buffer.position += (8 - (bit_buffer.position % 8)) % 8;
        }

        // DC
        int dc_size = ParseHuffman(frame->dht[type | DHT_DC], &bit_buffer);
        int dc_value = ParseSignedDecimal(dc_size, &bit_buffer);
        frame->dc[ci] += dc_value;
        block->dct[0] = frame->dc[ci] * qtable->values[0];

        // AC
        int ac_index = 1;
        while (ac_index < 64) {
            int value = ParseHuffman(frame->dht[type | DHT_AC], &bit_buffer);

            if (value == 0) { // EOB
                break;
            } else if (value == 0xF0) { // ZRL
                ac_index += 16;
            } else {
                int zeroes = value >> 4;
                int dc_size = value & 0xF;
                ac_index += zeroes;

                int ac_value = ParseSignedDecimal(dc_size, &bit_buffer);
                int index = zigzag_map[ac_index];

                if (ac_index >= 64 || ac_index < 0) {
                    fprintf(stderr, "AC index %d %d %d\n", ac_index, index, zeroes);
                    return false;
                }

                block->dct[index] = ac_value * qtable->values[ac_index];
                ac_index += 1;
            }
        }

        // DQT
        float tmp[64];

        for (int i = 0; i < 64; i++) {
            tmp[i] = 0;

            for (int j = 0; j < 8; j++) {
                int x = i % 8;
                int y = i / 8;
                float a = block->dct[y * 8 + j];
                float b = dct_matrix[j][x];
                tmp[i] += a * b;
            }
        }

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 8; j++) {
                int x = i % 8;
                int y = i / 8;
                float a = dct_transpose[y][j];
                float b = tmp[j * 8 + x];
                block->result[i] += a * b;
            }

            block->result[i] += 128;
        }
    }

    return true;
}

static void Render1(jpeg_frame *frame, uint32_t *pixels)
{
    for (int y = 0; y < frame->by; y++) {
        for (int x = 0; x < frame->bx; x++) {
            int idx = y * frame->bx + x;
            JPEG_BLOCK *block = &frame->channels[0]->blocks[idx];

            for (int u = 0; u < 8; u++) {
                int absy = 8 * y + u;

                if (absy >= frame->height)
                    break;

                for (int v = 0; v < 8; v++) {
                    int absx = 8 * x + v;

                    if (absx >= frame->width)
                        break;

                    int bw = block->result[u * 8 + v];
                    bw = min(max(bw, 0), 255);
                    uint32_t col = ((uint8_t)bw << 16) | ((uint8_t)bw << 8) | (uint8_t)bw;
                    pixels[absy * frame->width + absx] = col;
                }
            }
        }
    }
}

static void Render3(jpeg_frame *frame, uint32_t *pixels)
{
    int x2 = 0;
    int y2 = 0;
    int bx2 = frame->bx / frame->sample_x;
    int by2 = frame->by / frame->sample_y;

    for (int y = 0; y < frame->by; y++) {
        for (int x = 0; x < frame->bx; x++) {
            JPEG_BLOCK *block0 = &frame->channels[0]->blocks[y * frame->bx + x];
            JPEG_BLOCK *block1 = &frame->channels[1]->blocks[y2 * bx2 + x2];
            JPEG_BLOCK *block2 = &frame->channels[2]->blocks[y2 * bx2 + x2];

            for (int u = 0; u < 8; u++) {
                int absy = 8 * y + u;

                if (absy >= frame->height)
                    break;

                for (int v = 0; v < 8; v++) {
                    int absx = 8 * x + v;

                    if (absx >= frame->width)
                        break;

                    int u2 = (absy / frame->sample_y) % 8;
                    int v2 = (absx / frame->sample_x) % 8;

                    float Y = block0->result[8 * u + v];
                    float B = block1->result[8 * u2 + v2];
                    float R = block2->result[8 * u2 + v2];

                    int r = round(Y + 1.402 * (1.0 * R - 128.0));
                    int g = round(Y - 0.344136 * (1.0 * B - 128.0) - 0.714136 * (1.0 * R - 128.0));
                    int b = round(Y + 1.772 * (1.0 * B - 128.0));

                    r = min(max(r, 0), 255);
                    g = min(max(g, 0), 255);
                    b = min(max(b, 0), 255);

                    uint32_t col = ((uint8_t)r << 16) | ((uint8_t)g << 8) | (uint8_t)b;
                    pixels[absy * frame->width + absx] = col;
                }
            }

            if (x % frame->sample_x == frame->sample_x - 1)
                x2 = (x2 + 1) % bx2;
        }

        if (y % frame->sample_y == frame->sample_y - 1)
            y2 += 1;
    }
}

static void Render(jpeg_frame *frame, uint32_t *pixels)
{
    if (frame->num_channels == 1) {
        Render1(frame, pixels);
    } else if (frame->num_channels == 3) {
        Render3(frame, pixels);
    }
}

static void Cleanup(jpeg_frame *frame)
{
    if (!frame)
        return;

    for (int i = 0; i < sizeof(frame->channels) / sizeof(frame->channels[0]); i++) {
        JPEG_CHANNEL *channel = frame->channels[i];

        if (!channel)
            continue;

        if (channel->blocks)
            delete[] channel->blocks;

        delete channel;
        frame->channels[i] = 0;
    }

    for (int i = 0; i < sizeof(frame->qtables) / sizeof(frame->qtables[0]); i++) {
        JPEG_QTABLE *qtable = frame->qtables[i];

        if (!qtable)
            continue;

        delete qtable;
        frame->qtables[i] = 0;
    }

    for (int i = 0; i < sizeof(frame->dht) / sizeof(frame->dht[0]); i++) {
        DHT_TABLE *dht = frame->dht[i];

        if (!dht)
            continue;

        delete dht;
        frame->dht[i] = 0;
    }
}

bool DoParse(void *data, size_t data_length, jpeg_frame *frame)
{
    uint8_t *data_end = (uint8_t *)data + data_length;

    for (int data_index = 0; data_index < data_length - 1; data_index++) {
        uint8_t *ptr = (uint8_t *)data + data_index;
        uint16_t marker = ntohs(*(uint16_t *)ptr);

        if (marker <= 0xFF00)
            continue;

        ptr += 2;

        jpeg_buffer buffer;
        buffer.ptr = ptr;
        buffer.position = 0;
        buffer.size = data_end - ptr;

        uint16_t mark_length = Read16(&buffer);

        switch (marker) {
        case MARK_DRT:
            frame->restart_interval = Read16(&buffer);
            break;

        case MARK_SOF0:
            if (!ParseSOF0(&buffer, frame)) {
                fprintf(stderr, "Parse SOF0 failed\n");
                return false;
            }
            break;

        case MARK_DHT:
            if (!DHT_TABLE::Parse(ptr + 2, mark_length - 2, frame->dht)) {
                fprintf(stderr, "DHT parse failed\n");
                return false;
            }
            break;

        case MARK_DQT:
            if (!ParseDQT(&buffer, frame, mark_length)) {
                fprintf(stderr, "Parse DQT failed\n");
                return false;
            }
            break;

        case MARK_SOS:
            if (!ParseSOS(&buffer, frame, mark_length)) {
                fprintf(stderr, "Parse SOS failed\n");
                return false;
            }

            return true;
        }
    }

    return false;
}

JPEG::~JPEG()
{
    if (pixels)
        delete[] pixels;
}

JPEG *JPEG::Parse(void *data, size_t length)
{
    jpeg_frame frame;

    if (DoParse(data, length, &frame)) {
        // TODO: validate
        JPEG *jpeg = new JPEG();
        jpeg->width = frame.width;
        jpeg->height = frame.height;
        jpeg->pixels = new uint32_t[frame.width * frame.height];
        Render(&frame, jpeg->pixels);
        return jpeg;
    }

    Cleanup(&frame);
    return 0;
}
