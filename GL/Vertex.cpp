#include "Vertex.h"

namespace gl
{
	Vertex::Vertex()
	{
	}

	Vertex::Vertex(float x, float y, float z, ColRGB col, Vector4 normal)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->color = col;
		this->normal = normal;
	}

	void Vertex::MulMatrix(const Matrix& mat)
	{
		mul_x = mat.m_elements[0] * x + mat.m_elements[1] * y + mat.m_elements[2] * z + mat.m_elements[3];
		mul_y = mat.m_elements[4] * x + mat.m_elements[5] * y + mat.m_elements[6] * z + mat.m_elements[7];
		mul_z = mat.m_elements[8] * x + mat.m_elements[9] * y + mat.m_elements[10] * z + mat.m_elements[11];
		mul_w = mat.m_elements[12] * x + mat.m_elements[13] * y + mat.m_elements[14] * z + mat.m_elements[15];
	}
}
