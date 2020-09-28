#include "3DGame.h"
#include "GEngine.h"
#include "GUI.h"
#include <andyos/drawing.h>
#include <unistd.h>

using namespace gui;

class GameWindow : public Window
{
  public:
    GameWindow() : Window("Game", 600, 400, Color::Black) { SetCapture(true); }

    void OnClose() { exit(0); }
};

int main()
{
    GameWindow wnd;
    GL::Init();
    MyGame game;
    GEngine::StartGame(&game, &wnd);
    return 0;
}
