#pragma once
#include "Scene.h"

class Game
{
public:
	//Scene* scene;
	List<GameObject*> objects;
	List<Camera*> cameras;
	List<LightSource*> lights;

	Game();

	void AddObject(GameObject* object);
	void AddCamera(Camera* cam);
	void AddLightSource(LightSource* light);

	Camera* GetActiveCamera();
	void SetActiveCamera(Camera* cam);

	GameObject* GetObject(String name);
	Camera* GetCamera(String name);
	LightSource* GetLightSource(String name);

	template<class T>
	T* CreateObject(String name);

	template<class T>
	T* CreateCamera(String name);

	template<class T>
	T* CreateLightSource(String name);

private:
	Camera* active_cam;
};


//Creates an object and adds it
template<class T>
T* Game::CreateObject(String name)
{
	GameObject* t = new T;
	t->SetName(name);
	AddObject(t);
	return (T*)t;
}

//Creates a camera and adds it
template<class T>
T* Game::CreateCamera(String name)
{
	Camera* t = new T;
	t->SetName(name);
	AddCamera(t);
	return (T*)t;
}

//Creates a light source and adds it
template<class T>
T* Game::CreateLightSource(String name)
{
	LightSource* t = new T;
	t->SetName(name);
	AddLightSource(t);
	return (T*)t;
}