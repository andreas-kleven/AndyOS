#include <AndyOS.h>
#include <sys/drawing.h>
#include "GUI.h"
#include "math.h"

using namespace gui;

double zoom = 4;
double ofx = 0;
double ofy = 0;
double rot = 0;

void run(GC& gc)
{
	int iter_max = 256;

	int ticks = get_ticks();
	double delta = 1;

	GC gc_buf;

	while (1)
	{
		if (gc_buf.width != gc.width || gc_buf.height != gc.height);
			gc_buf = GC(gc.width, gc.height);

		double asd0 = zoom;
		double asd1 = asd0 / gc_buf.width;

		if (InputManager::GetKeyDown(KEY_LCTRL)) zoom += 1.0f * delta * asd0;
		if (InputManager::GetKeyDown(KEY_SPACE)) zoom -= 1.0f * delta * asd0;

		if (InputManager::GetKeyDown(KEY_A)) ofx -= 1.0f * delta * asd0;
		if (InputManager::GetKeyDown(KEY_D)) ofx += 1.0f * delta * asd0;
		if (InputManager::GetKeyDown(KEY_W)) ofy -= 1.0f * delta * asd0;
		if (InputManager::GetKeyDown(KEY_S)) ofy += 1.0f * delta * asd0;

		if (InputManager::GetKeyDown(KEY_R)) rot += 0.5f * delta * asd0;

		Drawing::Clear(Color::Black, gc_buf);
		debug_reset();

		int index = 0;
		uint32* buf = gc_buf.framebuffer + gc_buf.y * gc_buf.stride + gc_buf.x;

		double Cx;
		double Cy;
		double Zx = 0;
		double Zy = 0;
		double Zx2 = 0;
		double Zy2 = 0;

		for (int _y = 0; _y < gc_buf.height; _y++)
		{
			for (int _x = 0; _x < gc_buf.width; _x++)
			{
				Cx = (_x - gc_buf.width / 2) * asd1 + ofx;
				Cy = (_y - gc_buf.height / 2) * asd1 + ofy;
				Zx = 0;
				Zy = 0;
				Zx2 = 0;
				Zy2 = 0;

				int iter = 0;
				/*for (iter = 0; iter < iter_max && ((Zx2 + Zy2) < (1 << 16)); iter++)
				{
					Zy = Zx * Zy;
					Zy += Zy;
					Zy += cy;

					Zx = Zx2 - Zy2 + cx;

					Zx2 = Zx * Zx;
					Zy2 = Zy * Zy;

					//Zy = Zx * Zy * 2.0 + cy;
					//Zx = Zx2 - Zy2 + cx;
					//
					//Zx2 = Zx * Zx;
					//Zy2 = Zy * Zy;
				}*/

				/*double Zx = cx;
				double Zy = cy;

				double Cx = 0.7885 * cos(rot);
				double Cy = 0.7885 * sin(rot);

				Zx2 = Zx*Zx;
				Zy2 = Zy*Zy;*/

				while (iter < iter_max && ((Zx2 + Zy2) < (1 << 16)))
				{
					Zy = 2 * Zx*Zy + Cy;
					Zx = Zx2 - Zy2 + Cx;
					Zx2 = Zx*Zx;
					Zy2 = Zy*Zy;
					iter++;
				}

				if (iter == iter_max)
				{
					*buf++ = 0;
				}
				else
				{
					//*buf++ = iter;
					//continue;

					double zn = sqrt(Zx2 + Zy2);
					double hue = iter + 1.0 - log2(log(zn));  // 2 is escape radius
					//int gg = clamp((int)(logn(hue, 200) * 0xFF), 0, 0xFF);

					uint32 color = (int)(logn(hue, 200) * 0xFF) % 0xFF;

					//int g = clamp((int)(logn(logn(hue, 5), 5) * 0xFF), 0, 0xFF);
					//int g2 = clamp((int)(logn(logn(hue, 6), 6) * 0xFF), 0, 0xFF);
					//color += g2 << 16;
					//color += g << 8;

					*buf++ = color;

					//*buf++ = palette.GetColor(iter);
					//buf++ = palette.GetColor(color);
				}

				/*z.real = _x * asd1 - 2;
				z.img = _y * asd1 - 2;

				int res = test(z);

				if (res == 0)
				{
					VBE::SetPixel(_x, _y, 0);
				}
				else
				{
					//double val = logn(res, count) * 255;

					//uint8 b = clamp((int)((val + frac)), 0, 0xFF);
					uint8 b = res;

					uint32 c = (b << 16) | (b << 8) | b;
					VBE::SetPixel(_x, _y, c);
				}

				index++;

				/*double x0 = _x * asd1 - 2;
				double y0 = _y * asd1 - 2;
				double x = 0.0;
				double y = 0.0;
				double iteration = 0;
				int max_iteration = 1000;

				// Here N=2^8 is chosen as a reasonable bailout radius.
				while (x*x + y*y < (1 << 16) && iteration < max_iteration)
				{
					double xtemp = x*x - y*y + x0;
					y = 2 * x*y + y0;
					x = xtemp;
					iteration = iteration + 1;
				}

				// Used to avoid doubleing point issues with points inside the set.
				if (iteration < max_iteration)
				{
					// sqrt of inner term removed using log simplification rules.
					double log_zn = log(x*x + y*y) / 2;
					double nu = log(log_zn / log(2)) / log(2);

					// Rearranging the potential function.
					// Dividing log_zn by log(2) instead of log(N = 1<<8)
					// because we want the entire palette to range from the
					// center to radius 2, NOT our bailout radius.
					iteration = iteration + 1 - nu;
				}

				double color1 = (double)floor(iteration);
				double color2 = (double)floor(iteration) + 1;

				// iteration % 1 = fractional part of iteration.
				double frac = iteration - (int)iteration;

				uint8 color = (uint8)((color1 + (color2 - color1) * frac));
				VBE::SetPixel(_x, _y, color);*/
			}

			index += gc_buf.stride - gc_buf.width;
			buf += gc_buf.stride - gc_buf.width;
		}

		Drawing::BitBlt(gc_buf, 0, 0, gc_buf.width, gc_buf.height, gc, 0, 0);

		int delta_ticks = (get_ticks() - ticks);
		delta = delta_ticks / 1000.0f;
		ticks = get_ticks();
		debug_print("Ticks: %i\tFPS: %i\n", delta_ticks, (int)(1 / delta));
		debug_print("Zoom: %f\n", zoom);
	}
}

class MainWindow : public Window
{
public:
	MainWindow()
		: Window("Mandelbrot")
	{
		SetCapture(true);
		run(this->gc);
	}

	void OnClose()
	{
		exit(0);
	}
};

int main()
{
	MainWindow wnd;
	exit(0);
}