#include "wav.h"
#include <AndyOS.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BUFFER_SIZE 0x2000
#define MAX_SAMPLES 0x1000

#define AC97_IOCTL_SET_VOLUME _IO('A', 1)

struct sample_buffer
{
    int fd;
    uint16_t *ptr;
    size_t size;
    size_t position;
};

struct input_buffer
{
    void *ptr;
    size_t size;
    int bytes;
};

void *read_header(WAVE_CHUNK *chunk, const char *id, size_t header_size)
{
    if (chunk->size + sizeof(WAVE_CHUNK) < header_size)
        return 0;

    if (memcmp(chunk->id, id, 4) == 0) {
        char *ptr = new char[header_size];
        memcpy(ptr, chunk, header_size);
        return ptr;
    }

    return 0;
}

uint16_t read_sample(input_buffer *buffer, size_t offset)
{
    offset %= buffer->size;
    char *ptr = (char *)buffer->ptr + offset;

    if (offset + buffer->bytes >= buffer->size)
        return 0;

    if (buffer->bytes == 1)
        return (uint16_t)(*(uint8_t *)ptr) * 256;

    if (buffer->bytes == 2)
        return *(uint16_t *)ptr;

    if (buffer->bytes == 4)
        return *(uint32_t *)ptr / 256;

    return 0;
}

void flush_samples(sample_buffer *buffer)
{
    if (!buffer->position)
        return;

    size_t write_size = buffer->position * 2;

    if (write(buffer->fd, buffer->ptr, write_size) != write_size) {
        perror("Write audio");
        exit(1);
    }

    buffer->position = 0;
}

void write_sample(sample_buffer *buffer, uint16_t sample)
{
    buffer->ptr[buffer->position++] = sample;

    if (buffer->position >= buffer->size)
        flush_samples(buffer);
}

void usage(char *progname, int opt)
{
    fprintf(stderr, "Usage: %s file [-v volume] [-h]\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int volume = 0;
    char filename[PATH_MAX] = {0};

    int opt;

    while ((opt = getopt(argc, argv, "-:v:h")) != EOF) {
        switch (opt) {
        case 'v':
            volume = (int)strtoul(optarg, NULL, 0);
            break;

        case 1:
            strncpy(filename, optarg, sizeof(filename));
            break;

        case 'h':
        default:
            usage(basename(argv[0]), opt);
            break;
        }
    }

    if (strlen(filename) == 0)
        usage(argv[0], 0);

    char *filedata = new char[BUFFER_SIZE];

    int devfd = open("/dev/ac97", O_RDWR);

    if (devfd < 0) {
        perror("Could not open audio device");
        return 1;
    }

    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Could not open audio file");
        return 1;
    }

    if (fread(filedata, 1, sizeof(WAVE_HEADER), file) != sizeof(WAVE_HEADER)) {
        perror("Read error");
        return 1;
    }

    WAVE_HEADER *header = (WAVE_HEADER *)filedata;

    if (memcmp(header->checksum, "RIFF", 4)) {
        printf("Invalid checksum\n");
        return 1;
    }

    if (memcmp(header->format, "WAVE", 4)) {
        printf("Unknown format\n");
        return 1;
    }

    size_t filesize = 0;
    WAVE_FORMAT_HEADER *format = 0;
    WAVE_DATA_HEADER *data = 0;

    while (true) {
        if (fread(filedata, 1, sizeof(WAVE_CHUNK), file) != sizeof(WAVE_CHUNK)) {
            perror("Read error");
            return 1;
        }

        WAVE_CHUNK *chunk = (WAVE_CHUNK *)filedata;
        size_t header_size = chunk->size + sizeof(WAVE_CHUNK);
        filesize += chunk->size;

        if (memcmp(chunk->id, "data", 4) == 0)
            header_size = sizeof(WAVE_DATA_HEADER);

        char tmp[5];
        memcpy(tmp, chunk->id, 4);
        tmp[4] = 0;
        printf("Chunk %s  : %lu\n", tmp, header_size);

        if (header_size >= BUFFER_SIZE) {
            printf("Chunk error\n");
            return 1;
        }

        size_t rest_size = header_size - sizeof(WAVE_CHUNK);

        if (fread(filedata + sizeof(WAVE_CHUNK), 1, rest_size, file) != rest_size) {
            perror("Read error");
            return 1;
        }

        if (!format)
            format = (WAVE_FORMAT_HEADER *)read_header(chunk, "fmt ", sizeof(WAVE_FORMAT_HEADER));

        if (!data) {
            data = (WAVE_DATA_HEADER *)read_header(chunk, "data", sizeof(WAVE_DATA_HEADER));

            if (data)
                break;
        }
    }

    if (!format) {
        printf("Missing format chunk\n");
        return 1;
    }

    if (!data) {
        printf("Missing data chunk\n");
        return 1;
    }

    if (format->audioformat != 1) {
        printf("Not PCM\n");
        return 1;
    }

    bool duplicate = format->samplerate == 22050 || format->samplerate == 24000;
    size_t samples = data->size / format->blockalign;
    size_t bitrate = format->samplerate * format->blockalign * 8;
    int seconds = samples / format->samplerate;
    int hh = seconds / 3600;
    int mm = (seconds - hh * 3600) / 60;
    int ss = seconds - hh * 3600 - mm * 60;

    printf("\n");
    printf("Input file  : '%s'\n", filename);
    printf("Channels    : %d\n", format->numchannels);
    printf("Sample rate : %u\n", format->samplerate);
    printf("Precision   : %d-bit\n", format->bitspersample);
    printf("Duration    : %02d:%02d:%02d = %lu samples\n", hh, mm, ss, samples);
    printf("File size   : %.1fK\n", (float)filesize / 1000);
    printf("Bit rate    : %.1fK\n", (float)bitrate / 1000);
    printf("\n\n");

    sample_buffer output;
    output.fd = devfd;
    output.size = MAX_SAMPLES;
    output.ptr = new uint16_t[output.size];
    output.position = 0;

    input_buffer input;
    input.ptr = filedata;
    input.size = 0;
    input.bytes = format->bitspersample / 8;

    if (volume)
        ioctl(devfd, AC97_IOCTL_SET_VOLUME, volume);

    for (size_t offset = 0; offset < data->size; offset += input.bytes) {
        if (offset % BUFFER_SIZE == 0) {
            if ((input.size = fread(filedata, 1, BUFFER_SIZE, file)) == 0) {
                if (!feof(file)) {
                    perror("Read data error");
                    return 1;
                }
            }
        }

        if (input.size == 0)
            break;

        if (offset % (format->samplerate / 10) == 0) {
            float time = (float)offset / format->samplerate / format->blockalign;
            float total = (float)data->size / format->samplerate / format->blockalign;
            char buf[128];
            snprintf(buf, sizeof(buf), "\033[1F\033[KTime: %.1f / %.1f\n", time, total);
            printf(buf);
            fflush(stdout);
        }

        int count = 1;

        if (format->numchannels == 1)
            count *= 2;

        if (duplicate)
            count *= 2;

        uint16_t value = read_sample(&input, offset);

        for (int i = 0; i < count; i++)
            write_sample(&output, value);
    }

    flush_samples(&output);
    printf("Done\n");
    return 0;
}
