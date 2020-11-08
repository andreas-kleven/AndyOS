#include "dht.h"
#include <AndyOS.h>
#include <andyos/jpeg.h>
#include <andyos/math.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <tmmintrin.h>
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

#define INVALID_DECIMAL    0x10000000
#define NUMBER_SCALE_SHIFT 10
#define NUMBER_SCALE       (1 << NUMBER_SCALE_SHIFT)
#define NUMBER_SCALE2      (NUMBER_SCALE * NUMBER_SCALE)

struct JPEG_BLOCK
{
    int16_t dct[64];
    int16_t result[64];
} __attribute__((aligned(16)));

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
    size_t num_blocks = 0;
    JPEG_BLOCK *blocks = 0;
    void *blocks_ptr = 0;
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
    uint8_t *end;
};

struct jpeg_bitbuffer
{
    uint8_t *ptr;
    uint8_t *end;
    int offset;
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

static const int16_t dct_matrix[8][8]
__attribute__((aligned(16))) = {
    { 362,  362,  362,  362,  362,  362,  362,  362 },
    { 502,  425,  284,   99,  -99, -284, -425, -502 },
    { 473,  195, -195, -473, -473, -195,  195,  473 },
    { 425,  -99, -502, -284,  284,  502,   99, -425 },
    { 362, -362, -362,  362,  362, -362, -362,  362 },
    { 284, -502,   99,  425, -425,  -99,  502, -284 },
    { 195, -473,  473, -195, -195,  473, -473,  195 },
    {  99, -284,  425, -502,  502, -425,  284,  -99 },
};

static const int16_t dct_transpose[8][8]
__attribute__((aligned(16))) = {
    { 362,  502,  473,  425,  362,  284,  195,   99 },
    { 362,  425,  195,  -99, -362, -502, -473, -284 },
    { 362,  284, -195, -502, -362,   99,  473,  425 },
    { 362,   99, -473, -284,  362,  425, -195, -502 },
    { 362,  -99, -473,  284,  362, -425, -195,  502 },
    { 362, -284, -195,  502, -362,  -99,  473, -425 },
    { 362, -425,  195,   99, -362,  502, -473,  284 },
    { 362, -502,  473, -425,  362, -284,  195,  -99 },
};

// clang-format on

static inline uint8_t Read8(jpeg_buffer *buffer)
{
    if (buffer->ptr >= buffer->end)
        return 0;

    return *buffer->ptr++;
}

static inline uint16_t Read16(jpeg_buffer *buffer)
{
    if (buffer->ptr >= buffer->end - 1)
        return 0;

    uint16_t value = ntohs(*(uint16_t *)buffer->ptr);
    buffer->ptr += 2;
    return value;
}

static inline int NextBit(jpeg_bitbuffer *buffer)
{
    if (buffer->offset == 0) {
        if (buffer->ptr >= buffer->end - 2) {
            fprintf(stderr, "End reached\n");
            return -1;
        }
    }

    uint8_t byte1 = buffer->ptr[0];

    if (byte1 == 0xFF) {
        if (buffer->offset == 0) {
            uint8_t byte2 = buffer->ptr[1];

            if (byte2) {
                if (byte2 >= (MARK_RST0 & 0xFF) && byte2 <= (MARK_RST7 & 0xFF)) {
                    fprintf(stderr, "Reset %x\n", byte2);
                    buffer->ptr += 2;
                    return NextBit(buffer);
                } else {
                    fprintf(stderr, "-New marker %d: 0x%x\n", index, byte2);
                    return -1;
                }
            }
        } else if (buffer->offset == 7) {
            buffer->ptr += 1;
        }
    }

    uint8_t value = byte1 >> (7 - buffer->offset);

    if (buffer->offset == 7) {
        buffer->offset = 0;
        buffer->ptr += 1;
    } else {
        buffer->offset += 1;
    }

    return value & 1;
}

static inline int ParseHuffmanFallback(DHT_TABLE *table, jpeg_bitbuffer *buffer)
{
    int bit;

    while ((bit = NextBit(buffer)) != -1) {
        if (table->Next(bit)) {
            return table->GetValue();
        }
    }

    return INVALID_DECIMAL;
}

static inline int ParseHuffman(DHT_TABLE *table, jpeg_bitbuffer *buffer)
{
    if (buffer->offset == 0) {
        if (buffer->ptr >= buffer->end - 2) {
            fprintf(stderr, "End reached\n");
            return INVALID_DECIMAL;
        }
    }

    uint32_t bytes = *(uint32_t *)buffer->ptr;
    uint8_t byte0 = bytes & 0xFF;
    uint8_t byte1 = bytes >> 8;

    bool skip = false;

    if (byte0 == 0xFF) {
        skip = byte1 == 0;

        if (skip) {
            byte1 = bytes >> 16;
        } else {
            fprintf(stderr, "-New marker %d: 0x%x 0x%x 0x%x\n", index, buffer->ptr[0],
                    buffer->ptr[1], buffer->ptr[2]);
        }
    }

    uint16_t combined = (byte0 << 8) | byte1;
    uint8_t value8 = (combined << buffer->offset) >> 8;

    DHT_PREFIX_ENTRY *entry = table->GetValue8(value8);

    if (entry->size) {
        if (skip && (entry->size + buffer->offset >= 8))
            buffer->ptr += 1;

        buffer->offset += entry->size;
        buffer->ptr += (buffer->offset / 8);
        buffer->offset = buffer->offset % 8;
        return entry->value;
    }

    return ParseHuffmanFallback(table, buffer);
}

static inline int ParseSignedDecimal(int size, jpeg_bitbuffer *buffer)
{
    if (!size)
        return 0;

    int real_value = 0;
    int real_sign = 0;

    uint8_t *tmp_ptr = buffer->ptr;
    uint32_t value = ntohl(*(uint32_t *)buffer->ptr);

    if (size > 8 && ((value >> 8) & 0xFFFF) == 0xFF00) {
        value = (value & 0xFFFF0000) | ((value << 8) & 0x0000FFFF);

        if (buffer->offset + size >= 16)
            buffer->ptr += 1;
    } else if ((value >> 16) == 0xFF00) {
        value = (value & 0xFF000000) | ((value << 8) & 0x00FFFFFF);

        if (buffer->offset + size >= 8)
            buffer->ptr += 1;
    }

    value <<= buffer->offset;
    bool sign = !(value & (1 << 31));
    value >>= (32 - size);

    if (sign)
        value -= (1 << (size)) - 1;

    buffer->offset += size;
    buffer->ptr += (buffer->offset / 8);
    buffer->offset = buffer->offset % 8;

    return value;
}

static inline void FreeChannel(jpeg_frame *frame, int index)
{
    JPEG_CHANNEL *channel = frame->channels[index];

    if (!channel)
        return;

    if (channel->blocks)
        delete channel->blocks_ptr;

    delete channel;
    frame->channels[index] = 0;
}

static inline void FreeQtable(jpeg_frame *frame, int index)
{
    JPEG_QTABLE *qtable = frame->qtables[index];

    if (!qtable)
        return;

    delete qtable;
    frame->qtables[index] = 0;
}

static inline void FreeDht(jpeg_frame *frame, int index)
{
    DHT_TABLE *dht = frame->dht[index];

    if (!dht)
        return;

    delete dht;
    frame->dht[index] = 0;
}

static inline void Cleanup(jpeg_frame *frame)
{
    if (!frame)
        return;

    for (int i = 0; i < sizeof(frame->channels) / sizeof(frame->channels[0]); i++)
        FreeChannel(frame, i);

    for (int i = 0; i < sizeof(frame->qtables) / sizeof(frame->qtables[0]); i++)
        FreeQtable(frame, i);

    for (int i = 0; i < sizeof(frame->dht) / sizeof(frame->dht[0]); i++)
        FreeDht(frame, i);
}

static inline bool ParseSOF0(jpeg_buffer *buffer, jpeg_frame *frame)
{
    frame->precision = Read8(buffer);
    frame->height = Read16(buffer);
    frame->width = Read16(buffer);
    frame->num_channels = Read8(buffer);

    // fprintf(stderr, "width: %d, height: %d, channels: %d\n", frame->width, frame->height,
    //        frame->num_channels);

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

        FreeChannel(frame, index);

        JPEG_CHANNEL *channel = new JPEG_CHANNEL();
        frame->channels[index] = channel;
        channel->id = id;
        channel->sx = sx;
        channel->sy = sy;
        channel->qtable = qtable;
        channel->num_blocks = 0;
        channel->blocks = 0;
        channel->blocks_ptr = 0;

        // fprintf(stderr, "Comp idx:%d: id:%x qt:%x, sx:%d sy:%d ptr:%p\n", i, id, qtable, sx, sy,
        //        channel);
    }

