#include <string>
#include "GEngine.h"
#include "Game.h"

Game::Game()
{
	active_cam = 0;
}

void Game::AddObject(GameObject* object)
{
	objects.push_back(object);
}

void Game::AddCamera(Camera* cam)
{
	cameras.push_back(cam);

	if (!active_cam)
		SetActiveCamera(cam);
}

void Game::AddLightSource(LightSource* light)
{
	lights.push_back(light);
}

Camera* Game::GetActiveCamera()
{
	return active_cam;
}

void Game::SetActiveCamera(Camera* cam)
{
	active_cam = cam;
}

GameObject* Game::GetObject(const std::string& name)
{
	for (int i = 0; i < objects.size(); i++)
	{
		printf(objects[i]->GetName().c_str());

		GameObject* obj = objects[i];
		if (obj->GetName() == name)
			return obj;
	}

	return 0;
}

Camera* Game::GetCamera(const std::string& name)
{
	for (int i = 0; i < cameras.size(); i++)
	{
		Camera* cam = cameras[i];
		if (cam->GetName() == name)
			return cam;
	}

	return 0;
}

LightSource* Game::GetLightSource(const std::string& name)
{
	for (int i = 0; i < lights.size(); i++)
	{
		LightSource* light = lights[i];
		if (light->GetName() == name)
			return light;
	}

	return 0;
}

