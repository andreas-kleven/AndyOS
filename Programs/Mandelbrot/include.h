#include <andyos/drawing.h>

Color mandelbrot_render(const GC &gc, int sx, int sy, bool julia);
void mandelbrot_update(double delta);
void mandelbrot_init();

Color raymarch_render(const GC &gc, int sx, int sy);
void raymarch_update(double delta);
void raymarch_init();
