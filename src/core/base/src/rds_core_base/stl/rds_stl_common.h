#pragma once

#include "rds_core_base/common/rds_core_base_common.h"


#include <nmsp_stl/string/nmspLexer.h>
#include <nmsp_stl/string/nmspStrUtil.h>


namespace rds
{
#if 0
#pragma mark --- rds_stl-Impl ---
#endif // 0
#if 1



#if 0
#pragma mark --- rds_stl_utility-Impl ---
#endif
#if 1

#if 0
#pragma mark --- rds_stl_functional-Impl ---
#endif
#if 1

using ::nmsp::Less		;
using ::nmsp::EqualTo	;
using ::nmsp::StrLess	;

using ::nmsp::Plus;

using ::nmsp::Hash;

using ::nmsp::invoke;

#endif

#if 0
#pragma mark --- rds_stl_memory-Impl ---
#endif
#if 1

using ::nmsp::memory_copy;
using ::nmsp::memory_move;
using ::nmsp::memory_set;

#endif

#if 0
#pragma mark --- rds_stl_algorithm-Impl ---
#endif
#if 1

using ::nmsp::find_if;

#endif

#endif


#if 0
#pragma mark --- rds_stl_container-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- rds_stl_vector-Impl ---
#endif // 0
#if 1
template<class T>												using IVector	= ::nmsp::IVector_T<T>;
template<class T, size_t N = 0, class ALLOC = DefaultAllocator>	using Vector	= ::nmsp::Vector_T<T, N, ALLOC>;
#endif

#if 0
#pragma mark --- rds_stl_string-Impl ---
#endif // 0
#if 1
template<class T>												using IString_T = ::nmsp::IString_T<T>;
template<class T, size_t N = 0, class ALLOC = DefaultAllocator>	using String_T	= ::nmsp::String_T<T, N, ALLOC>;

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
#pragma mark --- rds_stl_set_and_map-Impl ---
#endif // 0
#if 1
template<class KEY, class PRED = Less<KEY>, class ALLOC = DefaultAllocator>	using Set = ::nmsp::Set_T<KEY, PRED, ALLOC>;

template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using Map			= ::nmsp::Map_T<KEY, VALUE, PRED, ALLOC>;
template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using UnorderedMap = ::nmsp::UnorderedMap_T<KEY, VALUE, PRED, ALLOC>;
template<class KEY, class VALUE, class PRED = Less<KEY>, class ALLOC = DefaultAllocator> using VectorMap	= ::nmsp::VectorMap_T<KEY, VALUE, PRED, ALLOC>;
template<class VALUE, class PRED = StrLess<const char*>, class ALLOC = DefaultAllocator> using StringMap	= ::nmsp::StringMap_Impl<VALUE, PRED, ALLOC>;

#endif

#endif

#if 0
#pragma mark --- rds_stl_view-Impl ---
#endif // 0
#if 1

template<class T> using Span = ::nmsp::Span_T<T>;
using ByteSpan = ::nmsp::ByteSpan_T;
using ::nmsp::spanCast;
using ::nmsp::spanConstCast;

template<class T> using StrView_T = ::nmsp::StrView_T<T>;
using StrView  = ::nmsp::StrViewA_T;
using StrViewW = ::nmsp::StrViewW_T;
using ::nmsp::toStrView;
using ::nmsp::toStrViewW;

using ::nmsp::makeStrView;
using ::nmsp::makeByteSpan;

#endif


#if 0
#pragma mark --- rds_stl_extra-Impl ---
#endif // 0
#if 1

template<class T1, class T2> using CompressedPair	= ::nmsp::CompressedPair_T<T1, T2>;
template<class T1, class T2> using Pair				= ::nmsp::Pair_T<T1, T2>;
using ::nmsp::makePair;

template<class T, size_t LOCAL_SIZE = 32, size_t ALIGN = CoreBaseTraits::s_kDefaultAlign, class FALLBACK_ALLOCATOR = DefaultAllocator> 
using Function_T = ::nmsp::Function_T<T, LOCAL_SIZE, ALIGN, FALLBACK_ALLOCATOR>;

template<class T, T... VALS>	using IntSeq = ::nmsp::IntSeq_T<T, VALS...>;
template<size_t N>				using IdxSeq = ::nmsp::IdxSeq_T<N>;

template<class T> using Opt = ::nmsp::Opt_T<T>;

template<class T, size_t ALIGN> using PaddedData = ::nmsp::PaddedData_T<T, ALIGN>;

template<class... ARGS> using Tuple = ::nmsp::Tuple_T<ARGS...>;

//template<class T> using TypeBitMixture_Impl = ::nmsp::TypeBitMixture_Impl<T>;
//template<class T> using TypeBitMixture		= ::nmsp::TypeBitMixture_T<T, TypeBitMixture_Impl<T> >;
//template<class T> using TBM					= ::nmsp::TBM<T>;

#endif


#if 0
#pragma mark --- rds_stl_pointer-Impl ---
#endif // 0
#if 1

using ::nmsp::DefaultDeleter;
template<class T, class DELETER = DefaultDeleter<T> > using UPtr = ::nmsp::UPtr_T<T, DELETER>;
using ::nmsp::makeUPtr;

template<class T, class DELETER = DefaultDeleter<T>> using SPtr = ::nmsp::SPtr_T<T, DELETER>;
using ::nmsp::makeSPtr;

template<class T> using SharedPtr = ::nmsp::SharedPtr_T<T>;
using ::nmsp::makeShared;

#endif

#if 0
#pragma mark --- rds_stl_string-Impl ---
#endif
#if 1

using Lexer_TokenType	= ::nmsp::Lexer_TokenType_T;
using Lexer_Token		= ::nmsp::Lexer_Token_T;
using Lexer				= ::nmsp::Lexer_T;

using StrUtil = ::nmsp::StrUtil;
using UtfUtil = ::nmsp::UtfUtil_T<DefaultAllocator>;


#endif // 0


#if 0
#pragma mark --- rds_stl_allocator-Impl ---
#endif
#if 1

using LinearAllocator = ::nmsp::LinearAllocator_T;


#endif


#endif
}


