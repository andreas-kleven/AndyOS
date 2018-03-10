#include "3DGame.h"
#include "System.h"
#include "MyBox.h"
#include "MeshComponent.h"
#include "Model3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Thing.h"
#include "BoxCollider.h"

#include "../GL/GL.h"

MyGame::MyGame()
{
	Camera* cam = CreateCamera<Camera>("Cam1");
	cam->transform.position = Vector3(0, 5, -15);

	//DirectionalLight* light = CreateLightSource<DirectionalLight>("Light");
	//light->SetRotation(Vector3(-1.6, 1, 0.5).Normalized());

	Thing* thing = CreateObject<Thing>("Thing");
	thing->transform.position = Vector3(0, 10, 0);
	thing->transform.rotation = Quaternion();
	thing->transform.scale = Vector3(1, 1, 1);

	MyBox* box = CreateObject<MyBox>("Floor");
	box->transform.position = Vector3(0, 0, 0);
	box->transform.rotation = Quaternion();
	box->transform.scale = Vector3(10, 1, 10);

	MyBox* box1 = CreateObject<MyBox>("Floor");
	box1->transform.position = Vector3(11.1, 11, 0);
	box1->transform.rotation = Quaternion();
	box1->transform.scale = Vector3(1, 10, 10);

	MyBox* box2 = CreateObject<MyBox>("Floor");
	box2->transform.position = Vector3(-11.1, 11, 0);
	box2->transform.rotation = Quaternion();
	box2->transform.scale = Vector3(1, 10, 10);

	MyBox* box3 = CreateObject<MyBox>("Floor");
	box3->transform.position = Vector3(0, 22.1, 0);
	box3->transform.rotation = Quaternion();
	box3->transform.scale = Vector3(10, 1, 10);

	return;
	int num = 3;
	int dist = 4;
	for (int i = 0; i < 9; i++)
	{
		String name = "Box_";
		name += i;
		MyBox* b = CreateObject<MyBox>(name);
		//b->transform.position = Vector3((rand() % num) - num / 2, (rand() % num) - num / 2, (rand() % num) - num / 2);
		b->transform.position = Vector3((i % num) * dist, ((i / num) % num) * dist + num, ((i / num / num) % num) * dist) - Vector3((num + 1) * dist / 2, 0, 0);
		thing->transform.rotation = Quaternion();
		thing->transform.scale = Vector3(1, 1, 1);

		Rigidbody* phys = b->CreateComponent<Rigidbody>("Rigidbody");
		phys->collider = new BoxCollider();
		//phys->bEnabled = 0;
		phys->bEnabledGravity = 0;
	}
}
