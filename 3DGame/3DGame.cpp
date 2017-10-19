#include "3DGame.h"
#include "System.h"
#include "MyBox.h"
#include "MeshComponent.h"
#include "Model3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Thing.h"

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
	box->transform.scale = Vector3(1, 1, 1);
	box->transform.scale = Vector3(10, 1, 10);
	//box->SetScale(Vector3(10, 1, 10));

	return;
	int num = 100;
	for (int i = 0; i < 100; i++)
	{
		String name = "Box_";
		name += i;
		MyBox* b = CreateObject<MyBox>(name);
		//b->transform.position = Vector3((rand() % num) - num / 2, (rand() % num) - num / 2, (rand() % num) - num / 2);
		b->transform.position = Vector3((i % 10) * 4, ((i / 10) % 10) * 4 + 10, ((i / 100) % 10) * 4);
		thing->transform.rotation = Quaternion();
		thing->transform.scale = Vector3(1, 1, 1);
	}
}
