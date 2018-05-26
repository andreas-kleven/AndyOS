#include "A3D.h"
#include "System.h"
#include "Vertex.h"

struct RAW_VERTEX
{
	float pos_x;
	float pos_y;
	float pos_z;

	float norm_x;
	float norm_y;
	float norm_z;

	float tex_u;
	float tex_v;
};

A3D::A3D(char* buffer)
{
	vertex_count = *(uint32*)buffer;
	vertex_count;

	RAW_VERTEX* vertex = (RAW_VERTEX*)((char*)buffer + 4);
	vertices = new Vertex[vertex_count];

	for (int i = 0; i < vertex_count; i++)
	{
		RAW_VERTEX raw = *vertex++;

		//gl::ColRGB col(i % 3 == 0, i % 3 == 1, i % 3 == 2);
		ColRGB col(1, 1, 1);
		Vector4 norm = Vector4(raw.norm_x, raw.norm_y, raw.norm_z, 1);
		Vertex vert(raw.pos_x, raw.pos_y, raw.pos_z, col, norm);
		vert.tex_u = raw.tex_u;
		vert.tex_v = raw.tex_v;
		vertices[i] = vert;

		//Debug::Print("%f, %f, %f\n", raw.pos_x, raw.pos_y, raw.pos_z);
	}

	//Debug::Print("%i", vertex_count);
	//while (1);
}