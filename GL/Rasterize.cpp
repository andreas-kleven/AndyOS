#include "Rasterize.h"
#include "System.h"
#include "GL.h"
#include "math.h"

namespace gl
{
	float EdgeFunction(const Vertex &a, const Vertex &b, const Vertex &c)
	{
		return (c.mul_y - a.mul_y) * (b.mul_x - a.mul_x) - (c.mul_x - a.mul_x) * (b.mul_y - a.mul_y);
	}

	uint32* Rasterize::color_buffer;
	float* Rasterize::depth_buffer;

	STATUS Rasterize::Init()
	{
		uint32 pixels = GL::m_height * GL::m_width;
		uint32 blocks = pixels * sizeof(uint32) / BLOCK_SIZE;

		depth_buffer = (float*)Memory::AllocBlocks(blocks);
		return STATUS_SUCCESS;
	}

	void Rasterize::DrawTriangle(Vertex* buffer, BMP* texture)
	{
		uint32* colorBuffer = VBE::buffer;
		uint32 stride = GL::m_width * 4;

		Vertex& v0 = buffer[0];
		Vertex& v1 = buffer[1];
		Vertex& v2 = buffer[2];

		v0.mul_x = v0.mul_x * GL::m_width / v0.mul_w + GL::m_width / 2;
		v1.mul_x = v1.mul_x * GL::m_width / v1.mul_w + GL::m_width / 2;
		v2.mul_x = v2.mul_x * GL::m_width / v2.mul_w + GL::m_width / 2;

		v0.mul_y = v0.mul_y * GL::m_height / v0.mul_w + GL::m_height / 2;
		v1.mul_y = v1.mul_y * GL::m_height / v1.mul_w + GL::m_height / 2;
		v2.mul_y = v2.mul_y * GL::m_height / v2.mul_w + GL::m_height / 2;

		/*VBE::DrawLine((int)v0.mul_x, (int)v0.mul_y, (int)v1.mul_x, (int)v1.mul_y, 0xFF);
		VBE::DrawLine((int)v1.mul_x, (int)v1.mul_y, (int)v2.mul_x, (int)v2.mul_y, 0xFF00);
		VBE::DrawLine((int)v2.mul_x, (int)v2.mul_y, (int)v0.mul_x, (int)v0.mul_y, 0xFF0000);

		Debug::Print("%i, %i\n", x1, y1);
		Debug::Print("%i, %i\n", x2, y2);
		Debug::Print("%i, %i\n", x3, y3);

		VBE::Draw(); 
		PIT::Sleep(10);*/

		int minx = min(floor(v0.mul_x), floor(v1.mul_x), floor(v2.mul_x));
		int maxx = max(floor(v0.mul_x), floor(v1.mul_x), floor(v2.mul_x));
		int miny = min(floor(v0.mul_y), floor(v1.mul_y), floor(v2.mul_y));
		int maxy = max(floor(v0.mul_y), floor(v1.mul_y), floor(v2.mul_y));

		minx = clamp(minx, 0, (int)GL::m_width - 1);
		maxx = clamp(maxx, 0, (int)GL::m_width - 1);
		miny = clamp(miny, 0, (int)GL::m_height - 1);
		maxy = clamp(maxy, 0, (int)GL::m_height - 1);

		float area = EdgeFunction(v0, v1, v2); // area of the triangle multiplied by 2 


		uint32* color_ptr = (uint32*)(VBE::buffer + miny * GL::m_width + minx);
		float* depth_ptr = (float*)(depth_buffer + miny * GL::m_width + minx);

		int line_delta = (maxx - minx) + 1;

		v0.mul_w = 1 / v0.mul_w;
		v1.mul_w = 1 / v1.mul_w;
		v2.mul_w = 1 / v2.mul_w;

		float co[3][3] = {
			{ v0.built_color.r * v0.mul_w, v1.built_color.r * v1.mul_w , v2.built_color.r * v2.mul_w },
			{ v0.built_color.g * v0.mul_w, v1.built_color.g * v1.mul_w , v2.built_color.g * v2.mul_w },
			{ v0.built_color.b * v0.mul_w, v1.built_color.b * v1.mul_w , v2.built_color.b * v2.mul_w }
		};

		int A01 = v0.mul_y - v1.mul_y, B01 = v1.mul_x - v0.mul_x;
		int A12 = v1.mul_y - v2.mul_y, B12 = v2.mul_x - v1.mul_x;
		int A20 = v2.mul_y - v0.mul_y, B20 = v0.mul_x - v2.mul_x;

		Vertex p;
		for (p.mul_y = miny; p.mul_y <= maxy; p.mul_y++)
		{
			for (p.mul_x = minx; p.mul_x <= maxx; p.mul_x++)
			{
				float w0 = EdgeFunction(v1, v2, p);
				float w1 = EdgeFunction(v2, v0, p);
				float w2 = EdgeFunction(v0, v1, p);
				//float w2 = area - w0 - w1;

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 /= area;
					w1 /= area;
					w2 /= area;

					//if ((w0 + w1 + w2) != 1)
					//	Debug::Print("%f ", (w0 + w1 + w2));

					float r = (co[0][0] * w0 + co[0][1] * w1 + co[0][2] * w2);
					float g = (co[1][0] * w0 + co[1][1] * w1 + co[1][2] * w2);
					float b = (co[2][0] * w0 + co[2][1] * w1 + co[2][2] * w2);

					//for (int gg = 0; gg < bmp->height; gg++)
					//{
					//	for (int ww = 0; ww < bmp->width; ww++)
					//	{
					//		VBE::SetPixel(ww, gg, bmp->pixels[gg * bmp->width + ww]);
					//	}
					//}
					//
					//VBE::Draw();
					//while (1);

					float Z = 1 / (v0.mul_w * w0 + v1.mul_w * w1 + v2.mul_w * w2);
					float U = ((w0 * v0.tex_u * v0.mul_w) + (w1 * v1.tex_u * v1.mul_w) + (w2 * v2.tex_u * v2.mul_w)) * Z;
					float V = ((w0 * v0.tex_v * v0.mul_w) + (w1 * v1.tex_v * v1.mul_w) + (w2 * v2.tex_v * v2.mul_w)) * Z;

					//Debug::Print("%f\t%f\n", U, V);
					//Debug::Print("%f, %f, %f\t", r, g, b);
					r *= Z;
					g *= Z;
					b *= Z;

					float lum = 0.21 * r + 0.72 * g + 0.07 * b;

					int X = texture->width * U;
					int Y = texture->height * V;

					//col = bmp->pixels[(int)(U * (bmp->width - 1) + V * (bmp->height - 1) * bmp->width)];
					ColRGB color(texture->pixels[(int)(Y * texture->width + X)]);
					//r = color.r;
					//g = color.g;
					//b = color.b;
					r = lum * color.r;
					g = lum * color.g;
					b = lum * color.b;
					//Debug::Print("%f, %f, %f\n", r, g, b);
					//Debug::Print("%i, %i\n", X, Y);

					int col = ((int)(r * 255) << 16) | ((int)(g * 255) << 8) | (int)(b * 255);

					if (Z < *depth_ptr)
					{
						//*color_ptr = col;
						VBE::SetPixel((int)p.mul_x, (int)(p.mul_y), col);
						*depth_ptr = Z;
					}

				}

				color_ptr++;
				depth_ptr++;
			}

			color_ptr += GL::m_width - line_delta;
			depth_ptr += GL::m_width - line_delta;
		}
	}
}