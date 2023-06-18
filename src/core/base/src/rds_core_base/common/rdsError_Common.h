#pragma once

#include <nmsp_core_base.h>
#include "rdsDataType_Common.h"

#define RDS_ERROR(...) rds::Error(RDS_SRCLOC, rds::fmtAs_T<rds::TempString>(__VA_ARGS__).c_str())
#define RDS_THROW(...) throw RDS_ERROR(__VA_ARGS__)

#if 0
#pragma mark --- Error-Decl ---
#endif // 0
#if 1
namespace rds
{

using Error_Base = ::nmsp::Error_Base;
using Error      = ::nmsp::Error_T;

}
#endif

