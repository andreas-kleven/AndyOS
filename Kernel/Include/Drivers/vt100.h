#pragma once
#include <Drivers/tty.h>
#include <circbuf.h>
#include <driver.h>
#include <sync.h>
#include <types.h>

#define ESCAPE_BUFFER_SIZE 32

struct VT100_STATE
{
    int x = 0;
    int y = 0;
    uint32 fg;
    uint32 bg;
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
    bool escaped;
    char escape_buffer[ESCAPE_BUFFER_SIZE];
    int escape_index;

  public:
    Vt100Driver();
    ~Vt100Driver();

    int Open();
    int Close();
    int Write(const void *buf, size_t size);
    int HandleInput(const char *input, size_t size);
    void Activate();
    void Deactivate();

  private:
    void ClearScreen();
    void RedrawScreen();
    int AddText(const char *text, size_t size);
    void ParseSequence(char cmd);
    void HandleSequence(char cmd, int num_args, int *args);
    void DrawText();
    void DrawChar(int x, int y, char c);
    void Putc(char c);
    uint32 GetColor(int number);
    uint32 GetColor(int r, int g, int b);
    void ResetColors();
};
