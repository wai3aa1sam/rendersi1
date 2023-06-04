#pragma once

#include "rds_memory_common.h"
#include "rdsThreadAllocator.h"

#include "rds_core_base/utility/rdsSingleton.h"

namespace rds
{

#if 0
#pragma mark --- MemoryContext-Decl ---
#endif // 0
#if 1

class MemoryContext : public Singleton<MemoryContext>
{
public:
	using Base = Singleton<MemoryContext>;

	using SizeType = MemoryTraits::SizeType;

	template<class T, size_t N = 0> using Vector	= ::nmsp::Vector_T<T, N, Mallocator>;
	template<class T, size_t N = 0> using Stack		= ::nmsp::Vector_T<T, N, Mallocator>;

	using ThreadAllocStack = Stack<IAllocator*, 64>;

	static constexpr SizeType s_kThreadCount = CoreBaseTraits::s_kLogicalThreadCount;
	using AllThreadsAllocStack		= Vector	<ThreadAllocStack, s_kThreadCount>;
	using AllThreadsDefaultAlloc	= Vector	<ThreadAllocator , s_kThreadCount>;

public:
	MemoryContext();
	~MemoryContext();

	void create();
	void destroy();

	void*	alloc	(SizeType n, SizeType align = MemoryTraits::s_kDefaultAlign, SizeType offset = 0);
	void	free	(void* p, SizeType n = 0);

	bool is_owning(void* p, SizeType n) const;
	const char* name() const;

	void pushAllocator(IAllocator* p);
	void popAllocator();

			IAllocator* curAllocator();
	const	IAllocator* curAllocator() const;

			/*ThreadAllocator&  defaultAllocator();
	const	ThreadAllocator&  defaultAllocator() const;*/

private:
			ThreadAllocStack& threadAllocStack();
	const	ThreadAllocStack& threadAllocStack() const;


private:
	AllThreadsAllocStack	_allocStacks;
	AllThreadsDefaultAlloc	_defaultAllocators;
};


#endif



#if 0
#pragma mark --- MemoryContext-Decl ---
#endif // 0
#if 1

inline IAllocator* 
MemoryContext::curAllocator()
{
	return threadAllocStack().back();
}

inline const	IAllocator* 
MemoryContext::curAllocator() const
{
	return threadAllocStack().back();
}

inline const char* 
MemoryContext::name() const
{
	return curAllocator()->name();
}

inline MemoryContext::ThreadAllocStack& 
MemoryContext::threadAllocStack()
{
	auto threadLocalId = OsTraits::threadLocalId();
	return _allocStacks[threadLocalId];
}

inline const MemoryContext::ThreadAllocStack& 
MemoryContext::threadAllocStack() const
{
	auto threadLocalId = OsTraits::threadLocalId();
	return _allocStacks[threadLocalId];
}

#if 0
inline const char* 
MemoryContext::name() const
{
	const auto& allocStack = threadAllocStack();
	if (!allocStack.is_empty())
	{
		return allocStack.front().name();
	}

	const auto& defaultAlloc = defaultAllocator();
	return defaultAlloc.name();
}

inline ThreadAllocator&  
MemoryContext::defaultAllocator()
{
	auto threadLocalId = OsTraits::threadLocalId();
	return _defaultAllocators[threadLocalId];
}

inline const ThreadAllocator&  
MemoryContext::defaultAllocator() const
{
	auto threadLocalId = OsTraits::threadLocalId();
	return _defaultAllocators[threadLocalId];
}

#endif // 0

#endif

}