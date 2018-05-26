#include "3DGame.h"
#include "System.h"
#include "MyBox.h"
#include "MeshComponent.h"
#include "Model3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Thing.h"
#include "MySphere.h"
#include "BoxCollider.h"

#include "../GL/GL.h"

MyGame::MyGame()
{
	Camera* cam = CreateCamera<Camera>("Cam1");
	cam->transform.position = Vector3(0, 6, -20);

	//DirectionalLight* light = CreateLightSource<DirectionalLight>("Light");
	//light->transform.position = Vector3(0, 100, 0);
	//light->transform.rotation = Quaternion::LookAt(Vector3(), Vector3(0.3, -1, 0.5));

	PointLight* light = CreateLightSource<PointLight>("PointLight");
	light->transform.position = Vector3(2, 2, -2);
	light->intensity = 100;

	Thing* thing = CreateObject<Thing>("Thing");
	thing->transform.position = Vector3(0, 6, 0);
	thing->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, M_PI_4));
	thing->transform.scale = Vector3(4, 0.2, 0.2);

	MySphere* sphere = CreateObject<MySphere>("Sphere");
	sphere->transform.position = Vector3(0, 4, 0);
	sphere->transform.scale = Vector3(4, 4, 1);

	//Walls
	MyBox* walls[6];

	walls[0] = CreateObject<MyBox>("Floor");
	walls[0]->transform.position = Vector3(0, -1, 0);
	walls[0]->transform.scale = Vector3(1, 1, 2);

	walls[1] = CreateObject<MyBox>("Roof");
	walls[1]->transform.position = Vector3(0, 1, 0);
	walls[1]->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, M_PI));
	walls[1]->transform.scale = Vector3(1, 1, 2);

	walls[2] = CreateObject<MyBox>("Wall-Left");
	walls[2]->transform.position = Vector3(-1, 0, 0);
	walls[2]->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, -M_PI_2));
	walls[2]->transform.scale = Vector3(1, 1, 2);

	walls[3] = CreateObject<MyBox>("Wall-Right");
	walls[3]->transform.position = Vector3(1, 0, 0);
	walls[3]->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, M_PI_2));
	walls[3]->transform.scale = Vector3(1, 1, 2);

	walls[4] = CreateObject<MyBox>("Wall-Back");
	walls[4]->transform.position = Vector3(0, 0, -1);
	walls[4]->transform.rotation = Quaternion::FromEuler(Vector3(M_PI_2, 0, 0));

	walls[5] = CreateObject<MyBox>("Wall-Front");
	walls[5]->transform.position = Vector3(0, 0, 1);
	walls[5]->transform.rotation = Quaternion::FromEuler(Vector3(-M_PI_2, 0, 0));

	ColRGB colors[] =
	{
		ColRGB(1, 1, 1),
		ColRGB(1, 1, 1),
		ColRGB(1, 0, 0),
		ColRGB(0, 1, 0),
		ColRGB(1, 1, 1),
		ColRGB(1, 1, 1)
	};

	for (int i = 0; i < 6; i++)
	{
		Vector3 scale(10, 10, 20);

		walls[i]->transform.position.x *= scale.x;
		walls[i]->transform.position.y *= scale.y;
		walls[i]->transform.position.z *= scale.z;

		walls[i]->transform.Scale(Vector3(10, 1, 10));

		MeshComponent* mesh = walls[i]->meshComponents[0];

		for (int j = 0; j < mesh->vertex_count; j++)
		{
			mesh->vertices[j].color = colors[i];
		}
	}

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
