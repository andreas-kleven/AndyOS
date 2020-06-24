#pragma once
#include <AndyOS.h>
#include <sys/types.h>
#include "Vertex.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Vector4.h"
#include "Matrix4.h"

#define GL_MAX_TEXTURES			1024
#define GL_MAX_LIGHTSOURCES		4
#define GL_MATRIX_STACK_LENGTH	32

enum GLMatrixMode
{
	GL_MODEL,
	GL_VIEW,
	GL_PROJECTION
};

namespace GL
{
	int AddTexture(BMP* bmp);
	void BindTexture(int id);

	int LightSource(Vector4 light);

	void MatrixMode(GLMatrixMode mode);
	void MulMatrix(const Matrix4& mat);

	void LoadIdentity();
	void LoadMatrix(const Matrix4& mat);

	void PushMatrix();
	void PopMatrix();

	void CameraDirection(Vector4 dir);

	void Draw(Vertex* verts, int count);
	void Clear(Color color);
	void SwapBuffers();
	
	void Init(GC gc);
};