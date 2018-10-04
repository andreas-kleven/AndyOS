#include "MyBox.h"
#include "GEngine.h"

char* img_buf = 0;
BMP* bmp = 0;

MyBox::MyBox()
{
	Model3D* model = ModelLoader::LoadModel("gamedata/cube.a3d", Format3D::FORMAT_A3D);

	if (!img_buf)
	{
		if (!read_file(img_buf, "gamedata/img.bmp"))
		{
			debug_print("bmp not found");
			while (1);
		}
		bmp = new BMP(img_buf);
	}

	MeshComponent* mesh = CreateComponent<MeshComponent>("Mesh");
	mesh->SetModel(model);
	//mesh->texId = GL::AddTexture(bmp);

	//SphereCollider* sphere = CreateComponent<SphereCollider>("SphereCollider");
	//sphere->radius = 1;

	Rigidbody* phys = CreateComponent<Rigidbody>("Rigidbody");
	phys->mass = 1e10;
	phys->collider = new BoxCollider();
	//phys->bEnabled = 0;
	phys->bEnabledGravity = 0;
}
