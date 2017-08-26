#pragma once

#include "Vector.h"
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
	float delta_time;

	GEngine();

	void StartGame(Game* game);

private:
	std::List<Component*> all_components;
	std::List<MeshComponent*> mesh_components;
	std::List<ColliderComponent*> collider_components;
	std::List<Component*> physics_components;

	void Update();
	void Collision();
	void Render();
};