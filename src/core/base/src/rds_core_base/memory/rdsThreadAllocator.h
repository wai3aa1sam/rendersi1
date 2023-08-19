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
		RDS_CORE_ASSERT(threadId() == OsTraits::threadLocalId(), "");
		return Mallocator::alloc(n, align);
	}

	void free(void* p, SizeType n = 0)
	{
		RDS_CORE_ASSERT(threadId() == OsTraits::threadLocalId(), "");
		Mallocator::free(p);
	}
	
	void	setThreadId(int id);
	int		threadId() const;

private:
	int _threadId = -1;

	FixedPoolAllocator	_fixedPools[MemoryTraits::s_kBlockCount];
	PoolAllocator		_pool[MemoryTraits::s_kBlockCount];

};

inline void	ThreadAllocator::setThreadId(int id) { RDS_CORE_ASSERT(_threadId == -1, "already set"); _threadId = id; }

inline int	ThreadAllocator::threadId() const { RDS_CORE_ASSERT(_threadId != -1, ""); return _threadId; }

#endif
}

