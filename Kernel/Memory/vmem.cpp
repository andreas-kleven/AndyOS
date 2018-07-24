#include "vmem.h"
#include "memory.h"
#include "Arch/memory.h"
#include "Kernel/kernel.h"
#include "string.h"
#include "Lib/debug.h"
#include "Process/scheduler.h"

namespace VMem
{
	void* MapFirstFree(void* phys, size_t count, pflags_t flags, size_t start, size_t end)
	{
		void* virt = FirstFree(count, start, end);

		if (!virt)
			return 0;

		if (MapPages(virt, phys, count, flags))
		{
			PMem::DeInitRegion(phys, count * PAGE_SIZE);
			return (void*)virt;
		}

		return 0;
	}
	
	void* Alloc(size_t count, pflags_t flags, size_t start, size_t end)
	{
		Scheduler::Disable();

		void* virt = FirstFree(count, start, end);
		void* phys = PMem::AllocBlocks(count);
		MapPages(virt, phys, count, flags);

		Scheduler::Enable();
		return virt;

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

	void* KernelAlloc(size_t count)
	{
		return Alloc(count, PAGE_PRESENT | PAGE_WRITE, KERNEL_BASE, KERNEL_END);
	}

	void* UserAlloc(size_t count)
	{
		return Alloc(count, PAGE_PRESENT | PAGE_WRITE | PAGE_USER, USER_BASE, USER_END);
	}

	void* KernelMapFirstFree(void* phys, size_t count, pflags_t flags)
	{
		return MapFirstFree(phys, count, flags, KERNEL_BASE, KERNEL_END);
	}

	void* UserMapFirstFree(void* phys, size_t count, pflags_t flags)
	{
		return MapFirstFree(phys, count, flags, USER_BASE, USER_END);
	}

	bool UserAllocShared(ADDRESS_SPACE other_space, void*& addr1, void*& addr2, size_t count)
	{
		Scheduler::Disable();
		ADDRESS_SPACE cur_space = GetAddressSpace();

		void* phys = PMem::AllocBlocks(count);
		pflags_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;

		addr1 = UserMapFirstFree(phys, count, flags);

		SwapAddressSpace(other_space);
		void* _addr2 = UserMapFirstFree(phys, count, flags);

		//Switch back
		SwapAddressSpace(cur_space);
		addr2 = _addr2;

		Scheduler::Enable();
		return true;
	}

	//

	ADDRESS_SPACE GetAddressSpace()
	{
		return Arch::GetAddressSpace();
	}

    bool SwapAddressSpace(ADDRESS_SPACE& space)
	{
		Scheduler::Disable();
		bool ret = Arch::SwapAddressSpace(space);
		Scheduler::Enable();
		return ret;
	}

    ADDRESS_SPACE CreateAddressSpace()
	{
		Scheduler::Disable();
		ADDRESS_SPACE ret = Arch::CreateAddressSpace();
		Scheduler::Enable();
		return ret;
	}

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

    void* FirstFree(size_t count, size_t start, size_t end)
	{
		Scheduler::Disable();
		void* ret = Arch::FirstFree(count, start, end);
		Scheduler::Enable();
		return ret;
	}

    bool MapPages(void* virt, void* phys, size_t count, pflags_t flags)
	{
		Scheduler::Disable();
		bool ret = Arch::MapPages(virt, phys, count, flags);
		Scheduler::Enable();
		return ret;
	}

    bool FreePages(void* virt, size_t count)
	{
		Scheduler::Disable();
		bool ret = Arch::FreePages(virt, count);
		Scheduler::Enable();
		return ret;
	}

    bool Init()
	{
		if (!Arch::Init())
			return false;

		return true;
	}
}