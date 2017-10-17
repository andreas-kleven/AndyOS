#include "font.h"
#include "string.h"
#include "disk.h"
#include "debug.h"

#define SWAPW(x) *x = (*x >> 8) | (*x << 8)
#define SWAPD(x) *x = ((*x >> 24) & 0xFF) | ((*x << 8) & 0xFF0000) | ((*x >> 8) & 0xFF00) | ((*x << 24) & 0xFF000000)
#define SWAPQ(x) *x = *x

Font::Font(char* name)
{
	header = *(TTF_HEADER*)ttf;
	SWAPD(&header.scalar_type);
	SWAPW(&header.num_tables);
	SWAPW(&header.search_range);
	SWAPW(&header.entry_selector);
	SWAPW(&header.range_shift);

	tables = new TTF_TABLE[header.num_tables];

	TTF_TABLE* table_ptr = (TTF_TABLE*)(ttf + 12);
	for (int i = 0; i < header.num_tables; i++)
	{
		tables[i] = *table_ptr++;
		SWAPD(&tables[i].checksum);
		SWAPD(&tables[i].offset);
		SWAPD(&tables[i].length);

		Debug::Print("table %ux %ux %ux", tables[i].checksum, tables[i].offset, tables[i].length);
		Debug::x = 30;
		Debug::Dump(tables[i].tag, 4, 1);
	}

	ReadHead();

	Debug::Dump(&head, sizeof(head));
}

TTF_TABLE* Font::FindTable(char* tag)
{
	TTF_TABLE* table = tables;
	for (int i = 0; i < header.num_tables; i++)
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
	head = *(TTF_HEAD_DATA*)(data + table->offset);

	SWAPD(&head.version);
	SWAPD(&head.font_revision);
	SWAPD(&head.checksum_adjust);
	SWAPD(&head.magic);

	SWAPW(&head.flags);
	SWAPW(&head.upe);

	SWAPQ(&head.created);
	SWAPQ(&head.modified);

	SWAPD(&head.minx);
	SWAPD(&head.miny);
	SWAPD(&head.maxx);
	SWAPD(&head.maxy);

	SWAPD(&head.mac_style);
	SWAPD(&head.lowrec_ppem);
	SWAPD(&head.font_dir_hint);
	SWAPD(&head.index_to_loc);
	SWAPD(&head.glyph_format);
}
