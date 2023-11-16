#pragma once

#include "rds_memory_common.h"
#include "rdsMemoryContext.h"

#define RDS_TEMPORARY_ALLOC_MODE 1

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

	static void*	alloc	(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0);
	static void		free	(void* p, SizeType n = 0);

	static const char* name() { return MemoryContext::instance()->name(); }

	static bool is_owning(void* p, SizeType n) { RDS_CORE_ASSERT(false); return false; };

private:
};

inline
void*	
DefaultAllocator_Impl::alloc(SizeType n, SizeType align, SizeType offset) 
{
	_notYetSupported(RDS_SRCLOC);
	return MemoryContext::instance()->alloc(n, align, offset);
};

inline
void
DefaultAllocator_Impl::free(void* p, SizeType n)
{
	_notYetSupported(RDS_SRCLOC);
	MemoryContext::instance()->free(p, n);
}

class DefaultAllocator : public Allocator_Base<DefaultAllocator>
{
public:
	using Base = Allocator_Base<DefaultAllocator>;

public:
	DefaultAllocator(const char* name = "DefaultAllocator") : Base(name) {};
	~DefaultAllocator()	= default;

	static void*	alloc	(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0);
	static void		free	(void* p, SizeType n = 0);

	static const char* name() { return s_instance.name(); }

	static bool is_owning(void* p, SizeType n) { return s_instance.is_owning(p, n); };

private:
	static MemoryArena<DefaultAllocator_Impl, void> s_instance;
};

inline
void*	
DefaultAllocator::alloc(SizeType n, SizeType align, SizeType offset) 
{
	#if RDS_TEMPORARY_ALLOC_MODE
	NMSP_TODO("--- error");
	NMSP_TODO("currently will crash when using nmsp_alloc in ShaderResources::ConstBuffer::_setValue");
	NMSP_TODO("there is two temp solution to fix 1. use new u8[n], but those use NMSP_NEW eg. ::nmsp::Singleton will need to change too");
	NMSP_TODO("2. use memory_copy instead of dereference the buf");
	NMSP_TODO("--- end warning");
	return ::nmsp::nmsp_alloc(n, align, offset);
	//return new char[n];
	//return ::new(align, offset,    "", 0, 0, __FILE__, __LINE__) char[n];
	#else
	_notYetSupported(RDS_SRCLOC);
	return s_instance.alloc(n, align, offset); 
	#endif // RDS_TEMPORARY_ALLOC_MODE

};

inline
void
DefaultAllocator::free(void* p, SizeType n)
{
	#if RDS_TEMPORARY_ALLOC_MODE
	::nmsp::nmsp_free(p, n);
	//delete[] (char*)p;
	#else
	_notYetSupported(RDS_SRCLOC);
	s_instance.free(p, n); 
	#endif // RDS_TEMPORARY_ALLOC_MODE
}


#endif

}