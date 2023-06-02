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


using NullPtr = nullptr_t;

#if 0
#pragma mark --- nmspTypeTraits_Util-Impl ---
#endif // 0
#if 1

template<bool COND, class T = void> using EnableIfT = typename std::enable_if<COND, T>;
template<bool COND, class T = void> using EnableIf  = typename EnableIfT<COND, T>::type;

template<bool COND, class IF_TRUE_T, class IF_FASLE_T> using ConditionalT   = typename std::conditional<COND, IF_TRUE_T, IF_FASLE_T>;
template<bool COND, class IF_TRUE_T, class IF_FASLE_T> using Conditional    = typename ConditionalT<COND, IF_TRUE_T, IF_FASLE_T>::type;

#endif

#if 0
#pragma mark --- nmspTypeTraits_Remove-Impl ---
#endif // 0
#if 1

template<class T> using DecayT = typename std::decay<T>;
template<class T> using Decay  = typename DecayT<T>::type;

template<class T> using RemoveRefT  = typename std::remove_reference<T>;
template<class T> using RemoveRef   = typename RemoveRefT<T>::type;

template<class T> using RemovePtrT  = typename std::remove_pointer<T>;
template<class T> using RemovePtr   = typename RemovePtrT<T>::type;


#endif

#if 0
#pragma mark --- nmspTypeTraits_IsType-Impl ---
#endif // 0
#if 1

template<class T> using                 IsEmptyT    = std::is_empty<T>;
template<class T> inline constexpr bool IsEmpty     = IsEmptyT<T>::value;

template<class T1, class T2> using					IsSameT = std::is_same<T1, T2>;
template<class T1, class T2> inline constexpr bool	IsSame  = IsSameT<T1, T2>::value;

template<class T> using                 IsLVRefT    = std::is_lvalue_reference<T>;
template<class T> inline constexpr bool IsLVRef     = IsLVRefT<T>::value;

template<class T> using                 IsTrivialT  = std::is_trivial<T>;
template<class T> inline constexpr bool IsTrivial   = IsTrivialT<T>::value;

template<class BASE, class DERIVED> using				  IsBaseOfT = typename std::is_base_of<BASE, DERIVED>;
template<class BASE, class DERIVED> inline constexpr bool IsBaseOf  = IsBaseOfT<BASE, DERIVED>::value;

template<class T> using					IsFunctionT     = std::is_function<T>;
template<class T> inline constexpr bool	IsFunction      = IsFunctionT<T>::value;

template<class T> using					IsMemPtrT       = std::is_member_pointer<T>;
template<class T> inline constexpr bool	IsMemPtr        = IsMemPtrT<T>::value;

template<class T> using                 IsEnumT         = std::is_enum<T>;
template<class T> inline constexpr bool IsEnum          = IsEnumT<T>::value;

template<class T> using					IsUnsignedT     = std::is_unsigned<T>;
template<class T> inline constexpr bool	IsUnsigned      = IsUnsignedT<T>::value;

template<class T> using					IsSignedT       = std::is_signed<T>;
template<class T> inline constexpr bool	IsSigned        = IsSignedT<T>::value;

template<class T> using					IsIntegralT     = std::is_integral<T>;
template<class T> inline constexpr bool	IsIntegral      = IsIntegralT<T>::value;

template<class T> using					IsFloatT        = std::is_floating_point<T>;
template<class T> inline constexpr bool	IsFloat         = IsFloatT<T>::value;

template<class T> inline constexpr bool	IsUInt          = IsUnsigned<T> && IsIntegral<T>;
template<class T> inline constexpr bool	IsInt           = IsSigned<T>   && IsIntegral<T>;


#endif

#if 0
#pragma mark --- nmspTypeTraits_Value-Impl ---
#endif // 0
#if 1

template<class... ARGS> using VoidType = void;

using TrueType = std::true_type;
using FalseType = std::false_type;

template<class T, T VALUE> using					IntConstantT   = ::nmsp::IntConstantT<T, VALUE>;
template<class T, T VALUE> inline constexpr bool	IntConstant    = IntConstantT<T, VALUE>::value;

template<class T> using EnumInt = typename std::underlying_type<T>::type;

template<class T> using NumLimit = typename std::numeric_limits<T>;

#endif

using ::nmsp::forward;
using ::nmsp::move;

#endif // 1

#endif
}
