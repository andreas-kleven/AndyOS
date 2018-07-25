#include "Arch/mvideo.h"
#include "Arch/uart.h"
#include "Arch/mailbox.h"
#include "string.h"

MVideoMode::MVideoMode(int width, int height, int depth)
{
    mbox[0] = 35 * 4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003; //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = width; //FrameBufferInfo.width
    mbox[6] = height; //FrameBufferInfo.height

    mbox[7] = 0x48004; //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = width; //FrameBufferInfo.virtual_width
    mbox[11] = height; //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0; //FrameBufferInfo.x_offset
    mbox[16] = 0; //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = depth; //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 0; //BGR, not RGB preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096; //FrameBufferInfo.pointer
    mbox[29] = 0; //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0; //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    if (Mailbox::Call(MBOX_CH_PROP) && mbox[20] == depth && mbox[28] != 0)
    {
        this->width = mbox[5];
        this->height = mbox[6];
        this->depth = mbox[20];
        
        mbox[28] &= 0x3FFFFFFF;
        this->framebuffer_phys = (void*)mbox[28];
        this->framebuffer = this->framebuffer_phys;
    } 
    else
    {
        uart_puts("Unable to set screen resolution\n");
    }
}

void MVideoMode::Draw(void* pixels)
{
	memcpy(framebuffer, pixels, memsize);
}

void MVideoMode::SetPixel(int x, int y, unsigned int col)
{
    unsigned int* a = (unsigned int*)framebuffer + y * width + x;
	*a = col;
}