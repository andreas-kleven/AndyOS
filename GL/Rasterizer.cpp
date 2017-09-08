#include "Rasterizer.h"
#include "System.h"
#include "GL.h"
#include "math.h"

namespace gl
{
	float EdgeFunction(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		return (c.mul_y - a.mul_y) * (b.mul_x - a.mul_x) - (c.mul_x - a.mul_x) * (b.mul_y - a.mul_y);
	}

	float* Rasterizer::depth_buffer;

	STATUS Rasterizer::Init()
	{
		uint32 pixels = GL::m_height * GL::m_width;
		uint32 blocks = pixels * sizeof(uint32) / BLOCK_SIZE;

		depth_buffer = (float*)Memory::AllocBlocks(blocks);
		return STATUS_SUCCESS;
	}

	void Rasterizer::DrawTriangle(Vertex* buffer, BMP* texture)
	{
		uint32 stride = GL::m_width * 4;

		Vertex& v0 = buffer[0];
		Vertex& v1 = buffer[1];
		Vertex& v2 = buffer[2];

		v0.mul_w = 1 / v0.mul_w;
		v1.mul_w = 1 / v1.mul_w;
		v2.mul_w = 1 / v2.mul_w;

		v0.mul_x = v0.mul_x * GL::m_width * v0.mul_w + GL::m_width * 0.5;
		v1.mul_x = v1.mul_x * GL::m_width * v1.mul_w + GL::m_width * 0.5;
		v2.mul_x = v2.mul_x * GL::m_width * v2.mul_w + GL::m_width * 0.5;

		v0.mul_y = v0.mul_y * GL::m_height * v0.mul_w + GL::m_height * 0.5;
		v1.mul_y = v1.mul_y * GL::m_height * v1.mul_w + GL::m_height * 0.5;
		v2.mul_y = v2.mul_y * GL::m_height * v2.mul_w + GL::m_height * 0.5;

		//VBE::DrawLine((int)v0.mul_x, (int)v0.mul_y, (int)v1.mul_x, (int)v1.mul_y, 0xFF);
		//VBE::DrawLine((int)v1.mul_x, (int)v1.mul_y, (int)v2.mul_x, (int)v2.mul_y, 0xFF00);
		//VBE::DrawLine((int)v2.mul_x, (int)v2.mul_y, (int)v0.mul_x, (int)v0.mul_y, 0xFF0000);
		//return;

		int minx = min(floor(v0.mul_x), floor(v1.mul_x), floor(v2.mul_x));
		int maxx = max(floor(v0.mul_x), floor(v1.mul_x), floor(v2.mul_x));
		int miny = min(floor(v0.mul_y), floor(v1.mul_y), floor(v2.mul_y));
		int maxy = max(floor(v0.mul_y), floor(v1.mul_y), floor(v2.mul_y));

		minx = clamp(minx, 0, (int)GL::m_width - 1);
		maxx = clamp(maxx, 0, (int)GL::m_width - 1);
		miny = clamp(miny, 0, (int)GL::m_height - 1);
		maxy = clamp(maxy, 0, (int)GL::m_height - 1);

		float area = EdgeFunction(v0, v1, v2); // area of the triangle multiplied by 2 
		float inv_area = 1 / area;

		uint32* color_ptr = (uint32*)(VBE::buffer + miny * GL::m_width + minx);
		float* depth_ptr = (float*)(depth_buffer + miny * GL::m_width + minx);

		int line_delta = GL::m_width - (maxx - minx) - 1;

		float co[3][3] = {
			{ v0.built_color.r * v0.mul_w, v1.built_color.r * v1.mul_w , v2.built_color.r * v2.mul_w },
			{ v0.built_color.g * v0.mul_w, v1.built_color.g * v1.mul_w , v2.built_color.g * v2.mul_w },
			{ v0.built_color.b * v0.mul_w, v1.built_color.b * v1.mul_w , v2.built_color.b * v2.mul_w }
		};


		Vertex p;
		p.mul_x = minx;
		p.mul_y = miny;

		float A01 = (v0.mul_y - v1.mul_y) * inv_area;
		float B01 = (v1.mul_x - v0.mul_x) * inv_area;
		float A12 = (v1.mul_y - v2.mul_y) * inv_area;
		float B12 = (v2.mul_x - v1.mul_x) * inv_area;
		float A20 = (v2.mul_y - v0.mul_y) * inv_area;
		float B20 = (v0.mul_x - v2.mul_x) * inv_area;

		float w0_row = EdgeFunction(v1, v2, p) * inv_area;
		float w1_row = EdgeFunction(v2, v0, p) * inv_area;
		float w2_row = EdgeFunction(v0, v1, p) * inv_area;

		float c0 = co[0][0];

		//float Z = 1 / (v0.mul_w * w0 + v1.mul_w * w1 + v2.mul_w * w2);
		//float U = ((w0_row * v0.tex_u * v0.mul_w) + (w1_row * v1.tex_u * v1.mul_w) + (w2_row * v2.tex_u * v2.mul_w)) * Z;
		//float V = ((w0_row * v0.tex_v * v0.mul_w) + (w1_row * v1.tex_v * v1.mul_w) + (w2_row * v2.tex_v * v2.mul_w)) * Z;

		for (int y = miny; y <= maxy; y++)
		{
			float w0 = w0_row;
			float w1 = w1_row;
			float w2 = w2_row;

			float c01 = c0;

			for (int x = minx; x <= maxx; x++)
			{
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					float r = (co[0][0] * w0 + co[0][1] * w1 + co[0][2] * w2);
					float g = (co[1][0] * w0 + co[1][1] * w1 + co[1][2] * w2);
					float b = (co[2][0] * w0 + co[2][1] * w1 + co[2][2] * w2);

					float Z = 1 / (v0.mul_w * w0 + v1.mul_w * w1 + v2.mul_w * w2);
					float U = ((w0 * v0.tex_u * v0.mul_w) + (w1 * v1.tex_u * v1.mul_w) + (w2 * v2.tex_u * v2.mul_w)) * Z;
					float V = ((w0 * v0.tex_v * v0.mul_w) + (w1 * v1.tex_v * v1.mul_w) + (w2 * v2.tex_v * v2.mul_w)) * Z;

					r *= Z;
					g *= Z;
					b *= Z;

					float lum = 0.21 * r + 0.72 * g + 0.07 * b;
					
					int X = texture->width * U;
					int Y = texture->height * V;
					
					uint32 color = texture->pixels[(int)(Y * texture->width + X)];
					r = lum * (uint8)(color >> 16);
					g = lum * (uint8)(color >> 8);
					b = lum * (uint8)(color >> 0);

					int col = ((int)(r) << 16) | ((int)(g) << 8) | (int)(b);

					if (Z < *depth_ptr)
					{
						*color_ptr = col;
						*depth_ptr = Z;
					}
				}
				
				w0 += A12;
				w1 += A20;
				w2 += A01;

				color_ptr++;
				depth_ptr++;
			}

			w0_row += B12;
			w1_row += B20;
			w2_row += B01;

			color_ptr += line_delta;
			depth_ptr += line_delta;
		}
	}
}