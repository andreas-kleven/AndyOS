#include "drawing.h"
#include "string.h"
#include "math.h"
#include "../API/api.h"
#include "../Memory/memory.h"

void Drawing::Init()
{
	//Hack
	Color::Red = Color(1, 0, 0);
	Color::Green = Color(0, 1, 0);
	Color::Blue = Color(0, 0, 1);
	Color::Cyan = Color(0, 1, 1);
	Color::Magenta = Color(1, 0, 1);
	Color::Yellow = Color(1, 1, 0);
	Color::Black = Color(0, 0, 0);
	Color::White = Color(1, 1, 1);
	Color::Gray = Color(0.5, 0.5, 0.5);
	Color::LightGray = Color(0.8, 0.8, 0.8);
	Color::DarkGray = Color(0.2, 0.2, 0.2);
}

void Drawing::Draw(GC& gc)
{
	draw(gc.framebuffer);
}

void Drawing::Clear(Color& col, GC& gc)
{
	FillRect(0, 0, gc.width, gc.height, col, gc);
}

void Drawing::BitBlt(GC& src, int x0, int y0, int w0, int h0, GC& dst, int x1, int y1, bool alpha)
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

	uint32* srcPtr = src.framebuffer + src.stride * (y0 + src.y) + (x0 + src.x);
	uint32* dstPtr = dst.framebuffer + dst.stride * (y1 + dst.y) + (x1 + dst.x);

	int d0 = src.stride - w0;
	int d1 = dst.stride - w0;

	if (alpha)
	{
		for (int _y = 0; _y < h0; _y++)
		{
			for (int _x = 0; _x < w0; _x++)
			{
				*dstPtr++ = BlendAlpha(*srcPtr++, *dstPtr);
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

void Drawing::SetPixel(int x, int y, Color& col, GC& gc)
{
	uint32* buffer = gc.framebuffer;

	if (x >= gc.width || x < 0)
		return;

	if (y >= gc.height || y < 0)
		return;

	uint32* a = buffer + (y + gc.y) * gc.stride + (x + gc.x);
	*a = BlendAlpha(col.ToInt(), *a);
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, Color& col, GC& gc)
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
			if (px < gc.width && px > 0)
				if (py < gc.height && py > 0)
					SetPixel(px, py, col, gc);

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
			if (px < gc.width && px > 0)
				if (py < gc.height && py > 0)
					SetPixel(px, py, col, gc);

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

void Drawing::DrawBezierQuad(Point* points, int count, Color& color, GC& gc)
{
	const int steps = 128;

	for (int i = 0; i < count - 2; i += 2)
	{
		Point& a = points[i];
		Point& b = points[i + 1];
		Point& c = points[i + 2];

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

			Drawing::DrawLine(lastx, lasty, (int)px10, (int)py10, color, gc);
			lastx = (int)px10;
			lasty = (int)py10;
		}
	}
}

void Drawing::DrawBezierCube(Point* points, int count, Color& color, GC& gc)
{
	const int steps = 128;

	for (int i = 0; i < count - 3; i += 3)
	{
		Point& a = points[i];
		Point& b = points[i + 1];
		Point& c = points[i + 2];
		Point& d = points[i + 3];

		DrawLine(a.x, a.y, b.x, b.y, color, gc);
		DrawLine(b.x, b.y, c.x, c.y, color, gc);
		DrawLine(c.x, c.y, d.x, d.y, color, gc);

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

			Drawing::DrawLine(lastx, lasty, (int)px20, (int)py20, color, gc);
			lastx = (int)px20;
			lasty = (int)py20;
		}
	}
}

void Drawing::DrawRect(int x, int y, int w, int h, int bw, Color& col, GC& gc)
{
	FillRect(x, y, w, bw, col, gc);			//Top
	FillRect(x, y + h - bw, w, bw, col, gc);	//Botom
	FillRect(x, y, bw, h, col, gc);			//Left
	FillRect(x + w - bw, y, bw, h, col, gc);	//Right
}

void Drawing::FillRect(int x, int y, int w, int h, Color& col, GC& gc)
{
	x = clamp(x, 0, gc.width);
	y = clamp(y, 0, gc.height);

	w = clamp(w, 0, gc.width - x);
	h = clamp(h, 0, gc.height - y);

	int delta = gc.stride - w;
	uint32* buf = gc.framebuffer + (y + gc.y) * gc.stride + (x + gc.x);

	uint32 _col = col.ToInt();

	for (int _y = 0; _y < h; _y++)
	{
		for (int _x = 0; _x < w; _x++)
		{
			//*buf++ = BlendAlpha(c, *buf);
			*buf++ = _col;
		}
		buf += delta;
	}
}

void Drawing::DrawImage(int x, int y, int w, int h, BMP* bmp, GC& gc)
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

	int delta = gc.stride - w;
	uint32* dst = gc.framebuffer + (y + oy) * gc.stride + (x + ox);
	uint32* src = bmp->pixels;

	for (int _y = 0; _y < h; _y++)
	{
		for (int _x = 0; _x < w; _x++)
		{
			*dst++ = *src++;
		}

		dst += delta;
	}
}


void Drawing::DrawText(int x, int y, char* c, Color& fg, GC& gc)
{
	for (int index = 0; index < strlen(c); index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, fg, gc);
			}
		}
	}
}

void Drawing::DrawText(int x, int y, char* c, Color& fg, Color& bg, GC& gc)
{
	for (int index = 0; index < strlen(c); index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, fg, gc);
				else
					SetPixel(x + index * 8 + (8 - j), y + i, bg, gc);
			}
		}
	}
}


inline uint32 Drawing::BlendAlpha(uint32 src, uint32 dst)
{
	uint32 a = 0xFF & (src >> 24);

	if (a == 0)
		return dst;

	if (a == 0xFF)
		return src;

	uint32 rs = 0xFF & (src >> 16);
	uint32 gs = 0xFF & (src >> 8);
	uint32 bs = 0xFF & src;

	uint32 rd = 0xFF & (dst >> 16);
	uint32 gd = 0xFF & (dst >> 8);
	uint32 bd = 0xFF & dst;

	uint8 r = (rs * a + rd * (255 - a)) / 255;
	uint8 g = (gs * a + gd * (255 - a)) / 255;
	uint8 b = (bs * a + bd * (255 - a)) / 255;

	return (0xFF << 24) | (r << 16) | (g << 8) | b;
}