    return true;
}

static inline bool ParseDQT(jpeg_buffer *buffer, jpeg_frame *frame, uint16_t mark_length)
{
    uint8_t *end = buffer->ptr + mark_length - 2;

    while (buffer->ptr < end) {
        uint8_t byte = Read8(buffer);
        uint8_t id = byte & 0xF;
        uint8_t precision = byte >> 4;

        // fprintf(stderr, "Qtable %d %d\n", id, precision);

        if (id >= sizeof(frame->qtables) / sizeof(frame->qtables[0]))
            return false;

        FreeQtable(frame, id);

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

static inline __m128i mul_row(const void *a, const void *b)
{
    __m128i x0, x1, x2, x3, v0, v1, v2, v3;
    __m128i val, sum, res1, res2;

    val = _mm_load_si128((__m128i *)a);

    v0 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[0]));
    v1 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[1]));
    v2 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[2]));
    v3 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[3]));
    sum = _mm_hadd_epi32(_mm_hadd_epi32(v0, v1), _mm_hadd_epi32(v2, v3));
    res1 = _mm_srai_epi32(sum, NUMBER_SCALE_SHIFT);

    v0 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[4]));
    v1 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[5]));
    v2 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[6]));
    v3 = _mm_madd_epi16(val, _mm_load_si128(&((__m128i *)b)[7]));
    sum = _mm_hadd_epi32(_mm_hadd_epi32(v0, v1), _mm_hadd_epi32(v2, v3));
    res2 = _mm_srai_epi32(sum, NUMBER_SCALE_SHIFT);

    return _mm_packs_epi32(res1, res2);
}

