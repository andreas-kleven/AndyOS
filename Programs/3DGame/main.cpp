#include "MyGame.h"
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

int main(int argc, char **argv)
{
    bool host = false;
    bool client = false;

    char **arg = argv;

    while (*++arg) {
        host |= strcmp(*arg, "-h") == 0 || strcmp(*arg, "h") == 0;
        client |= strcmp(*arg, "-c") == 0 || strcmp(*arg, "c") == 0;
    }

    GameWindow wnd;
    GL::Init();
    MyGame game(host, client);
    GEngine::StartGame(&game, &wnd);
    return 0;
}
