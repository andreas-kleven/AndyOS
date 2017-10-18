#include "font.h"
#include "string.h"
#include "FileSystem.h"
#include "drawing.h"
#include "hal.h"
#include "debug.h"

#define BTOLW(x) ((x >> 8) | (x << 8))
#define BTOLD(x) (((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) | ((x << 24) & 0xFF000000))

#define SWAPW(x) *x = (*x >> 8) | (*x << 8)
#define SWAPD(x) *x = ((*x >> 24) & 0xFF) | ((*x << 8) & 0xFF0000) | ((*x >> 8) & 0xFF00) | ((*x << 24) & 0xFF000000)
#define SWAPQ(x) *x = *x

#define GLYPH_ON_CURVE	0x1
#define GLYPH_X_BYTE	0x2
#define GLYPH_Y_BYTE	0x4
#define GLYPH_REPEAT	0x8
#define GLYPH_X_DELTA	0x10
#define GLYPH_Y_DELTA	0x20

Font::Font(char* name)
{
	char path[256];
	strcat(path, name);
	strcat(path, ".ttf");

	if (!FS::ReadFile(path, data))
	{
		Debug::Print("Font not found \"%s\"\n", path);
		return;
	}

	header = (TTF_HEADER*)data;
	SWAPD(&header->scalar_type);
	SWAPW(&header->num_tables);
	SWAPW(&header->search_range);
	SWAPW(&header->entry_selector);
	SWAPW(&header->range_shift);

	tables = (TTF_TABLE*)(data + sizeof(TTF_HEADER));
	TTF_TABLE* table = tables;
	for (int i = 0; i < header->num_tables; i++)
	{
		SWAPD(&table->checksum);
		SWAPD(&table->offset);
		SWAPD(&table->length);

		//Debug::Print("table %ux %ux %ux", tables[i].checksum, tables[i].offset, tables[i].length);
		//Debug::x = 40;
		//Debug::Dump(tables[i].tag, 4, 1);

		table++;
	}

	ReadHead();
	Drawing::Clear(0, Drawing::gc_direct);

	int w = 20;
	int h = 40;

	int x = w;
	int y = h;

	//Print(19, x, y, 10);
	//return;

	for (int i = 0; i < 2000; i++)
	{
		if (Print(i, x, y, 0.4))
		{
			x += w;

			if (x > Drawing::gc.width - w)
			{
				x = w;
				y += h;
			}

			//PIT::Sleep(500);
		}
	}
}

