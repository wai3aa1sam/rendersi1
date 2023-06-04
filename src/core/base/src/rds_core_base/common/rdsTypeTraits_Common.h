#pragma once

#include "rdsNmsplib_Common.h"


namespace rds
{
#if 0
#pragma mark --- rdsTypeTraits-Impl ---
#endif // 0
#if 1

template<class T>
class TypeDisplayer;
// eg. TypeDisplayer<decltype(data->data)> x; (void)x;


using NullPtr = ::nmsp::NullPtr;

#if 0
#pragma mark --- nmspTypeTraits_Util-Impl ---
#endif // 0
#if 1

template<bool COND, class T = void> using EnableIfT = ::nmsp::EnableIfT<COND, T>;
template<bool COND, class T = void> using EnableIf  = typename EnableIfT<COND, T>::type;

template<bool COND, class IF_TRUE_T, class IF_FASLE_T> using ConditionalT   =  ::nmsp::ConditionalT<COND, IF_TRUE_T, IF_FASLE_T>;
template<bool COND, class IF_TRUE_T, class IF_FASLE_T> using Conditional    = typename ConditionalT<COND, IF_TRUE_T, IF_FASLE_T>::type;

#endif

#if 0
#pragma mark --- nmspTypeTraits_Remove-Impl ---
#endif // 0
#if 1

template<class T> using DecayT = ::nmsp::DecayT<T>;
template<class T> using Decay  = typename DecayT<T>::type;

template<class T> using RemoveRefT  = ::nmsp::RemoveRefT<T>;
template<class T> using RemoveRef   = typename RemoveRefT<T>::type;

template<class T> using RemovePtrT  = ::nmsp::RemovePtrT<T>;
template<class T> using RemovePtr   = typename RemovePtrT<T>::type;


#endif

#if 0
#pragma mark --- nmspTypeTraits_IsType-Impl ---
#endif // 0
#if 1

template<class T> using					IsEmptyT	= ::nmsp::IsEmptyT<T>;
template<class T> inline constexpr bool	IsEmpty		= IsEmptyT<T>::value;

template<class T1, class T2> using					IsSameT = ::nmsp::IsSameT<T1, T2>;
template<class T1, class T2> inline constexpr bool	IsSame  = IsSameT<T1, T2>::value;

template<class T> using                 IsLVRefT			= ::nmsp::IsLVRefT<T>;
template<class T> inline constexpr bool IsLVRef				= IsLVRefT<T>::value;

template<class T> using                 IsTrivialT			= ::nmsp::IsTrivialT<T>;
template<class T> inline constexpr bool IsTrivial			= IsTrivialT<T>::value;

template<class BASE, class DERIVED> using				  IsBaseOfT = ::nmsp::IsBaseOfT<BASE, DERIVED>;
template<class BASE, class DERIVED> inline constexpr bool IsBaseOf  = IsBaseOfT<BASE, DERIVED>::value;

template<class T> using					IsFunctionT     = ::nmsp::IsFunctionT<T>;
template<class T> inline constexpr bool	IsFunction      = IsFunctionT<T>::value;

template<class T> using					IsMemPtrT       = ::nmsp::IsMemPtrT<T>;
template<class T> inline constexpr bool	IsMemPtr        = IsMemPtrT<T>::value;

template<class T> using                 IsEnumT         = ::nmsp::IsEnumT<T>;
template<class T> inline constexpr bool IsEnum          = IsEnumT<T>::value;

template<class T> using					IsUnsignedT     = ::nmsp::IsUnsignedT<T>;
template<class T> inline constexpr bool	IsUnsigned      = IsUnsignedT<T>::value;

template<class T> using					IsSignedT       = ::nmsp::IsSignedT<T>;
template<class T> inline constexpr bool	IsSigned        = IsSignedT<T>::value;

template<class T> using					IsIntegralT     = ::nmsp::IsIntegralT<T>;
template<class T> inline constexpr bool	IsIntegral      = IsIntegralT<T>::value;

template<class T> using					IsFloatT        = ::nmsp::IsFloatT<T>;
template<class T> inline constexpr bool	IsFloat         = IsFloatT<T>::value;

template<class T> inline constexpr bool	IsUInt          = ::nmsp::IsUInt<T>;
template<class T> inline constexpr bool	IsInt           = ::nmsp::IsInt <T>;


#endif

#if 0
#pragma mark --- nmspTypeTraits_Value-Impl ---
#endif // 0
#if 1

template<class... ARGS> using VoidType = ::nmsp::VoidType<ARGS...>;

using TrueType	= ::nmsp::TrueType;
using FalseType = ::nmsp::FalseType;

template<class T, T VALUE> using					IntConstantT   = ::nmsp::IntConstantT<T, VALUE>;
template<class T, T VALUE> inline constexpr bool	IntConstant    = IntConstantT<T, VALUE>::value;

template<class T> using EnumInt = ::nmsp::EnumInt<T>;

template<class T> using NumLimit = ::nmsp::NumLimit<T>;

template<class T, size_t N = 0> using					ArraySizeT  = std::extent<T, N>;
template<class T, size_t N = 0> inline constexpr size_t	ArraySize   = ArraySizeT<T, N>::value;

#endif

using ::nmsp::forward;
using ::nmsp::move;

#endif // 1

}
