#pragma once
#include <Drivers/tty.h>
#include <circbuf.h>
#include <driver.h>
#include <sync.h>
#include <types.h>
#include <video.h>

#define ESCAPE_BUFFER_SIZE 32

struct VT100_STATE
{
    int x = 0;
    int y = 0;
    uint32 fg;
    uint32 bg;
    uint32 blink_color = 0;
    bool invert = false;
    bool blink = false;
    bool escaped = false;
    int escape_index = 0;
    char escape_buffer[ESCAPE_BUFFER_SIZE];
};

class Vt100Driver : public TtyBaseDriver
{
  private:
    int width;
    int height;
    CircularDataBuffer *text_buffer;
    bool active;
    Mutex draw_mutex;
    VT100_STATE state;
    VideoMode video;

  public:
    Vt100Driver();
    ~Vt100Driver();

    int Open();
    int Close();
    int Write(const void *buf, size_t size);
    int HandleInput(const char *input, size_t size);
    void Activate();
    void Deactivate();
    void Blink(bool blink);

  private:
    void ClearScreen();
    void ClearLine(int line);
    void RedrawScreen();
    int AddText(const char *text, size_t size);
    bool ParseSequence(char cmd);
    bool HandleSequence(char cmd, int num_args, int *args);
    void DrawText();
    void DrawChar(int x, int y, int c);
    void InvertColors(int x, int y);
    int GetScreenColors(int x, int y, uint32 &fg, uint32 &bg);
    void Putc(char c);
    void Move(int x, int y);
    uint32 GetColor(int number);
    uint32 GetColor(int r, int g, int b);
    void ResetColors();
};
