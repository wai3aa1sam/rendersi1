#pragma once

#include "rdsNmsplib_Common.h"

#if 0
#pragma mark --- rdsTypeTraits-Impl ---
#endif // 0
#if 1
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


#if 0
#pragma mark --- rds_os_alloc-Impl ---
#endif // 0
#if 1

using ::nmsp::os_aligned_alloc;
using ::nmsp::os_aligned_free;

#endif // 0

using ::nmsp::throwIf;
using ::nmsp::throwError;
using ::nmsp::_log;
using ::nmsp::_notYetSupported;
using ::nmsp::_todo;

}
#endif