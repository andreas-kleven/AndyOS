#include "3DGame.h"
#include "System.h"
#include "Box.h"
#include "MeshComponent.h"
#include "Model3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Thing.h"

#include "../GL/GL.h"

MyGame::MyGame()
{
	Camera* cam = CreateCamera<Camera>("Cam1");
	cam->SetPosition(Vector(0, 5, -15));

	//DirectionalLight* light = CreateLightSource<DirectionalLight>("Light");
	//light->SetRotation(Vector(-1.6, 1, 0.5).Normalized());

	Thing* thing = CreateObject<Thing>("Thing");
	thing->SetPosition(Vector(0, 10, 0));
	thing->SetRotation(Vector(0, 0, 0));
	thing->SetScale(Vector(1, 1, 1));

	Box* box = CreateObject<Box>("Floor");
	box->SetPosition(Vector(0, 0, 0));
	box->SetRotation(Vector(0, 0, 0));
	box->SetScale(Vector(10, 1, 10));
}
