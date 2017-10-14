#include "GL.h"
#include "System.h"
#include "Rasterizer.h"
#include "string.h"

namespace gl
{
	GC GL::gc_buf;
	GC GL::gc_out;

	uint32 GL::m_width;
	uint32 GL::m_height;
	uint32 GL::m_stride;

	BMP* GL::m_textures[GL_MAX_TEXTURES];
	int tex_index = 0;
	int bound_tex = -1;

	Vector4 lightsources[GL_MAX_LIGHTSOURCES];
	int light_index = 0;

	GLMatrixMode mat_mode = GL_PROJECTION;
	Matrix4 mat_projection;
	Matrix4 mat_viewmodel;

	Matrix4 mat_stack[GL_MATRIX_STACK_LENGTH];
	int mat_stack_index = 0;

	Vertex* vert_ptr;


	STATUS GL::Init(GC gc)
	{
		gc_buf = GC::CreateGraphics(gc.width, gc.height);
		gc_out = gc;

		m_width = gc_buf.width;
		m_height = gc_buf.height;
		m_stride = gc_buf.stride;

		tex_index = 0;
		bound_tex = -1;
		light_index = 0;
		mat_stack_index = 0;

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

	int GL::LightSource(Vector4 light)
	{
		light_index = 0;

		if (light_index == GL_MAX_LIGHTSOURCES)
			return -1;

		float strength = light.w;
		Vector4 t = SelectedMatrix() * light;
		t.w = strength;

		lightsources[light_index] = t;
		return light_index++;
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
		Drawing::Clear(color, gc_buf);

		float val = 1e100;
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
			Vertex& a = vert_ptr[i];
			Vertex& b = vert_ptr[i + 1];
			Vertex& c = vert_ptr[i + 2];

			a.MulMatrix(M);
			b.MulMatrix(M);
			c.MulMatrix(M);

			float la = -a.worldNormal.Dot(light);
			float lc = -c.worldNormal.Dot(light);
			float lb = -b.worldNormal.Dot(light);

			//la = 100 * pow(clamp(Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(a.normal) * 2)), 0.f, a.normal.Dot(Vector4(0, 0, 1, 0))), 10);
			//lb = 100 * pow(clamp(Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(b.normal) * 2)), 0.f, b.normal.Dot(Vector4(0, 0, 1, 0))), 10);
			//lc = 100 * pow(clamp(Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(c.normal) * 2)), 0.f, c.normal.Dot(Vector4(0, 0, 1, 0))), 10);

			//la += Vector4(0, 0, 1, 0).Dot(a.normal * Vector4::Dot(light, a.normal) - light);
			//lb += Vector4(0, 0, 1, 0).Dot(b.normal * Vector4::Dot(light, b.normal) - light);
			//lc += Vector4(0, 0, 1, 0).Dot(c.normal * Vector4::Dot(light, c.normal) - light);

			//la = light.Dot(light - a.normal * (((light * 2).Dot(a.normal)) / (a.normal.Magnitude() * a.normal.Magnitude())));
			//lb = light.Dot(light - b.normal * (((light * 2).Dot(b.normal)) / (b.normal.Magnitude() * b.normal.Magnitude())));
			//lc = light.Dot(light - c.normal * (((light * 2).Dot(c.normal)) / (c.normal.Magnitude() * c.normal.Magnitude())));

			//a.builtColor = (a.color * Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(a.normal) * 2)))/* * 1 + (a.color * -a.normal.Dot(light2)) * 0.1*/;
			//b.builtColor = (b.color * Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(b.normal) * 2)))/* * 1 + (b.color * -b.normal.Dot(light2)) * 0.1*/;
			//c.builtColor = (c.color * Vector4(0, 0, 1, 0).Dot(light - (light * light.Dot(c.normal) * 2)))/* * 1 + (c.color * -c.normal.Dot(light2)) * 0.1*/;

			//a.builtColor = (a.color * -a.normal.Dot(light))/* * 1 + (a.color * -a.normal.Dot(light2)) * 0.1*/;
			//b.builtColor = (b.color * -b.normal.Dot(light))/* * 1 + (b.color * -b.normal.Dot(light2)) * 0.1*/;
			//c.builtColor = (c.color * -c.normal.Dot(light))/* * 1 + (c.color * -c.normal.Dot(light2)) * 0.1*/;

			a.builtColor = (a.color * la);
			b.builtColor = (b.color * lb);
			c.builtColor = (c.color * lc);

			if (a.tmpPos.w > 0 && b.tmpPos.w > 0 && c.tmpPos.w > 0)
			{
				Rasterizer::DrawTriangle(a, b, c, m_textures[bound_tex]);
			}
		}
	}

	void GL::SwapBuffers()
	{
		//Drawing::Draw(gc);
		Drawing::BitBlt(gc_buf, 0, 0, gc_buf.width, gc_buf.height, gc_out, 0, 0);
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