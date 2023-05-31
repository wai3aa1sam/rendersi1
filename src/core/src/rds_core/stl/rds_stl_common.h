#pragma once

#include "rds_core/common/rds_core_common.h"

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{



}
#endif

#if 0
#pragma mark --- rds_stl-Impl ---
#endif // 0
#if 1
namespace rds
{

#if 0
#pragma mark --- Container-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- Vector-Impl ---
#endif // 0
#if 1
template<class T>												using IVector	= ::nmsp::IVector_T<T>;
template<class T, size_t N = 0, class ALLOC = DefaultAllocator>	using Vector	= ::nmsp::Vector_T<T, N, ALLOC>;
#endif

#if 0
#pragma mark --- String-Impl ---
#endif // 0
#if 1
template<class T>												using IString_T = ::nmsp::IString_T<T>;
template<class T, size_t N = 0, class ALLOC = DefaultAllocator>	using String_T = ::nmsp::String_T<T, N, ALLOC>;

template<size_t N = 0, class ALLOC = DefaultAllocator>	using StringA_T		= String_T<char, N, ALLOC>;
template<class ALLOC = DefaultAllocator>				using TempStringA_T = ::nmsp::TempStringA_T<ALLOC>;
using String		= StringA_T<0, DefaultAllocator>;
using TempString	= TempStringA_T<DefaultAllocator>;

template<size_t N = 0, class ALLOC = DefaultAllocator>	using StringW_T		= String_T<wchar_t, N, ALLOC>;
template<class ALLOC = DefaultAllocator>				using TempStringW_T = ::nmsp::TempStringW_T<ALLOC>;
using StringW		= StringW_T<0, DefaultAllocator>;
using TempStringW	= TempStringW_T<DefaultAllocator>;
#endif

#if 0
#pragma mark --- Set_and_Map-Impl ---
#endif // 0
#if 1
template<class KEY, class PRED = Less<KEY>, class ALLOC = DefaultAllocator>	using Set = Set_T<KEY, PRED, ALLOC>;

template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using Map			= ::nmsp::Map_T<KEY, VALUE, PRED, ALLOC>;
template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using UnorderedMap = ::nmsp::UnorderedMap_T<KEY, VALUE, PRED, ALLOC>;
template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using VectorMap	= ::nmsp::VectorMap_T<KEY, VALUE, PRED, ALLOC>;
template<class VALUE, class PRED = StrLess<const char*>, class ALLOC = DefaultAllocator> using StringMap	= ::nmsp::StringMap_Impl<VALUE, PRED, ALLOC>;

#endif

#endif

}
#endif


