#pragma once

#include "rds_core_base/common/rdsNmsplib_Common.h"

#include <nmsp_stl/allocator/nmspFixedPoolAllocator.h>
#include <nmsp_stl/allocator/nmspPoolAllocator.h>
#include <nmsp_stl/allocator/nmspMemoryArena.h>
#include <nmsp_stl/allocator/nmspMemoryArenaPolicy.h>

#include "rds_core_base/common/rds_core_base_traits.h"
#include "rds_core_base/common/rdsClass_Common.h"

// references
// - (https://blog.molecular-matters.com/2011/07/05/memory-system-part-1/)
// - (https://allenchou.net/2013/05/memory-management-part-1-of-3-the-allocator/)

namespace rds
{
#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- rds_allocator-Impl ---
#endif // 0
#if 1

template<class ALLOC> using Allocator_Base	= ::nmsp::Allocator_Base<ALLOC>;

class IAllocator : public Allocator_Base<IAllocator>
{
public:
	using Base = Allocator_Base<IAllocator>;

public:
	IAllocator(const char* name = "IAllocator") : Base(name) {};
	virtual ~IAllocator()	= default;

	virtual void*	alloc	(SizeType n, SizeType align = CoreBaseTraits::s_kDefaultAlign, SizeType offset = 0) = 0;
	virtual void	free	(void* p, SizeType n = 0) = 0;

	bool is_owning(void* p, SizeType n) const { RDS_CORE_ASSERT(false); return false; };
private:

};

using ::nmsp::NoFallbackAllocator_Policy;
template<class PRIMARY_ALLOC, class FALLBACK_ALLOC = NoFallbackAllocator_Policy> using FallbackAllocator = ::nmsp::FallbackAllocator_T<PRIMARY_ALLOC, FALLBACK_ALLOC>;

using PoolAllocator			= ::nmsp::PoolAllocator_T;
using FixedPoolAllocator	= ::nmsp::FixedPoolAllocator_T;
using LinearAllocator		= ::nmsp::LinearAllocator_T;
using Mallocator			= ::nmsp::Mallocator_T;

template<class ALLOC, class BOUND_POLICY> using MemoryArena = ::nmsp::MemoryArena_T<ALLOC, BOUND_POLICY>;

struct MemoryDefaultTraits : public CoreBaseTraits
{
public:
	static constexpr SizeType s_kBlockSizes[] = 
	{
		// 4-increments
		4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
		52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 

		// 32-increments
		128, 160, 192, 224, 256, 288, 320, 352, 384, 
		416, 448, 480, 512, 544, 576, 608, 640, 

		// 64-increments
		704, 768, 832, 896, 960, 1024
	};

	static constexpr SizeType s_kBlockCount = ArraySize<decltype(s_kBlockSizes)>;


private:

};

using MemoryTraits = MemoryDefaultTraits;


#endif

#endif
}

