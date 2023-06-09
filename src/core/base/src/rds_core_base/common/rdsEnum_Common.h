#pragma once

#include <nmsp_core_base.h>

#if 0
#pragma mark --- rdsEnum-Impl ---
#endif // 0
#if 1
namespace rds
{
using ::nmsp::enumInt;
using ::nmsp::enumIntRef;
}
#endif


#if 0
#pragma mark --- rdsEnum-Impl ---
#endif // 0
#if 1

#define RDS_ENUM_BITWISE_OPERATOR(T)            NMSP_ENUM_BITWISE_OPERATOR(T)
#define RDS_ENUM_ARITHMETIC_OPERATOR(T)         NMSP_ENUM_ARITHMETIC_OPERATOR(T)
#define RDS_ENUM_ARITHMETIC_OPERATOR_INT(T)     NMSP_ENUM_ARITHMETIC_OPERATOR_INT(T)
#define RDS_ENUM_ALL_OPERATOR(T)                NMSP_ENUM_ALL_OPERATOR(T)

#define RDS_ENUM_STR__CASE(V, ...) case RDS_T::V: return #V;

#define RDS_ENUM_STR(T) \
	inline const char* enumStr(const T& v) { \
		using RDS_T = T; \
		switch (v) { \
			T##_ENUM_LIST(RDS_ENUM_STR__CASE) \
			default: RDS_ASSERT(false); return ""; \
		} \
	} \
//----

#define RDS_FORMATTER_ENUM(T) \
	} /* namespace rds */ \
	template<> \
	struct fmt::formatter<rds::T> { \
		auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); } \
		auto format(const rds::T& v, fmt::format_context& ctx) { \
			return rds::formatTo(ctx, "{}", rds::enumStr(v)); \
		} \
	}; \
	namespace rds { \
//-----

#define RDS_ENUM_TRY_PARSE__CASE(V, ...) if (str == #V) { outValue = RDS_T::V; return true; }

#define RDS_ENUM_TRY_PARSE(T) \
	inline bool enumTryParse(T& outValue, StrView str) { \
		using RDS_T = T; \
		T##_ENUM_LIST(RDS_ENUM_TRY_PARSE__CASE) \
		return false; \
	} \
//----

#define RDS_ENUM_STR_UTIL(T) \
	RDS_ENUM_STR(T) \
	RDS_ENUM_TRY_PARSE(T) \
	RDS_FORMATTER_ENUM(T) \
//----

#define RDS_ENUM_DECLARE__ITEM(ITEM, VALUE) ITEM VALUE,

#define RDS_ENUM_DECLARE(T, BASE_TYPE) \
	enum class T : BASE_TYPE { \
		T##_ENUM_LIST(RDS_ENUM_DECLARE__ITEM) \
	}; \
//----

#define RDS_ENUM_CLASS(T, BASE_TYPE) \
	RDS_ENUM_DECLARE(T, BASE_TYPE) \
	RDS_ENUM_STR_UTIL(T) \
//----
#endif