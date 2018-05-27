#include "MySphere.h"
#include "MeshComponent.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Model3D.h"
#include "ModelLoader.h"
#include "Engine.h"
#include "debug.h"

MySphere::MySphere()
{
	Model3D* model = ModelLoader::LoadModel("sphere.a3d", Format3D::FORMAT_A3D);

	MeshComponent* mesh = CreateComponent<MeshComponent>("Mesh");
	mesh->SetModel(model);
	mesh->shader = Shader(0.1f, 0.0f, 1.5f);

	Rigidbody* phys = CreateComponent<Rigidbody>("Rigidbody");
	phys->collider = new SphereCollider();
	phys->bEnabled = 0;
}

MySphere::~MySphere()
{
}
