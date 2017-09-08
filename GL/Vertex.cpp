#include "Vertex.h"

namespace gl
{
	Vertex::Vertex()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;

		this->tex_u = 0;
		this->tex_v = 0;

		this->mul_x = 0;
		this->mul_y = 0;
		this->mul_z = 0;
		this->mul_w = 0;
	}

	Vertex::Vertex(float x, float y, float z, ColRGB col, Vector4 normal) : Vertex()
	{
		this->x = x;
		this->y = y;
		this->z = z;

		this->color = col;
		this->normal = normal;
	}

	void Vertex::MulMatrix(const Matrix4& mat)
	{
		mul_x = mat.elems[0] * x + mat.elems[1] * y + mat.elems[2] * z + mat.elems[3];
		mul_y = mat.elems[4] * x + mat.elems[5] * y + mat.elems[6] * z + mat.elems[7];
		mul_z = mat.elems[8] * x + mat.elems[9] * y + mat.elems[10] * z + mat.elems[11];
		mul_w = mat.elems[12] * x + mat.elems[13] * y + mat.elems[14] * z + mat.elems[15];
	}
}
