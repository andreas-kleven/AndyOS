#pragma once
#include "Component.h"
#include "Model3D.h"
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

	void SetModel(Model3D* model);
	void CalculateBounds();
};