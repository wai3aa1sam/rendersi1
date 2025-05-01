#pragma once

#include <nmsp_core_base.h>
#include "rdsDataType_Common.h"

#define RDS_ERROR(...)			rds::Error(RDS_SRCLOC, rds::fmtAs_T<rds::TempString>(__VA_ARGS__).c_str())
#define RDS_THROW(...)			throw RDS_ERROR(__VA_ARGS__)
//#define RDS_THROW_IF(cond, ...) do { throwIf(cond, "{}: {}", RDS_SRCLOC, RDS_FMT(TempString, __VA_ARGS__)); } while(false)
#define RDS_THROW_IF(cond, ...) do { if (cond) { RDS_THROW(__VA_ARGS__) } } while(false)

#if 0
#pragma mark --- Error-Decl ---
#endif // 0
#if 1
namespace rds
{

using Error_Base = ::nmsp::Error_Base;
using Error      = ::nmsp::Error_T;

#if 0
template<class... ARGS> inline 
void 
throwIf(bool cond, const char* fmt, ARGS&&... args)
{
	if (cond)
	{
		throwError(fmt, rds::forward<ARGS>(args)...);
	}
}

template<class... ARGS> inline 
void 
throwError(const char* fmt, ARGS&&... args)
{
	throw RDS_ERROR(fmt, rds::forward<ARGS>(args)...);
}
#endif // 0


}
#endif

