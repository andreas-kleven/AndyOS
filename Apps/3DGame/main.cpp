#include <sys/drawing.h>
#include "GUI.h"
#include "GEngine.h"
#include "3DGame.h"

class GameWindow : public Window
{
public:
    GameWindow() 
        : Window("Game")
    {
        GEngine engine(this->gc);
        engine.StartGame(new MyGame());
    }
};

int main()
{
    Drawing::Init();
    GameWindow wnd;
}