bool Font::Print(int index, int _posx, int _posy, float _scale)
{
	uint32 off = GetGlyphOffset(index);

	if (!off)
		return 0;

	TTF_GLYPH* glyph = (TTF_GLYPH*)(data + off);
	SWAPW(&glyph->num_contours);
	SWAPW(&glyph->minx);
	SWAPW(&glyph->miny);
	SWAPW(&glyph->maxx);
	SWAPW(&glyph->maxy);

	//Drawing::FillRect(glyph->minx / 10, glyph->miny / 10, glyph->maxx / 10, glyph->maxy / 10, 0xFF0000, Drawing::gc);
	//Drawing::Draw();

	if (glyph->num_contours <= 0)
	{
		//Debug::Print("CONTOURS <= 0\n");
		return 0;
	}

	int total = 0;

	uint16* contour_ptr = (uint16*)(glyph + 1);
	for (int i = 0; i < glyph->num_contours; i++)
	{
		SWAPW(contour_ptr);
		uint16 p = *contour_ptr++;
		//Debug::Print("%i\n", p);
		total = p;
	}

	total++;

	//Debug::Print("Offset: %ux\tContours: %i\tPoints: %i\tMinX: %i\tMinY: %i\tMaxX: %i\tMaxY: %i\n", off, glyph->num_contours, total, glyph->minx, glyph->miny, glyph->maxx, glyph->maxy);

	SWAPW(contour_ptr);
	uint16 inst_length = *contour_ptr++;

	//Debug::Print("Instrcutions: %i\n", inst_length);

	TTF_POINT* points = new TTF_POINT[total];

	uint8* flags = (uint8*)contour_ptr + inst_length;
	uint8* nflags = new uint8[total];

	Debug::x0 = 50;
	Debug::x = 50;
	/*int p_index;
	for (int i = 0; i < total; i++)
	{
		uint8 flag = flags[i];
		points[p_index++].flags = flag;

		if (flag & GLYPH_REPEAT)
		{
			uint8 rep_count = flags[i];
			i += rep_count;

			while (rep_count--)
			{
				points[p_index++].flags = flag;
			}
		}
	}*/

	/*Debug::y = 0;
	Debug::color = 0xFFFF0000;

	for (int i = 0; i < total; i++)
	{
		uint8 flag = flags[i];

		if(i >= 48)
		Debug::x = 20;

		if (flag & GLYPH_REPEAT)
			Debug::Print("%i_R %x\n", i, flag);
		else
			Debug::Print("%i   %x\n", i, flag);
	}*/

	Debug::bcolor = 0xFF3F3F3F;

	int fc = 0;
	Debug::y = 0;

	int i = 0;
	for (i = 0; fc < total; i++)
	{
		uint8 flag = flags[i];
		//Debug::Print("%i   %x\n", fc, flag);
		nflags[fc++] = flag;

		if (flag & GLYPH_REPEAT)
		{
			uint8 rep_count = flags[++i];

			while (rep_count--)
			{
				//Debug::Print("%i_R %x\n", fc, flag);
				//i++;
				nflags[fc++] = flag;
			}
		}
	}

	contour_ptr = (uint16*)(glyph + 1);
	uint8* bx = flags + i;

	uint16 x = 0;
	uint16 y = 0;

	Debug::color = 0xFF0000FF;
	//Debug::Dump(bx, total);

	int cc = 0;

	Debug::color = 0xFFFF0000;

	for (int i = 0; i < total; i++)
	{
		uint16 val = 0;
		uint8 flag = nflags[i];

		if (flag & GLYPH_X_BYTE)
		{
			if (flag & GLYPH_X_DELTA)
				val = *bx++;
			else
				val = -*bx++;
		}
		else if (~flag & GLYPH_X_DELTA)
		{
			val = BTOLW(*(uint16*)bx++);
		}

		//if (i > total - 48)
		//if (i < 48)
		//	Debug::Print("%i:\t%ux\t%ux\t%i\n", i, flag, val, (flag & GLYPH_ON_CURVE) == 1);

		x += (int16)val;
		points[i].x = x;
		points[i].flags = flag;
	}

	//bx += 7;
	//Debug::Dump(bx, total);

	for (int i = 0; i < total; i++)
	{
		uint16 val = 0;
		uint8 flag = nflags[i];

		if (flag & GLYPH_Y_BYTE)
		{
			if (flag & GLYPH_Y_DELTA)
				val = *bx++;
			else
				val = -*bx++;
		}
		else if (~flag & GLYPH_Y_DELTA)
		{
			val = BTOLW(*(uint16*)bx++);
		}

		//if (i < 20)
		//	Debug::Print("%i:\t%ux\t%ux\n", i, flag, val);

		y += (int16)val;
		points[i].y = y;
	}

	float scale = 64.0f / head->upem * _scale;

	//scale = 1;
	//Debug::Print("%f\n", scale);

	for (int i = 0; i < total; i++)
	{
		TTF_POINT& a = points[i];
		//Debug::Print("(%i, %i)\n", a.x, a.y);

		points[i].x += 0;
		points[i].y = -points[i].y + 0;
	}

	TTF_POINT first;
	first.x = 0;
	first.y = 0;
	//first = points[0];

	int lx = 0;
	int ly = 0;

	int fx = points[0].x;
	int fy = points[0].y;

	int c = 0;
	for (int i = 0; i < total - 1; i++)
	{
		TTF_POINT& p = points[i];
		TTF_POINT& p1 = points[i + 1];

		//Drawing::FillRect(lx * scale, ly * scale, 2, 2, 0xFFFFFF00, Drawing::gc_direct);

		Debug::color = 0xFFFFFFFF;
		//Debug::Print("%i %f %f\n", i, p.x * scale + 512, p.y * scale + 384);
		if (i == contour_ptr[c])
		{
			Drawing::DrawLine(p.x * scale + _posx, p.y * scale + _posy, fx * scale + _posx, fy * scale + _posy, 0xFFFF0000, Drawing::gc_direct);
			c++;

			fx = p1.x;
			fy = p1.y;
		}
		else
		{
			/*if (i > 1 && ~p.flags & GLYPH_ON_CURVE)
			{
				Point a(points[i - 1].x * scale + _posx, points[i - 1].y* scale + _posy);
				Point b(p.x * scale + _posx, p.y* scale + _posy);
				Point c(p1.x * scale + _posx, p1.y* scale + _posy);

				Point ps[] = { a, b, c };

				Drawing::DrawBezierQuad(ps, 3, Drawing::gc_direct);
			}*/

			Drawing::DrawLine(p1.x * scale + _posx, p1.y * scale + _posy, p.x * scale + _posx, p.y * scale + _posy, 0xFFFF0000, Drawing::gc_direct);
		}

		lx = p.x;
		ly = p.y;
	}

	Drawing::DrawLine(points[total - 1].x * scale + _posx, points[total - 1].y * scale + _posy, fx * scale + _posx, fy * scale + _posy, 0xFFFF0000, Drawing::gc_direct);
	return 1;
}

TTF_TABLE* Font::FindTable(char* tag)
{
	TTF_TABLE* table = tables;
	for (int i = 0; i < header->num_tables; i++)
	{
		if (!memcmp(table->tag, tag, 4))
			return table;

		table++;
	}

	return 0;
}

void Font::ReadHead()
{
	TTF_TABLE* table = FindTable("head");
	head = (TTF_HEAD_DATA*)(data + table->offset);

	SWAPD(&head->version);
	SWAPD(&head->font_revision);
	SWAPD(&head->checksum_adjust);
	SWAPD(&head->magic);

	SWAPW(&head->flags);
	SWAPW(&head->upem);

	SWAPQ(&head->created);
	SWAPQ(&head->modified);

	SWAPD(&head->minx);
	SWAPD(&head->miny);
	SWAPD(&head->maxx);
	SWAPD(&head->maxy);

	SWAPD(&head->mac_style);
	SWAPD(&head->lowrec_ppem);
	SWAPD(&head->font_dir_hint);
	SWAPD(&head->index_to_loc);
	SWAPD(&head->glyph_format);
}

uint32 Font::GetGlyphOffset(int index)
{
	TTF_TABLE* loca = FindTable("loca");
	TTF_TABLE* glyf = FindTable("glyf");

	if (!loca)
		return 0;

	//Debug::Dump(data + loca->offset, 16);

	if (head->index_to_loc || 1)
	{
		uint32* offsets = (uint32*)(data + loca->offset);

		if (index > 0)
			if (offsets[index] == offsets[index - 1])
				return 0;

		return glyf->offset + BTOLD(offsets[index]);
	}
	else
	{
		uint16* offsets = (uint16*)(data + loca->offset);
		return BTOLW(offsets[index]) * 2;
	}
}
