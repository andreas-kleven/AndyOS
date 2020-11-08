#include <AndyOS.h>
#include <andyos/bmp.h>
#include <andyos/image.h>
#include <andyos/jpeg.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILE_SIZE 16777216 // 16 MB

IMAGE *IMAGE::Load(const char *filename)
{
    char *ext = strrchr(filename, '.');

    if (!ext) {
        fprintf(stderr, "No file extension\n");
        return 0;
    }

    ext += 1;
    FILE *file = fopen(filename, "r");

    if (!file)
        return 0;

    setvbuf(file, NULL, _IONBF, 0);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0 || size >= MAX_FILE_SIZE)
        return 0;

    IMAGE *img = 0;
    char *buf = new char[size];

    if (fread(buf, 1, size, file) == size) {
        if (strcmp(ext, "bmp") == 0) {
            img = BMP::Parse(buf, size);
        } else if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
            img = JPEG::Parse(buf, size);
        } else {
            fprintf(stderr, "Unknown file extension\n");
        }
    }

    fclose(file);
    delete[] buf;
    return img;
}
