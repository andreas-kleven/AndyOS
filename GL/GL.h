#pragma once
#include "definitions.h"
#include "System.h"
#include "Vertex.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Color.h"
#include "List.h"

#define GL_MAX_TEXTURES 32
#define GL_MATRIX_STACK_LENGTH 32

enum GLMatrixMode
{
	GL_PROJECTION,
	GL_MODELVIEW
};

namespace gl
{
	static class GL
	{
	public:
		static uint32 m_width;
		static uint32 m_height;

		static BMP* m_textures[];

		static STATUS Init();

		static int AddTexture(BMP* bmp);
		static void BindTexture(int id);

		static void MatrixMode(GLMatrixMode mode);
		static void MulMatrix(const Matrix& mat);

		static void LoadIdentity();
		static void LoadMatrix(const Matrix& mat);

		static void PushMatrix();
		static void PopMatrix();

		static void VertexPointer(Vertex* ptr);

		static void Clear(uint32 color);
		static void Draw(int start, int count);
		static void SwapBuffers();

	private:
		static Matrix& SelectedMatrix();
	};
}