#pragma once
#include "Component.h"
#include "Shader.h"
#include "Box.h"
#include "../GL/Vertex.h"

class MeshComponent : public Component
{
public:
	Vertex* vertices;
	int vertex_count;
	Box bounds;

	Shader shader;
	int texId;

	MeshComponent();

	virtual void Update(float delta);

	void SetMesh(Vertex* vertices, int vertex_count);
	void CalculateBounds();
};