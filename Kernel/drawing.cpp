#include "drawing.h"
#include "string.h"
#include "math.h"

//int Drawing::width;
//int Drawing::height;
//int Drawing::memsize;

uint32* screenBuffer;
GC Drawing::gc;
GC Drawing::gc_direct;

STATUS Drawing::Init(int width, int height, uint32* framebuffer)
{
	//Drawing::width = width;
	//Drawing::height = height;
	//Drawing::memsize = width * height * 4;

	screenBuffer = framebuffer;

	gc.width = width;
	gc.height = height;
	gc.framebuffer = new uint32[gc.memsize()];

	gc_direct.width = width;
	gc_direct.height = height;
	gc_direct.framebuffer = framebuffer;
	return STATUS_SUCCESS;
}

void Drawing::Draw(GC gc)
{
	memcpy_fast_128(screenBuffer, gc.framebuffer, gc.memsize());
	//memcpy((uint32*)gc.framebuffer, gc.framebuffer, gc.memsize());
}

void Drawing::Clear(uint32 c, GC gc)
{
	int pixels = gc.pixels();
	uint32* buffer = gc.framebuffer;

	_asm
	{
		mov edi, buffer
		mov eax, c
		mov ecx, pixels

		loop_set :
		mov[edi], eax

			add edi, 4
			dec ecx
			jnz loop_set
	}
}

void Drawing::BitBlt(GC src, int x0, int y0, int w0, int h0, GC dst, int x1, int y1, bool alpha)
{
	if (x1 < 0)
	{
		x0 -= x1;
		w0 += x1;
		x1 = 0;
	}

	if (y1 < 0)
	{
		y0 -= y1;
		h0 += y1;
		y1 = 0;
	}

	x0 = clamp(x0, 0, src.width);
	y0 = clamp(y0, 0, src.height);

	x1 = clamp(x1, 0, dst.width);
	y1 = clamp(y1, 0, dst.height);

	w0 = clamp(w0, 0, min(src.width - x0, dst.width - x1));
	h0 = clamp(h0, 0, min(src.height - y0, dst.height - y1));

	uint32* srcPtr = src.framebuffer + src.width * y0 + x0;
	uint32* dstPtr = dst.framebuffer + dst.width * y1 + x1;

	int d0 = src.width - w0;
	int d1 = dst.width - w0;

	if (alpha)
	{
		for (int _y = 0; _y < h0; _y++)
		{
			for (int _x = 0; _x < w0; _x++)
			{
				uint32 s = *srcPtr;
				uint32 d = *dstPtr;

				uint32 a = 0xFF & (s >> 24);

				uint32 rs = 0xFF & (s >> 16);
				uint32 gs = 0xFF & (s >> 8);
				uint32 bs = 0xFF & s;

				uint32 rd = 0xFF & (d >> 16);
				uint32 gd = 0xFF & (d >> 8);
				uint32 bd = 0xFF & d;

				uint8 r = (rs * a + rd * (255 - a)) / 255;
				uint8 g = (gs * a + gd * (255 - a)) / 255;
				uint8 b = (bs * a + bd * (255 - a)) / 255;

				*dstPtr++ = (r << 16) | (g << 8) | b;
				srcPtr++;
			}

			srcPtr += d0;
			dstPtr += d1;
		}
	}
	else
	{
		for (int _y = 0; _y < h0; _y++)
		{
			for (int _x = 0; _x < w0; _x++)
			{
				*dstPtr++ = *srcPtr++;
			}

			srcPtr += d0;
			dstPtr += d1;
		}
	}
}

void Drawing::SetPixel(int x, int y, uint32 c, GC gc)
{
	int width = gc.width;
	int height = gc.height;
	uint32* buffer = gc.framebuffer;

	if (x >= width || x < 0)
		return;

	if (y >= height || y < 0)
		return;

	uint32* a = buffer + y * width + x;
	*a = c;
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, uint32 c, GC gc)
{
	const int width = gc.width;
	const int height = gc.height;

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
					SetPixel(px, py, c, gc);

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
					SetPixel(px, py, c, gc);

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

void Drawing::DrawBezierQuad(Point* points, int count, GC gc)
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

			Drawing::DrawLine(lastx, lasty, (int)px10, (int)py10, 0xFF0000, gc);
			lastx = (int)px10;
			lasty = (int)py10;
		}
	}
}

void Drawing::DrawBezierCube(Point* points, int count, GC gc)
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

			Drawing::DrawLine(lastx, lasty, (int)px20, (int)py20, 0xFF0000, gc);
			lastx = (int)px20;
			lasty = (int)py20;
		}
	}
}

void Drawing::DrawRect(int x, int y, int w, int h, int bw, uint32 c, GC gc)
{
	FillRect(x, y, x + w, y + bw, c, gc);			//Top
	FillRect(x, y + h - bw, x + w, y + h, c, gc);	//Botom
	FillRect(x, y, x + bw, y + h, c, gc);			//Left
	FillRect(x + w - bw, y, x + w, y + h, c, gc);	//Right
}

void Drawing::FillRect(int x, int y, int w, int h, uint32 c, GC gc)
{
	int width = gc.width;
	int height = gc.height;

	x = clamp(x, 0, width);
	y = clamp(y, 0, height);

	w = clamp(w, 0, width - x);
	h = clamp(h, 0, height - y);

	int delta = width - w;
	uint32* buf = gc.framebuffer + y * width + x;

	for (int _y = 0; _y < h; _y++)
	{
		for (int _x = 0; _x < w; _x++)
		{
			*buf++ = c;
		}
		buf += delta;
	}
}

void Drawing::DrawImage(int x, int y, int w, int h, BMP* bmp, GC gc)
{
	int ox = 0;
	int oy = 0;

	if (x < 0)
	{
		w += x;
		ox = -x;
		x = 0;
	}

	if (y < 0)
	{
		h += y;
		oy = -y;
		y = 0;
	}

	x = clamp(x, 0, gc.width);
	y = clamp(y, 0, gc.height);

	w = clamp(w, 0, min(gc.width - x, bmp->width));
	h = clamp(h, 0, min(gc.height - y, bmp->height));

	int delta = gc.width - w;
	uint32* dst = gc.framebuffer + (y + oy) * gc.width + (x + ox);
	uint32* src = bmp->pixels;

	for (int _y = 0; _y < h; _y++)
	{
		for (int _x = 0; _x < w; _x++)
		{
			*dst++ = *src++;
		}

		dst += delta;
	}

	//memcpy(gc.framebuffer, bmp->pixels, bmp->pixel_count);
}


void Drawing::DrawText(int x, int y, char* c, uint32 col, GC gc)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col, gc);
			}
		}
	}
}

void Drawing::DrawText(int x, int y, char* c, uint32 col, uint32 bg, GC gc)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col, gc);
				else
					SetPixel(x + index * 8 + (8 - j), y + i, bg, gc);
			}
		}
	}
}