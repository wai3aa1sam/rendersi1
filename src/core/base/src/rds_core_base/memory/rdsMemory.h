#pragma once

#include "rds_memory_common.h"
#include "rdsMemoryContext.h"
#include "rdsDefaultAllocator.h"

#if 0
#pragma mark --- rds_memory_marco-Impl ---
#endif // 0
#if 1

#define RDS_ALLOC_ALIGNED(N, ALIGN)	::rds::DefaultAllocator::alloc(N, ALIGN)
#define RDS_FREE_ALIGNED(PTR, ...)	::rds::DefaultAllocator::free(PTR, __VA_ARGS__)

#define RDS_ALLOC(N)			RDS_ALLOC_ALIGNED(N, ::rds::CoreBaseTraits::s_kDefaultAlign)
#define RDS_FREE(PTR, ...)		RDS_FREE_ALIGNED(PTR, __VA_ARGS__)

#define RDS_NEW(T)				new(RDS_ALLOC_ALIGNED(sizeof(T), RDS_ALIGN_OF(T))) T
#define RDS_DELETE(PTR)			::rds::rds_delete(PTR)

#endif

namespace rds
{

class AllocScope
{
public:
	AllocScope(IAllocator* p)
	{
		MemoryContext::instance()->pushAllocator(p);
	}
	~AllocScope()
	{
		MemoryContext::instance()->popAllocator();
	}

private:

};

template<size_t LOCAL_SIZE, size_t ALIGN = NmspStlTraits::s_kDefaultAlign> using LocalBuffer = ::nmsp::LocalBuffer_T<LOCAL_SIZE, ALIGN>;
template<size_t LOCAL_SIZE = 0, size_t ALIGN = CoreBaseTraits::s_kDefaultAlign, class FALLBACK_ALLOC = DefaultAllocator> 
using LocalAllocator = ::nmsp::LocalAllocator_T<LOCAL_SIZE, ALIGN, FALLBACK_ALLOC>;

template<class T> inline 
void
rds_delete(T* p)	NMSP_NOEXCEPT 
{ 
	p->~T();
	RDS_FREE_ALIGNED(p); 
}

}
