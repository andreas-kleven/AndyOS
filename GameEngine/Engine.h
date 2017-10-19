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

	GEngine(GC gc);

	void StartGame(Game* game);

private:
	List<Component*> all_components;
	List<MeshComponent*> meshComponents;
	List<Rigidbody*> rigidbodies;
	List<Component*> physics_components;

	void Update();
	void Collision();
	void Render();
};