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
	cam->transform.position = Vector(0, 5, -15);

	//DirectionalLight* light = CreateLightSource<DirectionalLight>("Light");
	//light->SetRotation(Vector(-1.6, 1, 0.5).Normalized());

	Thing* thing = CreateObject<Thing>("Thing");
	thing->transform.position = Vector(0, 10, 0);
	thing->transform.rotation = Quaternion();
	thing->transform.scale = Vector(1, 1, 1);

	MyBox* box = CreateObject<MyBox>("Floor");
	box->transform.position = Vector(0, 0, 0);
	box->transform.rotation = Quaternion();
	box->transform.scale = Vector(1, 1, 1);
	//box->SetScale(Vector(10, 1, 10));
}
