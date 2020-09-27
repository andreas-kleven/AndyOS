#include "GUI.h"
#include "include.h"
#include <AndyOS.h>
#include <andyos/drawing.h>
#include <andyos/math.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace gui;

const int mandelbrot_max_scale = 4;
const int raymarch_max_scale = 8;

int max_scale = mandelbrot_max_scale;
bool juliaset = false;
bool raymarch = false;

bool any_key_down()
{
    return InputManager::GetKeyDown(KEY_LCTRL) || InputManager::GetKeyDown(KEY_SPACE) ||
           InputManager::GetKeyDown(KEY_R) || InputManager::GetKeyDown(KEY_W) ||
           InputManager::GetKeyDown(KEY_A) || InputManager::GetKeyDown(KEY_S) ||
           InputManager::GetKeyDown(KEY_D) || InputManager::GetKeyDown(KEY_Q) ||
           InputManager::GetKeyDown(KEY_E) || InputManager::GetKeyDown(KEY_UP) ||
           InputManager::GetKeyDown(KEY_DOWN) || InputManager::GetKeyDown(KEY_LEFT) ||
           InputManager::GetKeyDown(KEY_RIGHT) || InputManager::GetKeyDown(KEY_D1) ||
           InputManager::GetKeyDown(KEY_D2) || InputManager::GetKeyDown(KEY_D3);

    for (int i = 0; i < 100; i++)
        if (InputManager::GetKeyDown((KEYCODE)i))
            return true;

    return false;
}

void render(Window *wnd, GC &gc, int scale)
{
    int width = gc.width;
    int height = gc.height;
    int step = scale == max_scale ? scale : 1;
    int offset = scale == max_scale ? 0 : scale - 1;

    uint64_t prev_time = get_ticks();

    for (int sy = offset; sy < height; sy += step) {
        if (scale != max_scale && any_key_down())
            return;

        if (scale == max_scale) {
            if (sy % scale != 0)
                continue;
        } else if (sy % scale != scale - 1) {
            continue;
        }

        for (int sx = offset; sx < width; sx += step) {
            if (scale == max_scale) {
                if (sx % scale != 0)
                    continue;
            } else if (sx % scale != scale - 1) {
                continue;
            }

            Color color;

            if (raymarch)
                color = raymarch_render(gc, sx, sy);
            else
                color = mandelbrot_render(gc, sx, sy, juliaset);

            gc.FillRect(sx, sy, scale, scale, color);
        }

        if (raymarch) {
            if (get_ticks() - prev_time > 100000) {
                wnd->Paint();
                prev_time = get_ticks();
            }
        }
    }
}

void run(Window *wnd)
{
    mandelbrot_init();
    raymarch_init();

    int ticks = get_ticks();
    double delta = 1;
    int scale = max_scale;
    int mode = 1;
    bool change_mode = false;
    bool done = false;

    GC &gc = wnd->gc;
    GC gc_buf;

    while (1) {
        delta = clamp(delta, 0.0, 0.1);

        if (gc_buf.width != gc.width || gc_buf.height != gc.height) {
            gc_buf = GC(gc.width, gc.height);
            scale = max_scale;
            done = false;
        }

        if (raymarch)
            raymarch_update(delta);
        else
            mandelbrot_update(delta);

        if (InputManager::GetKeyDown(KEY_D1)) {
            change_mode = true;
            mode = 1;
        }

        if (InputManager::GetKeyDown(KEY_D2)) {
            change_mode = true;
            mode = 2;
        }

        if (InputManager::GetKeyDown(KEY_D3)) {
            change_mode = true;
            mode = 3;
        }

        if (change_mode) {
            juliaset = mode == 2;
            raymarch = mode == 3;
            max_scale = raymarch ? raymarch_max_scale : mandelbrot_max_scale;
            scale = max_scale;
            change_mode = false;
        }

        if (!done) {
            render(wnd, gc, scale);
            wnd->Paint();
        }

        if (any_key_down()) {
            done = false;
            scale = max_scale;
        } else if (scale > 1) {
            scale /= 2;
        } else {
            done = true;
            usleep(100 * 1000);
        }

        int delta_ticks = get_ticks() - ticks;
        delta = delta_ticks / 1000000.0f;
        ticks = get_ticks();
        printf("Ticks: %i\tFPS: %i\tScale: %i\n", delta_ticks, (int)(1 / delta), scale);
    }
}

class MainWindow : public Window
{
  public:
    MainWindow() : Window("Mandelbrot") { SetCapture(true); }

    void OnClose() { exit(0); }
};

int main()
{
    MainWindow wnd;
    run(&wnd);
    return 0;
}
