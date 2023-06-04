#include "rds_core_base-pch.h"

#include "rdsMemoryContext.h"


namespace rds 
{
#if 0
#pragma mark --- MemoryContext-Impl ---
#endif // 0
#if 1

MemoryContext* MemoryContext::s_instance = nullptr;

MemoryContext::MemoryContext()
{
	_allocStacks.resize(s_kThreadCount);
	_defaultAllocators.resize(s_kThreadCount);

	for (size_t i = 0; i < _allocStacks.size(); i++)
	{
		_allocStacks[i].emplace_back(&_defaultAllocators[i]);
	}
}

MemoryContext::~MemoryContext()
{
	destroy();
}

void 
MemoryContext::create()
{
	destroy();
	OsTraits::setThreadLocalId(OsTraits::s_kMainThreadLocalId);

	auto* p = sCast<MemoryContext*>(Mallocator::alloc(sizeof(MemoryContext), RDS_ALIGN_OF(MemoryContext)));
	new (p) MemoryContext();
	Base::create(p);

	
}

void 
MemoryContext::destroy()
{
	Mallocator::free(s_instance);
	Base::destroy();

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