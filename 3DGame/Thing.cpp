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
	Model3D* mod = ModelLoader::LoadModel("sphere.a3d", Format3D::FORMAT_A3D);
	
	char* img_buf;
	if (!FS::ReadFile("earth.bmp", img_buf))
	{
		Debug::Print("bmp not found");
		while (1);
	}
	BMP* bmp = new BMP(img_buf);
	Debug::Print("%i\n", bmp->width);

	//for (int y = 0; y < bmp->height; y++)
	//{
	//	for (int x = 0; x < bmp->width; x++)
	//	{
	//		uint32 pixel = bmp->pixels[y * bmp->width + x];
	//		VBE::SetPixel(x / 2, y / 2, pixel);
	//	}
	//}
	//Drawing::Paint();
	//while (1);

	MeshComponent* mesh = CreateComponent<MeshComponent>("Mesh");
	mesh->vertices = mod->vertices;
	mesh->vertex_count = mod->vertex_count;
	//mesh->texId = gl::GL::AddTexture(bmp);

	//SphereCollider* sphere = CreateComponent<SphereCollider>("SphereCollider");
	//sphere->radius = 1;
	phys = CreateComponent<Rigidbody>("Rigidbody");
	phys->collider = new BoxCollider();
	//phys->bEnabled = 0;
	phys->bEnabledGravity = 0;
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
		phys->AddImpulse(Vector3(0, speed, 0) * delta);
	}

	if (Keyboard::GetKeyDown(KEY_U))
	{
		phys->AddImpulse(Vector3(0, -speed, 0) * delta);
	}
}