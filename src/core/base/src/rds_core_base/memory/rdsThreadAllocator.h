#pragma once

#include "rds_memory_common.h"

namespace rds
{
#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

class ThreadAllocator : public IAllocator
{
public:
	using Base = IAllocator;

public:
	ThreadAllocator(const char* name = "ThreadAllocator") : Base(name) {};
	virtual ~ThreadAllocator()	= default;

	void* alloc(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0)
	{
		return Mallocator::alloc(n, align);
	}

	void free(void* p, SizeType n = 0)
	{
		Mallocator::free(p);
	}
	

private:
	FixedPoolAllocator	_fixedPools[MemoryTraits::s_kBlockCount];
	PoolAllocator		_pool[MemoryTraits::s_kBlockCount];

};

#endif
}

