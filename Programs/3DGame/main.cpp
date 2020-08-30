#include "3DGame.h"
#include "GEngine.h"
#include "GUI.h"
#include <andyos/drawing.h>

using namespace gui;

class GameWindow : public Window
{
  public:
    GameWindow() : Window("Game", 600, 400, Color::Black)
    {
        SetCapture(true);
        GEngine::StartGame(new MyGame(), this);
    }

    void OnClose() { exit(0); }
};

int main()
{
    GameWindow wnd;
    return 0;
}