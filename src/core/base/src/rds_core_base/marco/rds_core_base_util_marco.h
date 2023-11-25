#pragma once

#include <assert.h>

#if 0
#pragma mark --- rds_util_marco-Impl ---
#endif // 0
#if 1

#define RDS_FILE __FILE__
#define RDS_LINE __LINE__

#define RDS_COMMA ,
#define RDS_EMPTY
#define RDS_ARGS(...) __VA_ARGS__
#define RDS_STRINGIFY(...)	#__VA_ARGS__

#define RDS_IDENTITY(X) X
#define RDS_CALL(M, ARGS) RDS_IDENTITY( M(ARGS) )

#define RDS_CONCAT_IMPL(A, B) A ## B
#define RDS_CONCAT(A, B) RDS_CONCAT_IMPL(A, B)
#define RDS_CONCAT_TO_STR(A, B) RDS_STRINGIFY(A) RDS_STRINGIFY(B)
#define RDS_UNIQUE_VAR_NAME(CLASS) RDS_CONCAT(RDS_CONCAT(_, CLASS), RDS_LINE)
#define RDS_UNIQUE_VAR(CLASS)		CLASS RDS_UNIQUE_VAR_NAME(CLASS)

#define RDS_CALL_ARGS(X, ...)	RDS_IDENTITY( X(__VA_ARGS__) )
#define RDS_ANGLE_BRACKET(...) 	RDS_IDENTITY( <__VA_ARGS__> )
#define RDS_BRACKET(...)		RDS_IDENTITY( (__VA_ARGS__) )

#define RDS_VA_ARGS_COUNT(...) RDS_IDENTITY( RDS_VA_ARGS_COUNT_INTERNAL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1) )
#define RDS_VA_ARGS_COUNT_INTERNAL(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, COUNT, ...) COUNT

#define RDS_UNUSED(X)	(void)X

#define RDS_CONSTEXPR				constexpr
#define RDS_NOEXCEPT				noexcept
#define RDS_NOEXCEPT_IF(...)		noexcept(__VA_ARGS__)

#define RDS_ALIGN_OF(X) alignof(X)
#define RDS_ALIGN_AS(X) alignas(X)

#define RDS_S_ASSERT(COND, ...) static_assert(COND, RDS_FUNC_NAME_SZ ## "() " "--- " #COND ## " --- " ## __VA_ARGS__)

#define RDS_CALL_ONCE(...) do { static RDS_UNIQUE_VAR(bool) = false; if (!RDS_UNIQUE_VAR_NAME(bool)) { __VA_ARGS__; RDS_UNIQUE_VAR_NAME(bool) = true; } } while(false)

#if RDS_DEBUG || RDS_ENABLE_ASSERT
	#define RDS_CORE_ASSERT(X, ...)	do{ if(!(X)) { _log(__VA_ARGS__); RDS_DEBUG_BREAK(); assert(X);  } } while(false)
	#define RDS_ASSERT(X, ...)		do{ if(!(X)) { _log(__VA_ARGS__); RDS_DEBUG_BREAK(); assert(X);  } } while(false)
#else
	#define RDS_CORE_ASSERT(X, ...)
	#define RDS_ASSERT(X, ...)	
#endif // RDS_ENABLE_ASSERT


#if RDS_DEBUG || RDS_DEVELOPMENT
	#define RDS_SRCLOC	::rds::SrcLoc(RDS_FILE, RDS_LINE, RDS_FUNC_NAME_SZ)

	#define RDS_DEBUG_CALL(FUNC) FUNC

	#define RDS_DEBUG_SRCLOC			_debugSrcLoc
	#define RDS_DEBUG_SRCLOC_DECL		::rds::SrcLoc RDS_DEBUG_SRCLOC
	#define RDS_DEBUG_SRCLOC_ARG		debugSrcLoc_
	#define RDS_DEBUG_SRCLOC_PARAM		const ::rds::SrcLoc& RDS_DEBUG_SRCLOC_ARG
	#define RDS_DEBUG_SRCLOC_ASSIGN()	RDS_DEBUG_SRCLOC = RDS_DEBUG_SRCLOC_ARG
	// can use default value marco 
	// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
	// RDS_DEBUG_SRCLOC_ASSIGN() + RDS_DEBUG_SRCLOC_ASSIGN(cDesc)

