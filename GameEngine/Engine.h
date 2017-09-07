#pragma once
#include "Vector3.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "Game.h"
#include "MeshComponent.h"
#include "List.h"

class GEngine
{
public:
	Game* active_game;
	float deltaTime;

	GEngine();

	void StartGame(Game* game);

private:
	std::List<Component*> all_components;
	std::List<MeshComponent*> meshComponents;
	std::List<Rigidbody*> rigidbodies;
	std::List<Component*> physics_components;

	void Update();
	void Collision();
	void Render();
};