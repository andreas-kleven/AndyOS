#pragma once
#include "GL.h"
#include "GUI.h"
#include "Game.h"
#include <AndyOS.h>

namespace GEngine {
extern Game *game;
extern float deltaTime;
extern gui::Window *window;

Vector3 WorldToScreen(Vector3 &point);
void DebugLine(Vector3 start, Vector3 end, Color &color);
void DebugBox(Box &box, Color &color);

void StartGame(Game *game, gui::Window *wnd);
} // namespace GEngine