static inline void mul_mat(const void *a, const void *b, void *dst, bool add)
{
    int16_t tmp[64] __attribute__((aligned(16)));
    uint16_t *ptr_dst = (uint16_t *)dst;

    _mm_store_si128((__m128i *)&tmp[0], mul_row(&((__m128i *)a)[0], b));
    _mm_store_si128((__m128i *)&tmp[8], mul_row(&((__m128i *)a)[1], b));
    _mm_store_si128((__m128i *)&tmp[16], mul_row(&((__m128i *)a)[2], b));
    _mm_store_si128((__m128i *)&tmp[24], mul_row(&((__m128i *)a)[3], b));
    _mm_store_si128((__m128i *)&tmp[32], mul_row(&((__m128i *)a)[4], b));
    _mm_store_si128((__m128i *)&tmp[40], mul_row(&((__m128i *)a)[5], b));
    _mm_store_si128((__m128i *)&tmp[48], mul_row(&((__m128i *)a)[6], b));
    _mm_store_si128((__m128i *)&tmp[56], mul_row(&((__m128i *)a)[7], b));

    // TODO: optimize

    if (add) {
        for (int y = 0; y < 8; y++) {
            for (int i = 0; i < 8; i++) {
                ptr_dst[i * 8 + y] += tmp[y * 8 + i];
            }
        }
    } else {
        for (int y = 0; y < 8; y++) {
            for (int i = 0; i < 8; i++) {
                ptr_dst[i * 8 + y] = tmp[y * 8 + i];
            }
        }
    }
}

