#pragma once
#include "Scene.h"

class Game
{
public:
	//Scene* scene;
	std::List<GameObject*> objects;
	std::List<Camera*> cameras;
	std::List<LightSource*> lights;

	Game();

	void AddObject(GameObject* object);
	void AddCamera(Camera* cam);
	void AddLightSource(LightSource* light);

	Camera* GetActiveCamera();
	void SetActiveCamera(Camera* cam);

	GameObject* GetObject(std::String name);
	Camera* GetCamera(std::String name);
	LightSource* GetLightSource(std::String name);

	template<class T>
	T* CreateObject(std::String name);

	template<class T>
	T* CreateCamera(std::String name);

	template<class T>
	T* CreateLightSource(std::String name);

private:
	Camera* active_cam;
};


//Creates an object and adds it
template<class T>
T* Game::CreateObject(std::String name)
{
	GameObject* t = new T;
	t->SetName(name);
	AddObject(t);
	return (T*)t;
}

//Creates a camera and adds it
template<class T>
T* Game::CreateCamera(std::String name)
{
	Camera* t = new T;
	t->SetName(name);
	AddCamera(t);
	return (T*)t;
}

//Creates a light source and adds it
template<class T>
T* Game::CreateLightSource(std::String name)
{
	LightSource* t = new T;
	t->SetName(name);
	AddLightSource(t);
	return (T*)t;
}