#pragma once
#include "definitions.h"
#include "math.h"
#include "font.h"
#include "bmp.h"

#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_BLACK		0xFF000000
#define COLOR_RED		0xFFFF0000
#define COLOR_GREEN		0xFF00FF00
#define COLOR_BLUE		0xFF0000FF
#define COLOR_CYAN		0xFF00FFFF
#define COLOR_MAGENTA	0xFFFF00FF
#define COLOR_YELLOW	0xFFFFFF00

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

	GC(GC gc, int x, int y, int width, int height)
	{
		this->x = x + gc.x;
		this->y = y + gc.y;
		this->width = clamp(width, 0, gc.width - x);
		this->height = clamp(height, 0, gc.height - y);

		this->stride = gc.stride;
		this->framebuffer = gc.framebuffer;
	}

	GC(GC gc, Rect bounds) : GC(gc, bounds.x, bounds.y, bounds.width, bounds.height)
	{ }

	inline int memsize()
	{
		return width * height * 4;
	}

	inline int pixels()
	{
		return width * height;
	}

	static GC CreateGraphics(int width, int height)
	{
		GC gc;
		gc.x = 0;
		gc.y = 0;
		gc.width = width;
		gc.height = height;
		gc.stride = width;
		gc.framebuffer = new uint32[width * height];
		return gc;
	}
};

static class Drawing
{
public:
	//static int width;
	//static int height;
	//static int memsize;

	static GC gc;
	static GC gc_direct;

	static STATUS Init(int width, int height, uint32* framebuffer);

private:
	static inline uint32 BlendAlpha(uint32 src, uint32 dst);

public:
	static void Draw(GC gc = gc);
	static void Clear(uint32 c, GC gc = gc);

	static void BitBlt(GC src, int x0, int y0, int w0, int h0, GC dst, int x1, int y1, bool alpha = 0);

	static void SetPixel(int x, int y, uint32 c, GC gc = gc);

	static void DrawLine(int x0, int y0, int x1, int y1, uint32 c, GC gc = gc);
	static void DrawBezierQuad(Point* points, int count, GC gc = gc);
	static void DrawBezierCube(Point* points, int count, GC gc = gc);

	static void DrawRect(int x, int y, int w, int h, int width, uint32 c, GC gc = gc);
	static void FillRect(int x, int y, int w, int h, uint32 c, GC gc = gc);

	static void DrawImage(int x, int y, int w, int h, BMP* bmp, GC gc = gc);

	static void DrawText(int x, int y, char* c, uint32 col, GC gc = gc);
	static void DrawText(int x, int y, char* c, uint32 col, uint32 bg, GC gc = gc);
};