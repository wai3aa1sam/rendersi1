#include "rds_core_base-pch.h"
#include "rdsDefaultAllocator.h"

namespace rds
{
#if 0
#pragma mark --- rdsDefaultAllocator-Impl ---
#endif // 0
#if 1

MemoryArena<DefaultAllocator_Impl, void> DefaultAllocator::s_instance;

DefaultAllocator_Impl::DefaultAllocator_Impl(const char* name)
	: Base(name)
{
	_nmspAllocationCallback.allocCallback = [](size_t n, size_t align, size_t offset) -> void*
	{
		return DefaultAllocator_Impl::alloc(n, align, offset);
	};

	_nmspAllocationCallback.freeCallback = [](void* p, size_t n)
	{
		DefaultAllocator_Impl::free(p, n);
	};
}

#endif
}