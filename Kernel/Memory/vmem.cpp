#include <memory.h>
#include <Arch/memory.h>
#include <kernel.h>
#include <string.h>
#include <debug.h>
#include <Process/scheduler.h>

namespace VMem
{
	void *MapFirstFree(void *phys, size_t count, pflags_t flags, size_t start, size_t end)
	{
		Scheduler::Disable();
		void *virt = FirstFree(count, start, end);

		if (!virt)
		{
			Scheduler::Enable();
			return 0;
		}

		if (MapPages(virt, phys, count, flags))
		{
			Scheduler::Enable();
			return (void *)virt;
		}

		Scheduler::Enable();
		return 0;
	}

	void *Alloc(size_t count, pflags_t flags, size_t start, size_t end, bool contiguous)
	{
		Scheduler::Disable();

		if (contiguous || true)
		{
			void *virt = FirstFree(count, start, end);

			if (virt == 0)
			{
				Scheduler::Enable();
				return 0;
			}

			void *phys = PMem::AllocBlocks(count);

			if (!phys)
			{
				Scheduler::Enable();
				return 0;
			}

			if (!MapPages(virt, phys, count, flags))
			{
				PMem::FreeBlocks(phys, count);
				Scheduler::Enable();
				return 0;
			}

			Scheduler::Enable();
			return virt;
		}

		//
		/*char* _virt = (char*)virt;

		if (!virt)
			return 0;

		for (int i = 0; i < count; i++)
		{
			void* phys = PMem::AllocBlocks(1);

			if (!phys)
				return 0;

			if (!MapPages(_virt, phys, 1, flags))
				return 0;

			_virt += PAGE_SIZE;
		}

		return virt;*/
	}

	///

	size_t GetAddress(size_t virt)
	{
		Scheduler::Disable();
		size_t ret = Arch::GetAddress(virt);
		Scheduler::Enable();
		return ret;
	}

	pflags_t GetFlags(size_t virt)
	{
		Scheduler::Disable();
		pflags_t ret = Arch::GetFlags(virt);
		Scheduler::Enable();
		return ret;
	}

	ADDRESS_SPACE GetAddressSpace()
	{
		return Arch::GetAddressSpace();
	}

	bool SwapAddressSpace(ADDRESS_SPACE &space)
	{
		Scheduler::Disable();
		bool ret = Arch::SwapAddressSpace(space);
		Scheduler::Enable();
		return ret;
	}

	bool CreateAddressSpace(ADDRESS_SPACE &space)
	{
		Scheduler::Disable();
		bool ret = Arch::CreateAddressSpace(space);
		Scheduler::Enable();
		return ret;
	}

	bool CopyAddressSpace(ADDRESS_SPACE &space)
	{
		Scheduler::Disable();
		bool ret = Arch::CopyAddressSpace(space);
		Scheduler::Enable();
		return ret;
	}

	void *FirstFree(size_t count, size_t start, size_t end)
	{
		Scheduler::Disable();

		void *ret = Arch::FirstFree(count, start, end);

		if (ret == 0)
			debug_print("Out of virtual memory %p %p %p\n", count, start, end);

		Scheduler::Enable();
		return ret;
	}

	bool MapPages(void *virt, void *phys, size_t count, pflags_t flags)
	{
		Scheduler::Disable();

		if (virt == 0 || phys == 0)
		{
			debug_print("Map address 0: %d, %p -> %p\n", virt, phys, flags);
			return false;
		}

		for (size_t i = 0; i < count; i++)
		{
			size_t addr = (size_t)virt + i * PAGE_SIZE;
			if (GetFlags(addr) & PAGE_PRESENT)
				debug_print("Page already mapped %p, %x, %p -> %p\n", GetAddressSpace().ptr, GetFlags(addr), addr, GetAddress(addr));
		}

		bool ret = Arch::MapPages(virt, phys, count, flags);
		Scheduler::Enable();
		return ret;
	}

	bool FreePages(void *virt, size_t count)
	{
		Scheduler::Disable();
		bool ret = Arch::FreePages(virt, count);
		Scheduler::Enable();
		return ret;
	}

	void *KernelAlloc(size_t count, bool contiguous)
	{
		return Alloc(count, PAGE_PRESENT | PAGE_WRITE, KERNEL_BASE, KERNEL_END, contiguous);
	}

	void *UserAlloc(size_t count, bool contiguous)
	{
		return Alloc(count, PAGE_PRESENT | PAGE_WRITE | PAGE_USER, HEAP_END, USER_END, contiguous);
	}

	void *KernelMapFirstFree(void *phys, size_t count, pflags_t flags)
	{
		return MapFirstFree(phys, count, flags, KERNEL_BASE, KERNEL_END);
	}

	void *UserMapFirstFree(void *phys, size_t count, pflags_t flags)
	{
		return MapFirstFree(phys, count, flags, HEAP_END, USER_END);
	}

	bool UserAllocShared(ADDRESS_SPACE other_space, void *&addr1, void *&addr2, size_t count)
	{
		Scheduler::Disable();
		ADDRESS_SPACE cur_space = GetAddressSpace();

		void *phys = PMem::AllocBlocks(count);
		pflags_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;

		addr1 = UserMapFirstFree(phys, count, flags);

		SwapAddressSpace(other_space);
		void *_addr2 = UserMapFirstFree(phys, count, flags);

		//Switch back
		SwapAddressSpace(cur_space);
		addr2 = _addr2;

		Scheduler::Enable();
		return true;
	}

	bool Init()
	{
		if (!Arch::Init())
			return false;

		return true;
	}
} // namespace VMem