static inline bool ParseSOS(jpeg_buffer *buffer, jpeg_frame *frame, uint16_t mark_length)
{
    memset(frame->dc, 0, sizeof(frame->dc));

    jpeg_bitbuffer bitbuffer;
    bitbuffer.ptr = buffer->ptr + mark_length - 2;
    bitbuffer.end = buffer->end;
    bitbuffer.offset = 0;

    // Allocate blocks
    frame->total_blocks = 0;

    for (int i = 0; i < frame->num_channels; i++) {
        JPEG_CHANNEL *channel = frame->channels[i];
        channel->block_index = 0;
        channel->num_blocks = frame->bx * frame->by / channel->sy / channel->sx;

        size_t blocks_size = sizeof(JPEG_BLOCK) * channel->num_blocks;

        if (!channel->blocks) {
            char *unaligned = new char[blocks_size + 16];
            char *aligned = unaligned + (16 - (size_t)unaligned % 16) % 16;
            channel->blocks_ptr = unaligned;
            channel->blocks = (JPEG_BLOCK *)aligned;
        }

        memset(channel->blocks, 0, blocks_size);
        frame->total_blocks += channel->num_blocks;

        // TODO: Optimize
        if (i == 0) {
            for (int j = 0; j < channel->num_blocks; j++) {
                JPEG_BLOCK *block = &channel->blocks[j];

                for (int k = 0; k < 64; k++)
                    block->result[k] = 128;
            }
        }
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
            // fprintf(stderr, "Restart interval\n");
            memset(frame->dc, 0, sizeof(frame->dc));

            if (bitbuffer.ptr[0] == 0xFF && bitbuffer.ptr[1] == 0)
                bitbuffer.ptr += 1;

            if (bitbuffer.offset) {
                bitbuffer.ptr += 1;
                bitbuffer.offset = 0;
            }
        }

        // DC
        int dc_size = ParseHuffman(frame->dht[type | DHT_DC], &bitbuffer);
        int dc_value = ParseSignedDecimal(dc_size, &bitbuffer);
        frame->dc[ci] += dc_value;
        block->dct[0] = frame->dc[ci] * qtable->values[0];

        // AC
        int ac_index = 1;
        while (ac_index < 64) {
            int value = ParseHuffman(frame->dht[type | DHT_AC], &bitbuffer);

            if (value == 0) { // EOB
                break;
            } else if (value == 0xF0) { // ZRL
                ac_index += 16;
            } else {
                int zeroes = value >> 4;
                int dc_size = value & 0xF;
                ac_index += zeroes;

                int ac_value = ParseSignedDecimal(dc_size, &bitbuffer);

                if (ac_index >= 64 || ac_index < 0) {
                    fprintf(stderr, "AC index %d %d %d %d %p %d\n", ac_index, index, zeroes, bi,
                            bitbuffer.ptr, bitbuffer.offset);
                    return false;
                }

                int index = zigzag_map[ac_index];
                block->dct[index] = ac_value * qtable->values[ac_index];
                ac_index += 1;
            }
        }

        // DQT
        int16_t tmp[64] __attribute__((aligned(16)));
        mul_mat(block->dct, dct_transpose, tmp, false);
        mul_mat(tmp, dct_transpose, block->result, true);
    }

    return true;
}

static inline void Render1(jpeg_frame *frame, uint32_t *pixels)
{
    for (int y = 0; y < frame->by; y++) {
        for (int x = 0; x < frame->bx; x++) {
            int idx = y * frame->bx + x;
            JPEG_BLOCK *block = &frame->channels[0]->blocks[idx];

            for (int i = 0; i < 8; i++) {
                int absy = 8 * y + i;

                if (absy >= frame->height)
                    break;

                for (int j = 0; j < 8; j++) {
                    int absx = 8 * x + j;

                    if (absx >= frame->width)
                        break;

                    int bw = block->result[i * 8 + j];

                    bw = min(max(bw, 0), 255);
                    uint32_t col = ((uint8_t)bw << 16) | ((uint8_t)bw << 8) | (uint8_t)bw;
                    pixels[absy * frame->width + absx] = col;
                }
            }
        }
    }
}

static inline void Render3(jpeg_frame *frame, uint32_t *pixels)
{
    JPEG_BLOCK *block0 = frame->channels[0]->blocks;
    JPEG_BLOCK *block1 = frame->channels[1]->blocks;
    JPEG_BLOCK *block2 = frame->channels[2]->blocks;
    JPEG_BLOCK *block1_row = block1;
    JPEG_BLOCK *block2_row = block2;

    for (int y = 0; y < frame->by; y++) {
        for (int x = 0; x < frame->bx; x++) {
            int absy = 8 * y;
            int i2 = (absy / frame->sample_y) % 8;

            for (int i = 0; i < 8; i++) {
                if (absy >= frame->height)
                    break;

                int absx = 8 * x;
                int j2 = (absx / frame->sample_x) % 8;

                int16_t *result0 = &block0->result[i * 8];
                int16_t *result1 = &block1->result[i2 * 8 + j2];
                int16_t *result2 = &block2->result[i2 * 8 + j2];

                for (int j = 0; j < 8; j++) {
                    if (absx >= frame->width)
                        break;

                    int cy = *result0 << NUMBER_SCALE_SHIFT;
                    int cu = *result1;
                    int cv = *result2;

                    int16_t c1 = -352; // 0.344136
                    int16_t c2 = 1436; // 1.402
                    int16_t c3 = -731; // 0.714136
                    int16_t c4 = 1814; // 1.772

                    __m128i zero = _mm_setzero_si128();
                    __m128i cy128 = _mm_set_epi32(0, cy, cy, cy);
                    __m128i constants = _mm_set_epi16(0, c1, 0, c2, 0, c3, 0, c4);
                    __m128i variables = _mm_set_epi16(0, cu, 0, cv, 0, cv, 0, cu);
                    __m128i products = _mm_add_epi32(cy128, _mm_madd_epi16(constants, variables));

                    __m128i mask = _mm_set_epi32(0, 0, 0xFFFFFFFF, 0);
                    __m128i shuffled = _mm_shuffle_epi32(products, _MM_SHUFFLE(3, 3, 3, 3));
                    __m128i addition = _mm_and_si128(shuffled, mask);
                    __m128i rgb32_1024 = _mm_add_epi32(products, addition);
                    __m128i rgb32 = _mm_srai_epi32(rgb32_1024, NUMBER_SCALE_SHIFT);
                    __m128i rgb8 = _mm_packus_epi16(_mm_packs_epi32(rgb32, zero), zero);

                    uint32_t col = _mm_cvtsi128_si32(rgb8) & 0xFFFFFF;
                    pixels[absy * frame->width + absx] = col;

                    if (j % frame->sample_x == frame->sample_x - 1) {
                        result1++;
                        result2++;
                    }

                    result0++;
                    absx += 1;
                }

                if (i % frame->sample_y == frame->sample_y - 1)
                    i2 += 1;

                absy += 1;
            }

            if (x % frame->sample_x == frame->sample_x - 1) {
                block1++;
                block2++;
            }

            block0++;
        }

        if (y % frame->sample_y == frame->sample_y - 1) {
            block1_row = block1;
            block2_row = block2;
        } else {
            block1 = block1_row;
            block2 = block2_row;
        }
    }
}

