#pragma once
#include <AndyOS.h>
#include "GUI.h"
#include "Game.h"
#include "GL.h"

class GEngine
{
public:
	static Game* game;
	static float deltaTime;
	static gui::Window* window;

	static void StartGame(Game* game, gui::Window* wnd);

	static Vector3 WorldToScreen(Vector3& point);
	static void DebugLine(Vector3 start, Vector3 end, Color& color);
	static void DebugBox(Box& box, Color& color);

private:
	static void Update();
	static void Collision();
	static void Render();
};