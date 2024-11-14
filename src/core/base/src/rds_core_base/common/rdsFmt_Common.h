#pragma once

#include "rdsNmsplib_Common.h"

#define RDS_FORMATTER_T(TEMPLATE_ARGS, CLASS) \
	template<TEMPLATE_ARGS> \
	struct fmt::formatter<rds::CLASS> { \
		auto parse(fmt::format_parse_context& ctx) const { return ctx.begin(); } \
		auto format(const rds::CLASS& v, fmt::format_context& ctx) { \
			rds::onFormat(ctx, v); \
			return ctx.out(); \
		} \
	} \
//------

#define RDS_FORMATTER(CLASS) RDS_FORMATTER_T(RDS_EMPTY, CLASS)

#define RDS_FMT(T, FMT, ...)									fmtAs_T<T>(FMT, __VA_ARGS__)
#define RDS_FMT_TO(VAR, FMT, ...)								fmtTo(VAR, FMT, __VA_ARGS__)

#if 1

#define RDS_FMT_VAR(STR_NAME, T, FMT, ...)				   auto RDS_VAR_NAME(STR_NAME) = RDS_FMT(T, FMT, __VA_ARGS__); RDS_UNUSED(RDS_VAR_NAME(STR_NAME))
#define RDS_FMT_STATIC_VAR(STR_NAME, T, FMT, ...)	static auto RDS_VAR_NAME(STR_NAME) = RDS_FMT(T, FMT, __VA_ARGS__); RDS_UNUSED(RDS_VAR_NAME(STR_NAME))

#else

#define RDS_FMT_VAR(STR_NAME, C_STR_NAME, T, FMT, ...)				   auto RDS_VAR_NAME(STR_NAME) = RDS_FMT(T, FMT, __VA_ARGS__);		  const auto* RDS_VAR_NAME(C_STR_NAME) = RDS_VAR_NAME(STR_NAME).c_str(); RDS_UNUSED(RDS_VAR_NAME(STR_NAME)); RDS_UNUSED(RDS_VAR_NAME(C_STR_NAME))
#define RDS_FMT_STATIC_VAR(STR_NAME, C_STR_NAME, T, FMT, ...)	static auto RDS_VAR_NAME(STR_NAME) = RDS_FMT(T, FMT, __VA_ARGS__); static const auto* RDS_VAR_NAME(C_STR_NAME) = RDS_VAR_NAME(STR_NAME).c_str(); RDS_UNUSED(RDS_VAR_NAME(STR_NAME)); RDS_UNUSED(RDS_VAR_NAME(C_STR_NAME))

#endif // 0



#if RDS_DEVELOPMENT

#define RDS_FMT_DEBUG(...) ::rds::fmtAs_T<::rds::TempString>(__VA_ARGS__)

#else

#define RDS_FMT_DEBUG(...) ""

#endif // RDS_DEVELOPMENT

#if 0
#pragma mark --- rdsFmt-Impl ---
#endif // 0
#if 1
namespace rds
{
    
using ::nmsp::onFormat;
using ::nmsp::formatTo;

template<class T> using FormatTrait = ::nmsp::FormatTrait<T>;

using ::nmsp::makeStdStrView;
using ::nmsp::fmtTo;
using ::nmsp::fmtToNew;
using ::nmsp::fmtAs_T;

#endif

}

