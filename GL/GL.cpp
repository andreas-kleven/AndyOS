#include "GL.h"
#include "System.h"
#include "Rasterizer.h"
#include "string.h"

namespace gl
{
	/*void GL::Draw(int start, int length)
	{
		Vector4 light = Vector4(0.3, -1, 0.5, 1).Normalized();
		Vector4 light2 = Vector4(-1, 0.8, -1, 1).Normalized();

		for (int i = start; i < length; i += 3)
		{
			Vertex* a = &vertex_buffer[i];
			Vertex* b = &vertex_buffer[i + 1];
			Vertex* c = &vertex_buffer[i + 2];

			//float asd = -a->normal.Normalized().Dot(light);
			//Debug::Print("%f\n", asd);
			//PIT::Sleep(10);

			//a->built_color = (a->color * -a->normal.Normalized().Dot(light)) * 1 + (a->color * -a->normal.Normalized().Dot(light2)) * 0.1;
			//b->built_color = (b->color * -b->normal.Normalized().Dot(light)) * 1 + (b->color * -b->normal.Normalized().Dot(light2)) * 0.1;
			//c->built_color = (c->color * -c->normal.Normalized().Dot(light)) * 1 + (c->color * -c->normal.Normalized().Dot(light2)) * 0.1;
			//Debug::Print("%f, %f, %f\n", a->built_color.r, a->built_color.g, a->built_color.b);

			if (a->w > 0 && b->w > 0 && c->w > 0)
			{
				//for (int c = 0; c < 10; c++)
				//if (i < length * 3)
					Rasterizer::DrawTriangle(&vertex_buffer[i]);
					 
				//VBE::DrawLine((int)(a->x * GL::m_width / a->w + m_width / 2), (int)(a->y * GL::m_height / a->w + m_height / 2), (int)(b->x * GL::m_width / b->w + m_width / 2), (int)(b->y * GL::m_height / b->w + m_height / 2), 0xFF0000);
				//VBE::DrawLine((int)(b->x * GL::m_width / b->w + m_width / 2), (int)(b->y * GL::m_height / b->w + m_height / 2), (int)(c->x * GL::m_width / c->w + m_width / 2), (int)(c->y * GL::m_height / c->w + m_height / 2), 0xFF0000);
				//VBE::DrawLine((int)(c->x * GL::m_width / c->w + m_width / 2), (int)(c->y * GL::m_height / c->w + m_height / 2), (int)(a->x * GL::m_width / a->w + m_width / 2), (int)(a->y * GL::m_height / a->w + m_height / 2), 0xFF0000);

			}
		}
	}

	void GL::SetVertexBuffer(Vertex* vertices, int start, int length)
	{
		memcpy(vertex_buffer + start, vertices, sizeof(Vertex) * length);
	}

	void GL::SetTextureBuffer(int* indexes, int length)
	{
		memcpy(texture_buffer, indexes, sizeof(int) * length);
	}

	void GL::MultiplyVertexBuffer(Matrix matrix, int start, int length)
	{
		for (int i = start; i < length + start; i++)
		{
			vertex_buffer[i] *= matrix;
		}
	}*/

	uint32 GL::m_width = 1024;
	uint32 GL::m_height = 768;

	BMP* GL::m_textures[GL_MAX_TEXTURES];
	int tex_index = 0;
	int bound_tex = -1;

	GLMatrixMode mat_mode = GL_PROJECTION;
	Matrix4 mat_projection;
	Matrix4 mat_viewmodel;

	Matrix4 mat_stack[GL_MATRIX_STACK_LENGTH];
	int mat_stack_index = 0;

	Vertex* vert_ptr;


	STATUS GL::Init()
	{
		Rasterizer::Init();
		return STATUS_SUCCESS;
	}


	int GL::AddTexture(BMP* bmp)
	{
		if (tex_index == GL_MAX_TEXTURES)
			return -1;

		m_textures[tex_index] = bmp;
		return tex_index++;
	}

