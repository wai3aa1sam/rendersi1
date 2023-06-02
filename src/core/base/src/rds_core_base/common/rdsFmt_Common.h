#pragma once

#include "rdsNmsplib_Common.h"

#define RDS_FORMATTER_T(TEMPLATE_ARGS, CLASS) \
	template<TEMPLATE_ARGS> \
	struct fmt::formatter<rds::CLASS> { \
		auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); } \
		auto format(const rds::CLASS& v, fmt::format_context& ctx) { \
			rds::onFormat(ctx, v); \
			return ctx.out(); \
		} \
	} \
//------

#define RDS_FORMATTER(CLASS) RDS_FORMATTER_T(RDS_EMPTY, CLASS)


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
using ::nmsp::fmtAs_T;

#endif

}

