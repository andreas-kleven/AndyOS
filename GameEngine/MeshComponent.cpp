#include "MeshComponent.h"
#include "GameObject.h"
#include "limits.h"
#include "math.h"

MeshComponent::MeshComponent()
{
	shader = Shader();
	texId = 0;
}

void MeshComponent::Update(float delta)
{
}

void MeshComponent::SetModel(Model3D* model)
{
	vertices = model->vertices;
	vertex_count = model->vertex_count;
	CalculateBounds();
}

void MeshComponent::CalculateBounds()
{
	if (vertex_count == 0)
	{
		bounds = Box();
		return;
	}

	bounds = Box(
		Vector3(FLT_MAX, FLT_MAX, FLT_MAX),
		Vector3(FLT_MIN, FLT_MIN, FLT_MIN));

	Transform transform = parent->GetWorldTransform();

	Matrix4 T = Matrix4::CreateTranslation(transform.position.ToVector4());
	Matrix4 R = transform.rotation.ToMatrix();
	Matrix4 S = Matrix4::CreateScale(transform.scale.ToVector4());
	Matrix4 M = T * R * S;

	for (int i = 0; i < vertex_count; i++)
	{
		Vector4& pos = M * vertices[i].pos;

		if (pos.x < bounds.min.x) bounds.min.x = pos.x;
		if (pos.y < bounds.min.y) bounds.min.y = pos.y;
		if (pos.z < bounds.min.z) bounds.min.z = pos.z;

		if (pos.x > bounds.max.x) bounds.max.x = pos.x;
		if (pos.y > bounds.max.y) bounds.max.y = pos.y;
		if (pos.z > bounds.max.z) bounds.max.z = pos.z;
	}
}

