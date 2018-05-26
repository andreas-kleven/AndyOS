#include "GL.h"
#include "System.h"
#include "Rasterizer.h"
#include "string.h"

GC GL::gc_buf;
GC GL::gc_out;

uint32 GL::m_width;
uint32 GL::m_height;
uint32 GL::m_stride;

BMP** GL::m_textures;
int tex_index;
int bound_tex;

Vector4 lightsources[GL_MAX_LIGHTSOURCES];
int light_index;

GLMatrixMode mat_mode = GL_PROJECTION;
Matrix4 mat_model;
Matrix4 mat_view;
Matrix4 mat_projection;

Matrix4* mat_stack;
int mat_stack_index;

Vertex* vert_ptr;

Vector4 cam_dir;

STATUS GL::Init(GC gc)
{
	gc_buf = GC(gc.width, gc.height);
	gc_out = gc;

	m_width = gc_buf.width;
	m_height = gc_buf.height;
	m_stride = gc_buf.stride;

	m_textures = new BMP*[GL_MAX_TEXTURES + 1];
	m_textures[0] = 0;

	mat_stack = new Matrix4[GL_MATRIX_STACK_LENGTH];

	tex_index = 1;
	bound_tex = 0;
	light_index = 0;
	mat_stack_index = 0;

	Rasterizer::Init();
	return STATUS_SUCCESS;
}

int GL::AddTexture(BMP* bmp)
{
	if (tex_index == GL_MAX_TEXTURES + 1)
		return 0;

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


void GL::CameraDirection(Vector4 dir) {
	cam_dir = dir;
}

Vector4 Reflect(Vector4 d, Vector4 n)
{
	return d - n * d.Dot(n) * 2;
}

void GL::Draw(int start, int count)
{
	Vector4 light = Vector4(0.3, -1, 0.5, 0).Normalized();
	Vector4 light2 = Vector4(-1, 0.8, -1, 0).Normalized();

	float diffuse = 1;
	float specular = 1;

	Matrix4 M = mat_projection * mat_view * mat_model;
	BMP* texture = m_textures[bound_tex];

	int end = start + count;
	for (int i = start; i < end; i += 3)
	{
		Vertex& a = vert_ptr[i];
		Vertex& b = vert_ptr[i + 1];
		Vertex& c = vert_ptr[i + 2];

		a.MulMatrix(mat_model);
		b.MulMatrix(mat_model);
		c.MulMatrix(mat_model);

		//Specular
		float specA = clamp(cam_dir.Dot(-Reflect(light, a.worldNormal)), 0.f, 1.f);
		float specB = clamp(cam_dir.Dot(-Reflect(light, b.worldNormal)), 0.f, 1.f);
		float specC = clamp(cam_dir.Dot(-Reflect(light, c.worldNormal)), 0.f, 1.f);

		//Diffuse
		float diffA = clamp(-a.worldNormal.Dot(light), 0.f, 1.f);
		float diffB = clamp(-b.worldNormal.Dot(light), 0.f, 1.f);
		float diffC = clamp(-c.worldNormal.Dot(light), 0.f, 1.f);

		a.builtColor = a.color * (0.1 + diffuse * diffA + specular * pow(specA, 5));
		b.builtColor = b.color * (0.1 + diffuse * diffB + specular * pow(specB, 5));
		c.builtColor = c.color * (0.1 + diffuse * diffC + specular * pow(specC, 5));

		a.MulMatrix(M);
		b.MulMatrix(M);
		c.MulMatrix(M);

		if (a.tmpPos.w > 0 && b.tmpPos.w > 0 && c.tmpPos.w > 0)
		{
			Rasterizer::DrawTriangle(a, b, c, texture);
		}
	}
}

void GL::Clear(uint32 color)
{
	Drawing::Clear(color, gc_buf);

	float val = 1e100;
	memset32(Rasterizer::depth_buffer, *(uint32*)&val, m_width * m_height);
}

void GL::SwapBuffers()
{
	Drawing::BitBlt(gc_buf, 0, 0, gc_buf.width, gc_buf.height, gc_out, 0, 0);
}


Matrix4& GL::SelectedMatrix()
{
	switch (mat_mode)
	{
	case GL_MODEL:
		return mat_model;

	case GL_VIEW:
		return mat_view;

	case GL_PROJECTION:
		return mat_projection;
	}
}