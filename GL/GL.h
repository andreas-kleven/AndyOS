#pragma once
#include "definitions.h"
#include "System.h"
#include "Vertex.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Color.h"

#define GL_MAX_TEXTURES			1024
#define GL_MAX_LIGHTSOURCES		4
#define GL_MATRIX_STACK_LENGTH	32

enum GLMatrixMode
{
	GL_MODEL,
	GL_VIEW,
	GL_PROJECTION
};

static class GL
{
public:
	static GC gc_buf;
	static GC gc_out;

	static uint32 m_width;
	static uint32 m_height;
	static uint32 m_stride;

	static BMP** m_textures;

	static STATUS Init(GC gc);

	static int AddTexture(BMP* bmp);
	static void BindTexture(int id);

	static int LightSource(Vector4 light);

	static void MatrixMode(GLMatrixMode mode);
	static void MulMatrix(const Matrix4& mat);

	static void LoadIdentity();
	static void LoadMatrix(const Matrix4& mat);

	static void PushMatrix();
	static void PopMatrix();

	static void CameraDirection(Vector4 dir);

	//static void LightingFunction()

	static void Draw(Vertex* verts, int count);
	static void Clear(uint32 color);
	static void SwapBuffers();

private:
	static Matrix4& SelectedMatrix();

	static ColRGB(*LightingFunction)(int x, int y, int z, const Vector4& normal);
};