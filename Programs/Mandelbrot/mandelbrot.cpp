#include <AndyOS.h>
#include <sys/drawing.h>
#include "GUI.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "unistd.h"

using namespace gui;

const int iter_max = 256;
const int max_scale = 4;

const int color_count = 256;
Color* colors;

double zoom = 4;
double ofx = 0;
double ofy = 0;
double rot = 0;
bool juliaset = false;

bool any_key_down()
{
	return InputManager::GetKeyDown(KEY_LCTRL)
		|| InputManager::GetKeyDown(KEY_SPACE)
		|| InputManager::GetKeyDown(KEY_R)
		|| InputManager::GetKeyDown(KEY_W)
		|| InputManager::GetKeyDown(KEY_A)
		|| InputManager::GetKeyDown(KEY_S)
		|| InputManager::GetKeyDown(KEY_D);

	for (int i = 0; i < 100; i++)
		if (InputManager::GetKeyDown((KEYCODE)i))
			return true;

	return false;
}

int get_iteration(double x, double y, double* z2)
{
	double Zx = 0;
	double Zy = 0;
	double Zx2 = 0;
	double Zy2 = 0;

	if (juliaset)
	{
		Zx = x;
		Zy = y;

		x = 0.7885 * cos(rot);
		y = 0.7885 * sin(rot);
	}
	else
	{
		//Optimizations
		double q = pow(x - 0.25, 2) + y*y;
		if ((q * (q + (x - 0.25)) < y*y / 4) 	//Inside cardioid
			|| ((x+1) * (x+1) + y*y) * 16 < 1)	//Inside period-2 bulb
		{
			*z2 = 0;
			return iter_max;
		}
	}

	Zx2 = Zx*Zx;
	Zy2 = Zy*Zy;

	int iter = 0;
	while (iter < iter_max && ((Zx2 + Zy2) < (1 << 16)))
	{
		Zy = 2 * Zx*Zy + y;
		Zx = Zx2 - Zy2 + x;
		Zx2 = Zx*Zx;
		Zy2 = Zy*Zy;
		iter++;
	}

	*z2 = Zx2 + Zy2;
	return iter;
}

void render(GC& gc, int scale)
{
	int width = gc.width;
	int height = gc.height;

	double sens = zoom / width;

	for (int _y = 0; _y < height; _y += scale)
	{
		for (int _x = 0; _x < width; _x += scale)
		{
			double x = (_x - width / 2) * sens + ofx;
			double y = (_y - height / 2) * sens + ofy;

			double z2;
			int iter = get_iteration(x, y, &z2);

			Color color = Color::Black;

			if (iter != iter_max)
			{
				double zn = sqrt(z2);
				double smooth = iter - log2(log(zn) / log(1 << 16));
				double f = smooth / iter_max;
				int i = (int)(f * 2000);
				color = colors[i % color_count];
			}

			gc.FillRect(_x, _y, scale, scale, color);
		}
	}
}

void init_colors()
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
		c1 = colors[((i+1) * res) % color_count];

		for (int j = 1; j < res; j++)
		{
			double alpha = (double)j / res;
			colors[i * res + j] = c1 * alpha + c0 * (1 - alpha);
		}
	}
}

void run(GC& gc)
{
	init_colors();

	int ticks = get_ticks();
	double delta = 1;
	int scale = max_scale;

	bool julia_released = true;

	GC gc_buf;

	while (1)
	{
		if (gc_buf.width != gc.width || gc_buf.height != gc.height)
		{
			gc_buf = GC(gc.width, gc.height);
			scale = max_scale;
		}

		double multiplier = zoom * clamp(delta, 0.0, 0.1);
		if (InputManager::GetKeyDown(KEY_LSHIFT)) multiplier *= 4;

		if (InputManager::GetKeyDown(KEY_LCTRL)) zoom += 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_SPACE)) zoom -= 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_A)) ofx -= 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_D)) ofx += 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_W)) ofy -= 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_S)) ofy += 1.0f * multiplier;
		if (InputManager::GetKeyDown(KEY_R)) rot += 0.5f * multiplier;

		//Toggle julia set
		if (InputManager::GetKeyDown(KEY_J))
		{
			if (julia_released)
			{
				juliaset = !juliaset;
				julia_released = false;
				scale = max_scale;
			}
		}
		else
		{
			julia_released = true;
		}
		
		bool enable_render = true;

		if (any_key_down())
		{
			scale = max_scale;
		}
		else if (scale > 1)
		{
			scale /= 2;
		}
		else
		{
			usleep(10);
			enable_render = false;
		}

		if (enable_render)
		{
			gc_buf.Clear(Color::Black);
			render(gc_buf, scale);
			gc_buf.CopyTo(0, 0, gc_buf.width, gc_buf.height, gc, 0, 0);
		}

		debug_reset();
		int delta_ticks = (get_ticks() - ticks);
		delta = delta_ticks / 1000.0f;
		ticks = get_ticks();
		printf("Ticks: %i\tFPS: %i\tScale: %i\n", delta_ticks, (int)(1 / delta), scale);
		printf("Zoom: %f\n", zoom);
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
	Drawing::Init();
	MainWindow wnd;
	exit(0);
}