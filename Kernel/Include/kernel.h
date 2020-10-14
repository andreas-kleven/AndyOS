#pragma once
#include <types.h>
#include <video.h>

namespace Kernel {
void Setup(size_t mem_size, size_t stack_phys, size_t stack_size, VideoMode *video_mode);
};
