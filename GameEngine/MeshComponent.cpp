#include "MeshComponent.h"
#include "math.h"

MeshComponent::MeshComponent()
{
	shader = Shader();
	texId = 0;
}

void MeshComponent::Update(float delta)
{
}

void MeshComponent::SetMesh(Vertex* _vertices, int _vertex_count)
{
	vertices = _vertices;
	vertex_count = _vertex_count;
	CalculateBounds();
}

void MeshComponent::CalculateBounds()
{
	for (int i = 0; i < vertex_count; i++)
	{
		Vector4& pos = vertices[i].pos;
		
		if (pos.x < bounds.min.x) bounds.min.x = pos.x;
		if (pos.y < bounds.min.y) bounds.min.x = pos.y;
		if (pos.z < bounds.min.z) bounds.min.x = pos.z;
		
		if (pos.x < bounds.max.x) bounds.max.x = pos.x;
		if (pos.y < bounds.max.y) bounds.max.x = pos.y;
		if (pos.z < bounds.max.z) bounds.max.x = pos.z;
	}
}

