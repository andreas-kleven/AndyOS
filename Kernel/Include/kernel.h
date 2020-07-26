#pragma once
#include <types.h>
#include <video.h>

namespace Kernel
{
	void Setup(size_t mem_start, size_t mem_end, VideoMode* video_mode);
};