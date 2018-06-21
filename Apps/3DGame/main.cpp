#include "GEngine.h"
#include "3DGame.h"
#include "AndyOS.h"

int main()
{
    sleep(1000);
    Drawing::Init();
    GC* gc = new GC(1024, 768);
    GEngine engine(*gc);
    engine.StartGame(new MyGame());
}