#include "MySphere.h"
#include "GEngine.h"

MySphere::MySphere()
{
	Model3D* model = ModelLoader::LoadModel("gamedata/bunny.a3d", Format3D::FORMAT_A3D);

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
