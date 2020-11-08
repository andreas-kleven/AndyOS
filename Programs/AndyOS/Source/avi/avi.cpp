#include <andyos/avi.h>
#include <string.h>
#include <unistd.h>

struct RIFF_CHUNK
{
    uint32_t fourcc;
    uint32_t size;
} __attribute__((packed));

struct RIFF_LIST
{
    uint32_t list;
    uint32_t size;
    uint32_t fourcc;
} __attribute__((packed));

struct AVI_AVIH
{
    uint32_t frame_micros;        // frame display rate (or 0)
    uint32_t max_transfer_rate;   // max. transfer rate
    uint32_t padding_granularity; // pad to multiples of this size;
    uint32_t flags;               // the ever-present flags
    uint32_t total_frames;        // # frames in file
    uint32_t initial_frames;
    uint32_t streams;
    uint32_t suggested_buffer;
    uint32_t width;
    uint32_t height;
    uint32_t reserved[4];
} __attribute__((packed));

struct AVI_STRH
{
    uint32_t type;
    uint32_t handler;
    uint32_t flags;
    uint16_t priority;
    uint16_t language;
    uint32_t initial_frames;
    uint32_t scale;
    uint32_t rate;
    uint32_t start;
    uint32_t length;
    uint32_t suggested_buffer;
    uint32_t quality;
    uint32_t sample_size;
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
} __attribute__((packed));

struct AVI_VIDS
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_pels_per_meter;
    uint32_t y_pels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
} __attribute__((packed));

struct AVI_AUDS
{
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
    // uint16_t size;
} __attribute__((packed));

struct avi_info
{
    AVI_AVIH avih;
    AVI_STRH strh_video;
    AVI_STRH strh_audio;
    AVI_VIDS vids;
    AVI_AUDS auds;
    fpos_t position;
};

static bool ReadChunkData(void *dst, size_t size, RIFF_CHUNK *chunk, FILE *file)
{
    if (size != chunk->size) {
        fprintf(stderr, "Read chunk data failed %lu %d\n", size, chunk->size);
        return false;
    }

    if (fread(dst, 1, size, file) != size)
        return false;

    return true;
}

static bool ReadList(FILE *file, RIFF_LIST *list, avi_info *info)
{
    // fprintf(stderr, "List 0x%x %d\n", list->fourcc, list->size);

    int total_read = 0;
    AVI_STRH strh;

    while (total_read < list->size) {
        char buf[sizeof(RIFF_LIST)];

        if (fread(&buf, 1, sizeof(RIFF_CHUNK), file) != sizeof(RIFF_CHUNK))
            return 0;

        RIFF_CHUNK *chunk = (RIFF_CHUNK *)buf;

        char copy[5] = {0};
        memcpy(copy, &chunk->fourcc, 4);

        // fprintf(stderr, "Chunk %s 0x%x, %d, %d < %d\n", copy, chunk->fourcc, chunk->size,
        //        total_read, list->size);

        switch (chunk->fourcc) {
            // HDRL
        case FOURCC_AVIH:
            if (!ReadChunkData(&info->avih, sizeof(AVI_AVIH), chunk, file))
                return false;
            break;

            // STRL
        case FOURCC_STRH:
            if (!ReadChunkData(&strh, sizeof(AVI_STRH), chunk, file))
                return false;

            if (strh.type == FOURCC_VIDS)
                info->strh_video = strh;
            else if (strh.type == FOURCC_AUDS)
                info->strh_audio = strh;

            break;

        case FOURCC_STRF:
            if (strh.type == FOURCC_VIDS) {
                if (!ReadChunkData(&info->vids, sizeof(AVI_VIDS), chunk, file))
                    return false;
            } else if (strh.type == FOURCC_AUDS) {
                if (!ReadChunkData(&info->auds, sizeof(AVI_AUDS), chunk, file))
                    return false;
            } else {
                fseek(file, chunk->size, SEEK_CUR);
            }
            break;

            // Other
        case FOURCC_JUNK:
            fseek(file, chunk->size, SEEK_CUR);
            return true;

        case FOURCC_LIST: {
            int size = sizeof(RIFF_LIST) - sizeof(RIFF_CHUNK);

            if (fread(buf + sizeof(RIFF_CHUNK), 1, size, file) != size)
                return false;

            RIFF_LIST *sublist = (RIFF_LIST *)buf;

            if (sublist->fourcc == FOURCC_MOVI) {
                info->position = ftell(file);
                return true;
            }

            if (!ReadList(file, sublist, info))
                return false;

            total_read += size;
        } break;

        default:
            fseek(file, chunk->size, SEEK_CUR);
            fprintf(stderr, "Unknown chunk type %x\n", chunk->fourcc);
            break;
        }

        total_read += chunk->size + sizeof(RIFF_CHUNK);

        if (total_read % 2 == 1) {
            fseek(file, 1, SEEK_CUR);
            total_read += 1;
        }
    }

    return true;
}

AVI *AVI::Open(FILE *file)
{
    RIFF_LIST root_list;

    if (fread(&root_list, 1, sizeof(root_list), file) != sizeof(root_list)) {
        perror("Avi read");
        return 0;
    }

    if (root_list.list != FOURCC_RIFF || root_list.fourcc != FOURCC_AVI) {
        fprintf(stderr, "Not riff\n");
        return 0;
    }

    avi_info *info = new avi_info();

    if (!ReadList(file, &root_list, info)) {
        delete info;
        return 0;
    }

    AVI *avi = new AVI();
    avi->file = file;
    avi->info = info;

    if (!avi->ReadSampleInfo()) {
        delete avi;
        return 0;
    }

    return avi;
}

AVI::~AVI()
{
    delete info;
}

bool AVI::Read(void *buf)
{
    info->position += sample_size;

    if (buf) {
        if (fread(buf, 1, sample_size, file) != sample_size)
            return false;
    } else {
        if (fseek(file, sample_size, SEEK_CUR) != 0)
            return false;
    }

    if (!ReadSampleInfo()) {
        sample_type = -1;
        sample_size = 0;
    }

    return true;
}

bool AVI::ReadSampleInfo()
{
    if (info->position % 2 == 1) {
        info->position += 1;

        if (fseek(file, 1, SEEK_CUR) != 0)
            return false;
    }

    uint32_t buf[2];

    if (fread(buf, 1, sizeof(buf), file) != sizeof(buf))
        return false;

    sample_type = buf[0];
    sample_size = buf[1];
    return true;
}
