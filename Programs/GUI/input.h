#pragma once
#include <AndyOS.h>

#define DOUBLE_CLICK_TIME 200

namespace gui {
struct KEY_PACKET
{
    KEYCODE code;
    char character;
    bool pressed;
    bool shift;
    bool alt;
    bool ctrl;
    bool caps;
    bool scroll;
    bool numlock;
};

class InputManager
{
  public:
    static void HandleKey(KEYCODE code, bool pressed);
    static KEY_PACKET GetPacket();
    static bool GetKeyDown(KEYCODE code);

    static void HandleMouse(int dx, int dy);
    static void GetMouseDelta(int &dx, int &dy);

  private:
    static void DecodeCharacter(KEY_PACKET &packet);
};
} // namespace gui
