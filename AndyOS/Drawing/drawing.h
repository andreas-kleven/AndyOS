#pragma once
#include "definitions.h"
#include "math.h"
#include "font.h"
#include "color.h"
#include "bmp.h"

struct Point
{
	int x;
	int y;

	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

struct Rect
{
	int x;
	int y;
	int width;
	int height;

	Rect()
	{
		this->x = 0;
		this->y = 0;
		this->width = 0;
		this->height = 0;
	}

	Rect(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		this->width = w;
		this->height = h;
	}

	bool Contains(int x, int y)
	{
		return ((x >= this->x)
			&& x <= (this->x + this->width)
			&& (y >= this->y)
			&& (y <= this->y + this->height));
	}

	int Right()
	{
		return this->x + this->width;
	}

	int Top()
	{
		return this->y + this->height;
	}
};

struct GC
{
	int x;
	int y;
	int width;
	int height;

	int stride;
	uint32* framebuffer;

	GC()
	{ }

	GC(int width, int height, uint32* framebuffer)
	{
		this->x = 0;
		this->y = 0;
		this->width = width;
		this->height = height;
		this->stride = width;
		this->framebuffer = framebuffer;
	}

	GC(int width, int height) : GC(width, height, new uint32[width * height])
	{ }

	GC(GC& gc, int x, int y, int width, int height)
	{
		this->x = x + gc.x;
		this->y = y + gc.y;
		this->width = clamp(width, 0, gc.width - x);
		this->height = clamp(height, 0, gc.height - y);

		this->stride = gc.stride;
		this->framebuffer = gc.framebuffer;
	}

	GC(GC& gc, Rect bounds) : GC(gc, bounds.x, bounds.y, bounds.width, bounds.height)
	{ }

	void Resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->stride = width;
	}

	inline int memsize()
	{
		return width * height * 4;
	}

	inline int pixels()
	{
		return width * height;
	}
};

class Drawing
{
public:
	static void Init();

	static void Draw(GC& gc);
	static void Clear(Color& col, GC& gc);

	static void BitBlt(GC& src, int x0, int y0, int w0, int h0, GC& dst, int x1, int y1, bool alpha = 0);

	static void SetPixel(int x, int y, Color& col, GC& gc);

	static void DrawLine(int x0, int y0, int x1, int y1, Color& col, GC& gc);
	static void DrawBezierQuad(Point* points, int count, Color& col, GC& gc);
	static void DrawBezierCube(Point* points, int count, Color& col, GC& gc);

	static void DrawRect(Rect& bounds, int width, Color& col, GC& gc);
	static void DrawRect(int x, int y, int w, int h, int width, Color& col, GC& gc);
	static void FillRect(Rect& bounds, Color& col, GC& gc);
	static void FillRect(int x, int y, int w, int h, Color& col, GC& gc);

	static void DrawImage(Rect& bounds, BMP* bmp, GC& gc);
	static void DrawImage(int x, int y, int w, int h, BMP* bmp, GC& gc);

	static void DrawText(int x, int y, char* c, Color& fg, GC& gc);
	static void DrawText(int x, int y, char* c, Color& fg, Color& bg, GC& gc);

private:
	static inline uint32 BlendAlpha(uint32 src, uint32 dst);
};