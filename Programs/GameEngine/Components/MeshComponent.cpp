#include <math.h>
#include <andyos/float.h>
#include "Components/MeshComponent.h"
#include "GameObject.h"

MeshComponent::MeshComponent()
{
	shader = Shader();
	texId = 0;
}

void MeshComponent::Update(float delta)
{
}

void MeshComponent::SetModel(Model3D* _model)
{
	model = _model;

	bvh = new KDTree();
	bvh->Build(_model);
}