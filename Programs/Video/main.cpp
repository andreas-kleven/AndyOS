#include "GUI.h"
#include <AndyOS.h>
#include <andyos/avi.h>
#include <andyos/drawing.h>
#include <andyos/jpeg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace gui;

class MainWindow : public Window
{
  public:
    MainWindow(const char *title, int width, int height)
        : Window(title, width, height, Color::Black)
    {}

    void OnClose() { exit(0); }
};

void usage(char *progname)
{
    fprintf(stderr, "Usage: %s file\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage(argv[0]);

    const char *filename = argv[1];

    FILE *avifile = fopen(filename, "r");

    if (!avifile) {
        perror("Could not open video file");
        return 1;
    }

    AVI *avi = AVI::Open(avifile);

    if (!avi) {
        fprintf(stderr, "Avi open failed\n");
        return 1;
    }

    int audiofd = open("/dev/ac97", O_RDWR);

    if (audiofd < 0) {
        perror("Could not open audio device");
        return 1;
    }

    MainWindow *window = 0;
    JPEG *jpeg = 0;
    char *avibuf = 0;
    uint32_t avisize = 0;

    while (true) {
        uint32_t sample_size = avi->sample_size;
        uint32_t sample_type = avi->sample_type;

        // printf("Sample %p %d\n", sample_type, sample_size);

        if (!sample_size)
            break;

        if (sample_type != FOURCC_CD00 && sample_type != FOURCC_BW10) {
            if (!avi->Read(0)) {
                perror("Skip error\n");
                return 1;
            }

            continue;
        }

        if (sample_size > avisize) {
            if (avibuf)
                delete[] avibuf;

            avibuf = new char[sample_size];
            avisize = sample_size;
        }

        if (!avi->Read(avibuf)) {
            perror("Read error");
            return 1;
        }

        switch (sample_type) {
        case FOURCC_CD00: {
            if (jpeg) {
                if (!jpeg->Update(avibuf, sample_size)) {
                    fprintf(stderr, "Update error\n");
                    return 1;
                }

            } else {
                jpeg = JPEG::Parse(avibuf, sample_size);

                if (!jpeg) {
                    fprintf(stderr, "Parse error\n");
                    return 1;
                }
            }

            if (!window)
                window = new MainWindow(filename, jpeg->width, jpeg->height);

            window->gc.DrawImage(0, 0, jpeg->width, jpeg->height, jpeg);
            window->Paint();
        } break;

        case FOURCC_BW10: {
            if (write(audiofd, avibuf, sample_size) == -1) {
                perror("Write audio");
            }
        } break;
        }
    }
}
