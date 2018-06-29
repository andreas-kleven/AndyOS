#include <sys/drawing.h>
#include "GUI.h"
#include "GEngine.h"
#include "3DGame.h"

using namespace gui;

class GameWindow : public Window
{
public:
    GameWindow() 
        : Window("Game", 600, 400)
    {
        SetCapture(true);

        GEngine engine(this->gc);
        engine.StartGame(new MyGame());
    }
};

int main()
{
    Drawing::Init();
    GameWindow wnd;

    exit(0);
}