#include "Thing.h"
#include "MeshComponent.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Model3D.h"
#include "ModelLoader.h"
#include "Engine.h"
#include "debug.h"

Rigidbody* phys;

Thing::Thing()
{
	Model3D* model = ModelLoader::LoadModel("cylinder.a3d", Format3D::FORMAT_A3D);
	
	char* img_buf;
	if (!FS::ReadFile("earth.bmp", img_buf))
	{
		Debug::Print("bmp not found");
		while (1);
	}
	BMP* bmp = new BMP(img_buf);

	MeshComponent* mesh = CreateComponent<MeshComponent>("Mesh");
	mesh->SetModel(model);

	for (int i = 0; i < mesh->model->vertices.Count(); i++)
	{
		mesh->model->vertices[i]->color = COLOR_RED;
	}

	//mesh->texId = GL::AddTexture(bmp);

	//SphereCollider* sphere = CreateComponent<SphereCollider>("SphereCollider");
	//sphere->radius = 1;
	phys = CreateComponent<Rigidbody>("Rigidbody");
	phys->collider = new BoxCollider();
	phys->bEnabled = 0;
	//phys->bEnabledGravity = 0;
}

float acc = -9.8 * 2;
float vel = 0;

void Thing::Update(float delta)
{
	/*vel += acc * delta;
	transform.position.y += vel * delta;

	if (transform.position.y < 1)
	{
		vel = 0;
		transform.position.y = 1;
	}*/

	float speed = 20;

	if (Keyboard::GetKeyDown(KEY_SPACE))
	{
		phys->AddImpulse(Vector3(0, speed * delta, 0));
	}

	if (Keyboard::GetKeyDown(KEY_L))
	{
		phys->AddImpulse(Vector3(speed, 0, 0) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_J))
	{
		phys->AddImpulse(Vector3(-speed, 0, 0) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_I))
	{
		phys->AddImpulse(Vector3(0, 0, speed) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_K))
	{
		phys->AddImpulse(Vector3(0, 0, -speed) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_O))
	{
		phys->AddImpulse(Vector3(0, speed + 9.8, 0) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_U))
	{
		phys->AddImpulse(Vector3(0, -speed, 0) * delta);
	}
}