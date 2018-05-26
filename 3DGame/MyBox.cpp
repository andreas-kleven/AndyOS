#include "MyBox.h"
#include "MeshComponent.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"
#include "Model3D.h"
#include "ModelLoader.h"
#include "debug.h"

char* img_buf = 0;
BMP* bmp = 0;

MyBox::MyBox()
{
	Model3D* model = ModelLoader::LoadModel("plane.a3d", Format3D::FORMAT_A3D);

	if (!img_buf)
	{
		if (!FS::ReadFile("img.bmp", img_buf))
		{
			Debug::Print("bmp not found");
			while (1);
		}
		bmp = new BMP(img_buf);
	}

	MeshComponent* mesh = CreateComponent<MeshComponent>("Mesh");
	mesh->vertices = model->vertices;
	mesh->vertex_count = model->vertex_count;
	//mesh->texId = GL::AddTexture(bmp);

	//SphereCollider* sphere = CreateComponent<SphereCollider>("SphereCollider");
	//sphere->radius = 1;

	Rigidbody* phys = CreateComponent<Rigidbody>("Rigidbody");
	phys->mass = 1e10;
	phys->collider = new BoxCollider();
	phys->bEnabled = 0;
	phys->bEnabledGravity = 0;
}
