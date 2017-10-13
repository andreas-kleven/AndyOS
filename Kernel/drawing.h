#pragma once
#include "definitions.h"
#include "font.h"

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
};

class GC
{
public:
	Rect bounds;
	uint32* framebuffer;
};

static class Drawing
{
public:
	static int width;
	static int height;
	static int memsize;

	static GC screen;
	static GC buffer;

	static STATUS Init(int width, int height, uint32* framebuffer);

	static void Draw(GC context = buffer);
	static void Clear(uint32 c, GC context = buffer);
	static void SetPixel(int x, int y, uint32 c, GC context = buffer);

	static void DrawLine(int x0, int y0, int x1, int y1, uint32 c, GC context = buffer);
	static void DrawBezierQuad(Point* points, int count, GC context = buffer);
	static void DrawBezierCube(Point* points, int count, GC context = buffer);

	static void DrawRect(int x, int y, int w, int h, uint32 c, GC context = buffer);

	static void DrawText(int x, int y, char* c, uint32 col, GC context = buffer);
	static void DrawText(int x, int y, char* c, uint32 col, uint32 bg, GC context = buffer);
};