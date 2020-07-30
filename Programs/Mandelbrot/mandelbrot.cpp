#include <math.h>
#include <andyos/drawing.h>
#include <andyos/math.h>
#include <AndyOS.h>
#include "GUI.h"

using namespace gui;

const int iter_max = 256;
const int color_count = 256;
Color *colors;

double zoom = 4;
double ofx = 0;
double ofy = 0;
double juliax = 1;
double juliay = 0;

inline int get_iteration(double x, double y, bool julia, double *z2)
{
    double Zx = 0;
    double Zy = 0;
    double Zx2 = 0;
    double Zy2 = 0;

    if (julia)
    {
        Zx = x;
        Zy = y;
        x = juliax;
        y = juliay;
    }
    else
    {
        //Optimizations
        double q = pow(x - 0.25, 2) + y * y;
        if ((q * (q + (x - 0.25)) < y * y / 4)       //Inside cardioid
            || ((x + 1) * (x + 1) + y * y) * 16 < 1) //Inside period-2 bulb
        {
            *z2 = 0;
            return iter_max;
        }
    }

    Zx2 = Zx * Zx;
    Zy2 = Zy * Zy;

    int iter = 0;
    while (iter < iter_max && ((Zx2 + Zy2) < (1 << 16)))
    {
        Zy = 2 * Zx * Zy + y;
        Zx = Zx2 - Zy2 + x;
        Zx2 = Zx * Zx;
        Zy2 = Zy * Zy;
        iter++;
    }

    *z2 = Zx2 + Zy2;
    return iter;
}

Color mandelbrot_render(const GC &gc, int sx, int sy, bool julia)
{
    double sens = zoom / gc.width;
    double x = (sx - gc.width / 2) * sens + ofx;
    double y = (sy - gc.height / 2) * sens + ofy;

    double z2;
    int iter = get_iteration(x, y, julia, &z2);

    if (iter == iter_max)
        return Color::Black;

    double zn = sqrt(z2);
    double smooth = iter - log2(log(zn) / log(1 << 16));
    double f = smooth / iter_max;

    int i = (int)(f * 2000);
    return colors[i % color_count];
}

void mandelbrot_update(double delta)
{
    double multiplier = zoom * delta;
    if (InputManager::GetKeyDown(KEY_LSHIFT))
        multiplier *= 4;

    if (InputManager::GetKeyDown(KEY_LCTRL))
        zoom += 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_SPACE))
        zoom -= 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_A))
        ofx -= 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_D))
        ofx += 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_W))
        ofy -= 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_S))
        ofy += 1.0f * multiplier;
    if (InputManager::GetKeyDown(KEY_RIGHT))
        juliax += 0.2f * multiplier;
    if (InputManager::GetKeyDown(KEY_LEFT))
        juliax -= 0.2f * multiplier;
    if (InputManager::GetKeyDown(KEY_UP))
        juliay += 0.2f * multiplier;
    if (InputManager::GetKeyDown(KEY_DOWN))
        juliay -= 0.2f * multiplier;
}

void mandelbrot_init()
{
    colors = new Color[color_count];

    const int res = color_count / 16;

    int i = 0;
    colors[i++ * res] = Color(66.f / 256, 30.f / 256, 15.f / 255);
    colors[i++ * res] = Color(25.f / 256, 7.f / 256, 26.f / 255);
    colors[i++ * res] = Color(9.f / 256, 1.f / 256, 47.f / 255);
    colors[i++ * res] = Color(4.f / 256, 4.f / 256, 73.f / 255);
    colors[i++ * res] = Color(0.f / 256, 7.f / 256, 100.f / 255);
    colors[i++ * res] = Color(12.f / 256, 44.f / 256, 138.f / 255);
    colors[i++ * res] = Color(24.f / 256, 82.f / 256, 177.f / 255);
    colors[i++ * res] = Color(57.f / 256, 125.f / 256, 209.f / 255);
    colors[i++ * res] = Color(134.f / 256, 181.f / 256, 229.f / 255);
    colors[i++ * res] = Color(211.f / 256, 236.f / 256, 248.f / 255);
    colors[i++ * res] = Color(241.f / 256, 233.f / 256, 191.f / 255);
    colors[i++ * res] = Color(248.f / 256, 201.f / 256, 95.f / 255);
    colors[i++ * res] = Color(255.f / 256, 170.f / 256, 0.f / 255);
    colors[i++ * res] = Color(204.f / 256, 128.f / 256, 0.f / 255);
    colors[i++ * res] = Color(153.f / 256, 87.f / 256, 0.f / 255);
    colors[i++ * res] = Color(106.f / 256, 52.f / 256, 3.f / 255);

    Color c0;
    Color c1;

    for (int i = 0; i <= 16; i++)
    {
        c0 = colors[i * res];
        c1 = colors[((i + 1) * res) % color_count];

        for (int j = 1; j < res; j++)
        {
            double alpha = (double)j / res;
            colors[i * res + j] = c1 * alpha + c0 * (1 - alpha);
        }
    }
}