static inline void Render(jpeg_frame *frame, uint32_t *pixels)
{
    // unsigned long ticks0 = get_ticks();

    if (frame->num_channels == 1) {
        Render1(frame, pixels);
    } else if (frame->num_channels == 3) {
        Render3(frame, pixels);
    }

    // printf("Render ticks %lu\n", (get_ticks() - ticks0) / 1000);
}

bool inline DoParse(void *data, size_t data_length, jpeg_frame *frame)
{
    if (!data || !data_length)
        return false;

    uint8_t *data_end = (uint8_t *)data + data_length;

    for (int data_index = 0; data_index < data_length - 1; data_index++) {
        uint8_t *ptr = (uint8_t *)data + data_index;
        uint16_t marker = ntohs(*(uint16_t *)ptr);

        if (marker <= 0xFF00)
            continue;

        ptr += 2;

        jpeg_buffer buffer;
        buffer.ptr = ptr;
        buffer.end = data_end;

        uint16_t mark_length = Read16(&buffer);

        // unsigned long ticks0 = get_ticks();

        switch (marker) {
        case MARK_DRT:
            frame->restart_interval = Read16(&buffer);
            fprintf(stderr, "Restart interval %d\n", frame->restart_interval);
            break;

        case MARK_SOF0:
            if (!ParseSOF0(&buffer, frame)) {
                fprintf(stderr, "Parse SOF0 failed\n");
                return false;
            }

            // printf("SOF0 ticks %lu\n", (get_ticks() - ticks0) / 1000);
            break;

        case MARK_DHT:
            if (!DHT_TABLE::Parse(ptr + 2, mark_length - 2, frame->dht)) {
                fprintf(stderr, "DHT parse failed\n");
                return false;
            }

            // printf("DHT ticks %lu\n", (get_ticks() - ticks0) / 1000);
            break;

        case MARK_DQT:
            if (!ParseDQT(&buffer, frame, mark_length)) {
                fprintf(stderr, "Parse DQT failed\n");
                return false;
            }

            // printf("DQT ticks %lu\n", (get_ticks() - ticks0) / 1000);
            break;

        case MARK_SOS:
            if (!ParseSOS(&buffer, frame, mark_length)) {
                fprintf(stderr, "Parse SOS failed\n");
                return false;
            }

            // printf("SOS ticks %lu\n", (get_ticks() - ticks0) / 1000);
            return true;
        }
    }

    return false;
}

JPEG::~JPEG()
{
    if (pixels)
        delete[] pixels;

    Cleanup(frame);
    delete frame;
}

bool JPEG::Update(void *data, size_t length)
{
    if (!DoParse(data, length, frame))
        return false;

    Render(frame, pixels);
    return true;
}

JPEG *JPEG::Parse(void *data, size_t length)
{
    jpeg_frame *frame = new jpeg_frame();

    if (DoParse(data, length, frame)) {
        // TODO: validate
        JPEG *jpeg = new JPEG();
        jpeg->frame = frame;
        jpeg->width = frame->width;
        jpeg->height = frame->height;
        jpeg->pixels = new uint32_t[frame->width * frame->height];
        Render(frame, jpeg->pixels);
        return jpeg;
    }

    Cleanup(frame);
    delete frame;
    return 0;
}
