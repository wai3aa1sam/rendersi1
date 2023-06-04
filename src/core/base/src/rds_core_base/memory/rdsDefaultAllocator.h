#pragma once

#include "rds_memory_common.h"
#include "rdsMemoryContext.h"

namespace rds
{
#if 0
#pragma mark --- rdsDefaultAllocator-Impl ---
#endif // 0
#if 1

class DefaultAllocator_Impl : public Allocator_Base<DefaultAllocator_Impl>
{
public:
	using Base = Allocator_Base<DefaultAllocator_Impl>;

public:
	DefaultAllocator_Impl(const char* name = "DefaultAllocator_Impl");
	~DefaultAllocator_Impl()	= default;

	static void*	alloc	(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0) { return MemoryContext::instance()->alloc(n, align, offset); };
	static void		free	(void* p, SizeType n = 0) { MemoryContext::instance()->free(p, n); }

	static const char* name() { return MemoryContext::instance()->name(); }

	static bool is_owning(void* p, SizeType n) { RDS_CORE_ASSERT(false); return false; };

private:
};

class DefaultAllocator : public Allocator_Base<DefaultAllocator>
{
public:
	using Base = Allocator_Base<DefaultAllocator>;

public:
	DefaultAllocator(const char* name = "DefaultAllocator") : Base(name) {};
	~DefaultAllocator()	= default;

	static void*	alloc	(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0) { return s_instance.alloc(n, align, offset); };
	static void		free	(void* p, SizeType n = 0) { s_instance.free(p, n); }

	static const char* name() { return s_instance.name(); }

	static bool is_owning(void* p, SizeType n) { return s_instance.is_owning(p, n); };

private:
	static MemoryArena<DefaultAllocator_Impl, void> s_instance;
};

#endif

}