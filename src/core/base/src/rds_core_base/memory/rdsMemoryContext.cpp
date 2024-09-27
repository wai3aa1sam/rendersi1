#include "rds_core_base-pch.h"

#include "rdsMemoryContext.h"

#include "rds_core_base/job_system/rds_job_system.h"

namespace rds 
{
#if 0
#pragma mark --- MemoryContext-Impl ---
#endif // 0
#if 1

MemoryContext* MemoryContext::s_instance = nullptr;

void MemoryContext::init()
{
	RDS_MALLOC_NEW(MemoryContext)();
}

void MemoryContext::terminate()
{
	RDS_MALLOC_DELETE(instance());
}

MemoryContext::MemoryContext()
	: Base()
{
	OsTraits::setMainThread();
	#if 1
	_allocStacks.resize(s_kThreadCount);
	_defaultAllocators.resize(s_kThreadCount);

	for (size_t i = 0; i < _allocStacks.size(); i++)
	{
		_allocStacks[i].emplace_back(&_defaultAllocators[i]);
	}

	for (int i = 0; i < s_kThreadCount; i++)
	{
		_defaultAllocators[i].setThreadId(i);
	}

	_log("TODO: ThreadAllocatorStack also need a thread id to check sanity");
	#endif // 0
}

MemoryContext::~MemoryContext()
{
	RDS_CORE_ASSERT(OsTraits::isMainThread(), "");
	OsTraits::resetThreadLocalId();
}

void*	
MemoryContext::alloc(SizeType n, SizeType align, SizeType offset)
{
	return curAllocator()->alloc(n, align, offset);
}

void	
MemoryContext::free	(void* p, SizeType n)
{
	return curAllocator()->free(p, n);
}

bool 
MemoryContext::is_owning(void* p, SizeType n) const
{
	return curAllocator()->is_owning(p, n);
}

void 
MemoryContext::pushAllocator(IAllocator* p)
{
	RDS_CORE_ASSERT(threadAllocStack().size() > 1, "can not pop DefaultAllocator");
	threadAllocStack().push_back(p);
}

void 
MemoryContext::popAllocator()
{
	RDS_CORE_ASSERT(threadAllocStack().size() > 1, "can not pop DefaultAllocator");
	threadAllocStack().pop_back();
}

#endif
}

#if RDS_OVERRIDE_NEW_OP

void*	operator new  (size_t size)
{
	void* p = RDS_ALLOC(size);
	return p;
}

void*	operator new[](size_t size)
{
	void* p = RDS_ALLOC(size);
	return p;
}

void*	operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	void* p = RDS_ALLOC(size);
	return p;
}

void*	operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	void* p = RDS_ALLOC_ALIGNED(size, alignment);
	return p;
}

void	operator delete  (void* ptr)					RDS_NOEXCEPT
{
	RDS_FREE(ptr);
}

void	operator delete  (void* ptr, std::size_t size)	RDS_NOEXCEPT
{
	RDS_FREE(ptr, size);
}

void	operator delete[](void* ptr)					RDS_NOEXCEPT
{
	RDS_FREE(ptr);
}

void	operator delete[](void* ptr, std::size_t size)	RDS_NOEXCEPT
{
	RDS_FREE(ptr, size);
}

#endif