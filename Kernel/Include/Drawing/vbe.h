#pragma once
#include <types.h>
#include <video.h>

struct VBE_MODE_INFO
{
    uint16 attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the
                        // mode supports a linear frame buffer.
    uint8 window_a;     // deprecated
    uint8 window_b;     // deprecated
    uint16 granularity; // deprecated; used while calculating bank numbers
    uint16 window_size;
    uint16 segment_a;
    uint16 segment_b;
    uint32 win_func_ptr; // deprecated; used to switch banks from protected mode without returning
                         // to real mode
    uint16 pitch;        // number of bytes per horizontal line
    uint16 width;        // width in pixels
    uint16 height;       // height in pixels
    uint8 w_char;        // unused...
    uint8 y_char;        // ...
    uint8 planes;
    uint8 bpp;   // bits per pixel in this mode
    uint8 banks; // deprecated; total number of banks in this mode
    uint8 memory_model;
    uint8 bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8 image_pages;
    uint8 reserved0;

    uint8 red_mask;
    uint8 red_position;
    uint8 green_mask;
    uint8 green_position;
    uint8 blue_mask;
    uint8 blue_position;
    uint8 reserved_mask;
    uint8 reserved_position;
    uint8 direct_color_attributes;

    uint32
        *framebuffer; // physical address of the linear frame buffer; write here to draw to the gc
    uint32 off_screen_mem_off;
    uint16
        off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the gc
    uint8 reserved1[206];
} __attribute__((packed));

class VBEVideoMode : public VideoMode
{
  private:
    VBE_MODE_INFO info;

  public:
    VBEVideoMode(VBE_MODE_INFO *info);

    void Draw(void *pixels);
    void SetPixel(int x, int y, unsigned int col);
};