	void GL::BindTexture(int id)
	{
		bound_tex = id;
	}

	void GL::MatrixMode(GLMatrixMode mode)
	{
		mat_mode = mode;
	}

	void GL::MulMatrix(const Matrix4& mat)
	{
		Matrix4& m = SelectedMatrix();
		m = m * mat;
	}

	void GL::LoadIdentity()
	{
		SelectedMatrix() = Matrix4();
	}

	void GL::LoadMatrix(const Matrix4& mat)
	{
		SelectedMatrix() = mat;
	}

	void GL::PushMatrix()
	{
		mat_stack[mat_stack_index++] = SelectedMatrix();
	}

	void GL::PopMatrix()
	{
		SelectedMatrix() = mat_stack[--mat_stack_index];
	}


	void GL::VertexPointer(Vertex* ptr)
	{
		vert_ptr = ptr;
	}


	void GL::Clear(uint32 color)
	{
		VBE::Clear(color);

		float val = 100000;
		memset32(Rasterizer::depth_buffer, *(uint32*)&val, m_width * m_height);
	}

	void GL::Draw(int start, int count)
	{
		Vector4 light = Vector4(0.3, -1, 0.5, 0).Normalized();
		Vector4 light2 = Vector4(-1, 0.8, -1, 0).Normalized();

		Matrix4 M = mat_projection * mat_viewmodel;

		int end = start + count;
		for (int i = start; i < end; i += 3)
		{
			Vertex* a = &vert_ptr[i];
			Vertex* b = &vert_ptr[i + 1];
			Vertex* c = &vert_ptr[i + 2];

			a->MulMatrix(M);
			b->MulMatrix(M);
			c->MulMatrix(M);


			//float asd = -a->normal.Normalized().Dot(light);
			//Debug::Print("%f\n", asd);
			//PIT::Sleep(10);

			a->built_color = (a->color * -a->normal.Dot(light)) * 1 + (a->color * -a->normal.Dot(light2)) * 0.1;
			b->built_color = (b->color * -b->normal.Dot(light)) * 1 + (b->color * -b->normal.Dot(light2)) * 0.1;
			c->built_color = (c->color * -c->normal.Dot(light)) * 1 + (c->color * -c->normal.Dot(light2)) * 0.1;
			//Debug::Print("%f, %f, %f\n", a->built_color.r, a->built_color.g, a->built_color.b);

			if (a->mul_w > 0 && b->mul_w > 0 && c->mul_w > 0)
			{
				Rasterizer::DrawTriangle(&vert_ptr[i], m_textures[bound_tex]);

				//VBE::DrawLine((int)(a->x * GL::m_width / a->w + m_width / 2), (int)(a->y * GL::m_height / a->w + m_height / 2), (int)(b->x * GL::m_width / b->w + m_width / 2), (int)(b->y * GL::m_height / b->w + m_height / 2), 0xFF0000);
				//VBE::DrawLine((int)(b->x * GL::m_width / b->w + m_width / 2), (int)(b->y * GL::m_height / b->w + m_height / 2), (int)(c->x * GL::m_width / c->w + m_width / 2), (int)(c->y * GL::m_height / c->w + m_height / 2), 0xFF0000);
				//VBE::DrawLine((int)(c->x * GL::m_width / c->w + m_width / 2), (int)(c->y * GL::m_height / c->w + m_height / 2), (int)(a->x * GL::m_width / a->w + m_width / 2), (int)(a->y * GL::m_height / a->w + m_height / 2), 0xFF0000);

			}
		}
	}

	void GL::SwapBuffers()
	{
		VBE::Draw();
	}


	Matrix4& GL::SelectedMatrix()
	{
		switch (mat_mode)
		{
		case GL_PROJECTION:
			return mat_projection;

		case GL_MODELVIEW:
			return mat_viewmodel;
		}
	}
}