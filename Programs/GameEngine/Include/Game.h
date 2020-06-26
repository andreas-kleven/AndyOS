#pragma once
#include <vector>
#include "Scene.h"

class Game
{
public:
	//Scene* scene;
	std::vector<GameObject*> objects;
	std::vector<Camera*> cameras;
	std::vector<LightSource*> lights;

	Game();

	void AddObject(GameObject* object);
	void AddCamera(Camera* cam);
	void AddLightSource(LightSource* light);

	Camera* GetActiveCamera();
	void SetActiveCamera(Camera* cam);

	GameObject* GetObject(const std::string& name);
	Camera* GetCamera(const std::string& name);
	LightSource* GetLightSource(const std::string& name);

	template<class T>
	T* CreateObject(const std::string& name);

	template<class T>
	T* CreateCamera(const std::string& name);

	template<class T>
	T* CreateLightSource(const std::string& name);

private:
	Camera* active_cam;
};


//Creates an object and adds it
template<class T>
T* Game::CreateObject(const std::string& name)
{
	GameObject* t = new T;
	t->SetName(name);
	AddObject(t);
	return (T*)t;
}

//Creates a camera and adds it
template<class T>
T* Game::CreateCamera(const std::string& name)
{
	Camera* t = new T;
	t->SetName(name);
	AddCamera(t);
	return (T*)t;
}

//Creates a light source and adds it
template<class T>
T* Game::CreateLightSource(const std::string& name)
{
	LightSource* t = new T;
	t->SetName(name);
	AddLightSource(t);
	return (T*)t;
}