#pragma once

#define BLOCK_SIZE	0x1000
#define BYTES_TO_BLOCKS(x)	((1 + (((x) - 1) / BLOCK_SIZE)) * ((x) != 0))
