#include "Mandelbrot.h"
#include "debug.h"
#include "vbe.h"
#include "math.h"
#include "hal.h"
#include "keyboard.h"
#include "mouse.h"

struct Complex
{
	float real;
	float img;

	Complex()
	{
		real = 0;
		img = 0;
	}

	Complex(float real, float img)
	{
		this->real = real;
		this->img = img;
	}

	Complex& operator+=(const Complex& z)
	{
		real += z.real;
		img += z.img;
		return *this;
	}

	Complex& operator*=(const Complex& z)
	{
		real = real * z.real - img * z.img;
		img = real * z.img + img * z.real;
		return *this;
	}

	void Square()
	{
		float _real = real;

		real = real * real - img * img;
		img = 2 * _real * img;
	}

	float Magnitude()
	{
		return sqrt(real * real + img * img);
	}
};

int count = 1024;

Complex tz;

inline int test(const Complex& c)
{
	int i;
	tz = Complex(0, 0);

	for (i = 0; i < count; i++)
	{
		tz.Square();
		tz += c;

		//Debug::Print("%f, %f, %f\t", z.real, z.img, z.Magnitude());

		if (tz.Magnitude() >= (1 << 16))
		{
			//Debug::Print("\nret %f, %i", z.Magnitude(), i);

			return i + 1 - log(log(tz.Magnitude())) / log(2);
			//return i;
		}
	}

	//Debug::Print("\nret1 %i\n", i);
	return 0;
}

#pragma optimize("", off)
void stuff()
{
	int ti = PIT::ticks;
	for (int i = 0; i < 10000000; i++)
	{
		//log(M_PI);
		float f = 0.00000000000000000000009048903434712123154234f;
		float b = 0.00000000000000000000009048903434712123154321f;
		float r = f * b;
	}
	Debug::Print("%i\n", PIT::ticks - ti);
	while (1);
}
#pragma optimize("", on)

void Mandelbrot::Create(int width, int height)
{
	//stuff();

	Debug::Print("Start\n");

	width = 768;
	height = width;
	//Complex z(1, 1);
	//z.Square();

	//
	//z.real = -1;
	//z.img = 0;
	//int gg = test(z);
	//Debug::Print("%f, %f", z.real, z.img);
	//while (1);

	int iter_max = 256;

	Mouse::scroll_y = 4;

	double zoom = 4;
	double ofx = 0;
	double ofy = 0;

	int ticks = PIT::ticks;
	double delta = 1;

	while (1)
	{
		double asd0 = zoom;
		double asd1 = asd0 / width;
		//double ofx = (Mouse::x - width / 2) / (double)width;
		//double ofy = (Mouse::y - height / 2) / (double)width;

		if (Keyboard::GetKeyDown(KEY_LCTRL)) zoom += 1.0f * delta * asd0;
		if (Keyboard::GetKeyDown(KEY_SPACE)) zoom -= 1.0f * delta * asd0;

		if (Keyboard::GetKeyDown(KEY_A)) ofx -= 1.0f * delta * asd0;
		if (Keyboard::GetKeyDown(KEY_D)) ofx += 1.0f * delta * asd0;
		if (Keyboard::GetKeyDown(KEY_W)) ofy -= 1.0f * delta * asd0;
		if (Keyboard::GetKeyDown(KEY_S)) ofy += 1.0f * delta * asd0;

		VBE::Clear(0);
		Debug::x = 0;
		Debug::y = 0;

		int index = 0;
		uint32* buf = VBE::buffer;

		double cr;
		double ci;
		double zr = 0;
		double zi = 0;
		double zrsqr = 0;
		double zisqr = 0;

		for (int _y = 0; _y < height; _y++)
		{
			for (int _x = 0; _x < width; _x++)
			{
				cr = (_x - width / 2) * asd1 + ofx;
				ci = (_y - height / 2) * asd1 + ofy;
				zr = 0;
				zi = 0;
				zrsqr = 0;
				zisqr = 0;

				int iter;
				for (iter = 0; iter < iter_max && ((zrsqr + zisqr) < (1 << 16)); iter++)
				{
					zi = zr * zi;
					zi += zi;
					zi += ci;

					zr = zrsqr - zisqr + cr;

					zrsqr = zr * zr;
					zisqr = zi * zi;

					//zi = zr * zi * 2.0 + ci;
					//zr = zrsqr - zisqr + cr;
					//
					//zrsqr = zr * zr;
					//zisqr = zi * zi;
				}

				if (iter == iter_max)
				{
					*buf++ = 0;
				}
				else
				{
					//*buf++ = iter;
					//continue;

					double zn = sqrt(zrsqr + zisqr);
					double hue = iter + 1.0 - log2(log(zn));  // 2 is escape radius
					//int gg = clamp((int)(logn(hue, 200) * 0xFF), 0, 0xFF);

					uint32 color = clamp((int)(logn(hue, 200) * 0xFF), 0, 0xFF);
					//int g = clamp((int)(logn(logn(hue, 5), 5) * 0xFF), 0, 0xFF);
					//int g2 = clamp((int)(logn(logn(hue, 6), 6) * 0xFF), 0, 0xFF);
					//color += g2 << 16;
					//color += g << 8;

					*buf++ = color;
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

			index += 1024 - width;
			buf += 1024 - width;
		}

		VBE::Draw();

		int delta_ticks = (PIT::ticks - ticks);
		delta = delta_ticks / 1000.0f;
		ticks = PIT::ticks;
		Debug::Print("Ticks: %i\tFPS: %i\n", delta_ticks, (int)(1 / delta));
		Debug::Print("Zoom: %f\n", zoom);
	}
	while (1);
}
