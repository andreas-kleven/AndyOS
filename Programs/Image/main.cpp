#include "GUI.h"
#include <AndyOS.h>
#include <andyos/drawing.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace gui;

class MainWindow : public Window
{
    IMAGE *image = 0;

  public:
    MainWindow(const char *title, int width, int height)
        : Window(title, width, height, Color::Black)
    {}

    void OnClose() { exit(0); }

    void OnResize() { RenderImage(); }

    void SetImage(IMAGE *img)
    {
        this->image = img;
        RenderImage();
    }

    void RenderImage()
    {
        if (!image)
            return;

        gc.DrawImage(0, 0, image->width, image->height, image);
        Paint();
    }
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

    char *filename = argv[1];
    IMAGE *img = IMAGE::Load(filename);

    if (!img) {
        fprintf(stderr, "Could not load image\n");
        return 1;
    }

    MainWindow *window = new MainWindow(filename, img->width, img->height);
    window->SetImage(img);

    while (true)
        sleep(1000);
}
