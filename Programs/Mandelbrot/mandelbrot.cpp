#include <AndyOS.h>
#include <sys/drawing.h>
#include "GUI.h"
#include "math.h"

using namespace gui;

const int iter_max = 256;
int histogram[iter_max + 1];

double zoom = 4;
double ofx = 0;
double ofy = 0;
double rot = 0;

int get_iteration(double x, double y, double* z2)
{
	double Zx = 0;
	double Zy = 0;
	double Zx2 = 0;
	double Zy2 = 0;

	//Julia set
	//Zx = x;
	//Zy = y;

	//x = 0.7885 * cos(rot);
	//y = 0.7885 * sin(rot);
	
	//Optimizations
	double q = pow(x - 0.25, 2) + y*y;
	if ((q * (q + (x - 0.25)) < y*y / 4) 	//Inside cardioid
		|| ((x+1) * (x+1) + y*y) * 16 < 1)	//Inside period-2 bulb
	{
		*z2 = 0;
		return iter_max;
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

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

uint32 get_color(double hue)
{
	hsv c;
	c.h = hue * 360;
	c.s = 1;
	c.v = 1;

	rgb rc = hsv2rgb(c);

	return Color(rc.r, rc.g, rc.b).ToInt();
}

void run(GC& gc)
{
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

		//Reset histogram
		for (int i = 0; i <= iter_max; i++)
			histogram[i] = 0;

		for (int _y = 0; _y < gc_buf.height; _y++)
		{
			for (int _x = 0; _x < gc_buf.width; _x++)
			{
				double x = (_x - gc_buf.width / 2) * asd1 + ofx;
				double y = (_y - gc_buf.height / 2) * asd1 + ofy;

				double z2;
				int iter = get_iteration(x, y, &z2);

				histogram[iter]++;
			}
		}

		for (int _y = 0; _y < gc_buf.height; _y++)
		{
			for (int _x = 0; _x < gc_buf.width; _x++)
			{
				double x = (_x - gc_buf.width / 2) * asd1 + ofx;
				double y = (_y - gc_buf.height / 2) * asd1 + ofy;
				
				double z2;
				int iter = get_iteration(x, y, &z2);

				int total = gc_buf.width * gc_buf.height;

				double hue = 0;
				for (int i = 0; i <= iter; i++)
  					hue += histogram[i] / (double)total;

				*buf++ = get_color(hue);

				/*if (iter == iter_max)
				{
					*buf++ = 0;
				}
				else
				{
					double zn = sqrt(z2);
					uint32 color = clamp((int)((iter - log2(log(zn) / log(1 << 16))) * 5), 0, 0xFF);
					*buf++ = color;
				}*/
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