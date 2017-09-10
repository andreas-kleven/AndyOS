#include "Vertex.h"

namespace gl
{
	Vertex::Vertex()
	{
		this->tex_u = 0;
		this->tex_v = 0;
	}

	Vertex::Vertex(float x, float y, float z, ColRGB col, Vector4 normal) : Vertex()
	{
		this->pos.x = x;
		this->pos.y = y;
		this->pos.z = z;

		this->color = col;
		this->builtColor = col;

		this->normal = normal;
		this->worldNormal = normal;
	}

	void Vertex::MulMatrix(const Matrix4& mat)
	{
		tmpPos.x = mat.elems[0] * pos.x + mat.elems[1] * pos.y + mat.elems[2] * pos.z + mat.elems[3];
		tmpPos.y = mat.elems[4] * pos.x + mat.elems[5] * pos.y + mat.elems[6] * pos.z + mat.elems[7];
		tmpPos.z = mat.elems[8] * pos.x + mat.elems[9] * pos.y + mat.elems[10] * pos.z + mat.elems[11];
		tmpPos.w = mat.elems[12] * pos.x + mat.elems[13] * pos.y + mat.elems[14] * pos.z + mat.elems[15];
	}
}