#else
	#define RDS_SRCLOC	::rds::SrcLoc()

	#define RDS_DEBUG_CALL(FUNC)

	#define RDS_DEBUG_SRCLOC			
	#define RDS_DEBUG_SRCLOC_DECL			
	#define RDS_DEBUG_SRCLOC_ARG			
	#define RDS_DEBUG_SRCLOC_PARAM			
	#define RDS_DEBUG_SRCLOC_ASSIGN()	

#endif

#define RDS_REP_0(X)
#define RDS_REP_1(X) X
#define RDS_REP_2(X)  RDS_REP_1(X) X
#define RDS_REP_3(X)  RDS_REP_2(X) X
#define RDS_REP_4(X)  RDS_REP_3(X) X
#define RDS_REP_5(X)  RDS_REP_4(X) X
#define RDS_REP_6(X)  RDS_REP_5(X) X
#define RDS_REP_7(X)  RDS_REP_6(X) X
#define RDS_REP_8(X)  RDS_REP_7(X) X
#define RDS_REP_9(X)  RDS_REP_8(X) X
#define RDS_REP_10(X) RDS_REP_9(X) X
//#define RDS_REP_N(HUNDREDS,TENS,ONES,X) \
//  RDS_REP##HUNDREDS(RDS_REP10(RDS_REP10(X))) \
//  RDS_REP##TENS(REP10(X)) \
//  RDS_REP##ONES(X)

#define RDS_IDX_SEQ_IMPL(SEP, I, X, ...)	SEP RDS_IDENTITY(X(I __VA_ARGS__))
#define RDS_IDX_SEQ_1(SEP, X, ...)		RDS_IDENTITY(X(0 __VA_ARGS__))
#define RDS_IDX_SEQ_2(SEP, X, ...)		RDS_IDX_SEQ_1(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 1, X, __VA_ARGS__)
#define RDS_IDX_SEQ_3(SEP, X, ...)		RDS_IDX_SEQ_2(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 2, X, __VA_ARGS__)
#define RDS_IDX_SEQ_4(SEP, X, ...)		RDS_IDX_SEQ_3(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 3, X, __VA_ARGS__)
#define RDS_IDX_SEQ_5(SEP, X, ...)		RDS_IDX_SEQ_4(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 4, X, __VA_ARGS__)
#define RDS_IDX_SEQ_6(SEP, X, ...)		RDS_IDX_SEQ_5(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 5, X, __VA_ARGS__)
#define RDS_IDX_SEQ_7(SEP, X, ...)		RDS_IDX_SEQ_6(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 6, X, __VA_ARGS__)
#define RDS_IDX_SEQ_8(SEP, X, ...)		RDS_IDX_SEQ_7(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 7, X, __VA_ARGS__)
#define RDS_IDX_SEQ_9(SEP, X, ...)		RDS_IDX_SEQ_8(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 8, X, __VA_ARGS__)
#define RDS_IDX_SEQ_10(SEP, X, ...)		RDS_IDX_SEQ_9(SEP, X, __VA_ARGS__) RDS_IDX_SEQ_IMPL(SEP, 9, X, __VA_ARGS__)

#define RDS_PARAMS_IMPL(I) A ## I
#define RDS_PARAMS_1		RDS_IDX_SEQ_1(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_2		RDS_IDX_SEQ_2(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_3		RDS_IDX_SEQ_3(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_4		RDS_IDX_SEQ_4(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_5		RDS_IDX_SEQ_5(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_6		RDS_IDX_SEQ_6(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_7		RDS_IDX_SEQ_7(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_8		RDS_IDX_SEQ_8(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_9		RDS_IDX_SEQ_9(RDS_COMMA, RDS_PARAMS_IMPL)
#define RDS_PARAMS_10		RDS_IDX_SEQ_10(RDS_COMMA, RDS_PARAMS_IMPL)


#endif