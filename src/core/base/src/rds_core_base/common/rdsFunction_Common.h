#pragma once

#include "rdsNmsplib_Common.h"

#if 0
#pragma mark --- rdsFunction_Common-Impl ---
#endif // 0
#if 1

#define RDS_NOT_YET_SUPPORT() NMSP_NOT_YET_SUPPORT()

namespace rds
{

#if 0
#pragma mark --- nmspCast-Impl ---
#endif // 0
#if 1

using ::nmsp::sCast;
using ::nmsp::reinCast;
using ::nmsp::constCast;

#endif

using ::nmsp::swap;
using ::nmsp::_alignTo;
using ::nmsp::memberOffset;

using ::nmsp::arraySize;
using ::nmsp::arraySizeT;


#if 0
#pragma mark --- rds_os_alloc-Impl ---
#endif // 0
#if 1

using ::nmsp::os_aligned_alloc;
using ::nmsp::os_aligned_free;

#endif // 0

#if 0
#pragma mark --- rdsFunction_Common-Impl ---
#endif // 0
#if 1

using ::nmsp::throwIf;
using ::nmsp::throwError;
using ::nmsp::_log;
using ::nmsp::_notYetSupported;
using ::nmsp::_todo;

#endif

}
#endif