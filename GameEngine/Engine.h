#pragma once
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "Game.h"
#include "MeshComponent.h"
#include "List.h"
#include "Drawing/drawing.h"
#include "../GL/GL.h"

class GEngine
{
public:
	Game* active_game;
	float deltaTime;

	GEngine(GC gc);

	void StartGame(Game* game);

	static Vector3 WorldToScreen(Game* game, Vector3& point);
	static void DebugLine(Game* game, Vector3& start, Vector3& end, ColRGB& color);
	static void DebugBox(Game* game, Box& box, ColRGB& color);

private:
	List<Component*> all_components;
	List<MeshComponent*> meshComponents;
	List<Rigidbody*> rigidbodies;
	List<Component*> physics_components;

	void Update();
	void Collision();
	void Render();
};