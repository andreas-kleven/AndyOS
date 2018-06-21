#include <Drawing.h>
#include "GEngine.h"
#include "3DGame.h"

int main()
{
    Drawing::Init();
    GC* gc = new GC(1024, 768);
    GEngine engine(*gc);
    engine.StartGame(new MyGame());
}