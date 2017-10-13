#include "drawing.h"
#include "string.h"
#include "math.h"

int Drawing::width;
int Drawing::height;
int Drawing::memsize;

GC Drawing::screen;
GC Drawing::buffer;

STATUS Drawing::Init(int width, int height, uint32* framebuffer)
{
	Drawing::width = width;
	Drawing::height = height;
	Drawing::memsize = width * height * 4;

	screen.bounds = Rect(0, 0, width, height);
	screen.framebuffer = framebuffer;

	buffer.bounds = screen.bounds;
	buffer.framebuffer = new uint32[width * height];
	return STATUS_SUCCESS;
}

void Drawing::Draw(GC context)
{
	memcpy_fast_128(screen.framebuffer, context.framebuffer, memsize);
	//memcpy((uint32*)screen.framebuffer, context.framebuffer, size);
}

void Drawing::Clear(uint32 c, GC context)
{
	_asm
	{
		mov edi, buffer
		mov eax, c
		mov ecx, memsize

		loop_set :
		mov[edi], eax

			add edi, 4
			dec ecx
			jnz loop_set
	}
}

void Drawing::SetPixel(int x, int y, uint32 c, GC context)
{
	if (x >= width || x < 0)
		return;

	if (y >= height || y < 0)
		return;

	uint32* a = buffer.framebuffer + y * width + x;
	*a = c;
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, uint32 c, GC context)
{
	int deltax = x1 - x0;
	int deltay = y1 - y0;

	int y = 0;
	int x = 0;

	int sdx = (deltax < 0) ? -1 : 1;
	int sdy = (deltay < 0) ? -1 : 1;

	deltax = sdx * deltax + 1;
	deltay = sdy * deltay + 1;

	int px = x0;
	int py = y0;

	if (deltax >= deltay)
	{
		for (x = 0; x < deltax; x++)
		{
			if (px < width && px > 0)
				if (py < height && py > 0)
					SetPixel(px, py, c);

			y += deltay;

			if (y >= deltax)
			{
				y -= deltax;
				py += sdy;
			}

			px += sdx;
		}
	}
	else
	{
		for (y = 0; y < deltay; y++)
		{
			if (px < width && px > 0)
				if (py < height && py > 0)
					SetPixel(px, py, c);

			x += deltax;

			if (x >= deltay)
			{
				x -= deltay;
				px += sdx;
			}

			py += sdy;
		}
	}
}

void Drawing::DrawBezierQuad(Point* points, int count, GC context)
{
	const int steps = 128;

	for (int i = 0; i < count - 2; i += 2)
	{
		Point& a = points[i];
		Point& b = points[i + 1];
		Point& c = points[i + 2];

		DrawLine(a.x, a.y, b.x, b.y, 0xFF);
		DrawLine(b.x, b.y, c.x, c.y, 0xFF);

		int lastx = a.x;
		int lasty = a.y;

		for (int iter = 1; iter <= steps; iter++)
		{
			float alpha = iter / (float)steps;

			float px00 = a.x + (b.x - a.x) * alpha;
			float py00 = a.y + (b.y - a.y) * alpha;
			float px01 = b.x + (c.x - b.x) * alpha;
			float py01 = b.y + (c.y - b.y) * alpha;

			float px10 = px00 + (px01 - px00) * alpha;
			float py10 = py00 + (py01 - py00) * alpha;

			Drawing::DrawLine(lastx, lasty, (int)px10, (int)py10, 0xFF0000);
			lastx = (int)px10;
			lasty = (int)py10;
		}
	}
}

void Drawing::DrawBezierCube(Point* points, int count, GC context)
{
	const int steps = 128;

	for (int i = 0; i < count - 3; i += 3)
	{
		Point& a = points[i];
		Point& b = points[i + 1];
		Point& c = points[i + 2];
		Point& d = points[i + 3];

		DrawLine(a.x, a.y, b.x, b.y, 0xFF);
		DrawLine(b.x, b.y, c.x, c.y, 0xFF);
		DrawLine(c.x, c.y, d.x, d.y, 0xFF);

		int lastx = a.x;
		int lasty = a.y;

		for (int iter = 1; iter <= steps; iter++)
		{
			float alpha = iter / (float)steps;

			float px00 = a.x + (b.x - a.x) * alpha;
			float py00 = a.y + (b.y - a.y) * alpha;
			float px01 = b.x + (c.x - b.x) * alpha;
			float py01 = b.y + (c.y - b.y) * alpha;
			float px02 = c.x + (d.x - c.x) * alpha;
			float py02 = c.y + (d.y - c.y) * alpha;

			float px10 = px00 + (px01 - px00) * alpha;
			float py10 = py00 + (py01 - py00) * alpha;
			float px11 = px01 + (px02 - px01) * alpha;
			float py11 = py01 + (py02 - py01) * alpha;

			float px20 = px10 + (px11 - px10) * alpha;
			float py20 = py10 + (py11 - py10) * alpha;

			Drawing::DrawLine(lastx, lasty, (int)px20, (int)py20, 0xFF0000);
			lastx = (int)px20;
			lasty = (int)py20;
		}
	}
}

void Drawing::DrawRect(int x, int y, int w, int h, uint32 c, GC context)
{
	x = clamp(x, 0, width);
	y = clamp(y, 0, height);

	w = clamp(w, 0, width - x);
	h = clamp(h, 0, height - y);

	uint32* ptr = buffer.framebuffer + y * width + x;
	int delta = width - w;

	for (int _y = 0; _y < h; _y++)
	{
		for (int _x = 0; _x < w; _x++)
		{
			*ptr++ = c;
		}
		ptr += delta;
	}
}

void Drawing::DrawText(int x, int y, char* c, uint32 col, GC context)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col);
			}
		}
	}
}

void Drawing::DrawText(int x, int y, char* c, uint32 col, uint32 bg, GC context)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col);
				else
					SetPixel(x + index * 8 + (8 - j), y + i, bg);
			}
		}
	}
}