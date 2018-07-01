#include "pmem.h"
#include "memory.h"

namespace PMem
{
	uint32 mem_size;
	uint32* mem_map;

	uint32 num_blocks;
	uint32 num_free;

	inline void SetBit(uint32 bit)
	{
		mem_map[bit / 32] |= (1 << (bit % 32));
	}

	inline void UnsetBit(uint32 bit)
	{
		mem_map[bit / 32] &= ~(1 << (bit % 32));
	}

	inline uint8 GetBit(uint32 bit)
	{
		return (mem_map[bit / 32] >> (bit % 32)) & 1;
	}

	uint32 FirstFree()
	{
		for (int i = 0; i < num_blocks; i++)
			if (!GetBit(i))
				return i;

		return 0;
	}

	uint32 FirstFreeNum(uint32 size)
	{
		if (size == 0)
			return 0;

		if (size == 1)
			return FirstFree();

		int i, j;

		for (i = 1; i <= num_blocks - size; i++)
		{
			if (!GetBit(i))
			{
				bool found = true;

				for (j = 1; j < size; j++)
				{
					if (GetBit(i + j))
						found = false;
				}

				if (found)
					return i;
				else
					i += j;
			}
		}

		return 0;
	}

	void InitRegion(void* addr, uint32 size)
	{
		int align = (uint32)addr / BLOCK_SIZE;
		int num = size / BLOCK_SIZE;

		for (int i = 0; i < num; i++)
		{
			UnsetBit(align++);
			num_free++;
		}

		SetBit(0);
	}

	void DeInitRegion(void* addr, uint32 size)
	{
		int align = (uint32)addr / BLOCK_SIZE;
		int blocks = size / BLOCK_SIZE;

		for (int i = 0; i < blocks; i++)
		{
			SetBit(align++);
			num_free--;
		}

		SetBit(0);
	}

	void* AllocBlocks(uint32 size)
	{
		uint32 frame = FirstFreeNum(size);

		if (frame == 0)
			return 0;

		for (int i = 0; i < size; i++)
		{
			SetBit(frame + i);
			num_free--;
		}

		return (void*)(frame * BLOCK_SIZE);
	}

	void FreeBlocks(void* addr, uint32 size)
	{
		int frame = (uint32)addr / BLOCK_SIZE;

		if (frame == 0 || size == 0)
			return;

		for (int i = 0; i < size; i++)
		{
			if (GetBit(frame + i))
			{
				UnsetBit(frame + i);
				num_free++;
			}
		}
	}

	STATUS Init(uint32 size, uint32* map)
	{
		mem_size = size;
		mem_map = map;

		num_blocks = BYTES_TO_BLOCKS(mem_size);
		num_free = num_blocks;

		DeInitRegion(0, mem_size);
		return STATUS_SUCCESS;
	}
}