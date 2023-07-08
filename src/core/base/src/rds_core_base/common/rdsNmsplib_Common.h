#pragma once

//#include <nmsp_core.h>

#include <nmsp_core_base.h>

//#include <nmsp_stl.h>
//#include <nmsp_math.h>
//#include <nmsp_os.h>
//#include <nmsp_profiler.h>
//#include <nmsp_log.h>
//#include <nmsp_file_io.h>
//#include <nmsp_job_system.h>

#include <nmsp_core.h>

#include "nmsp_core_base/common/nmspBitUtil.h"

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{

using NmspCoreBaseTraits        = ::nmsp::CoreBaseTraits;
using NmspStlTraits             = ::nmsp::StlTraits;
using NmspOsTraits              = ::nmsp::OsTraits;
using NmspJobSystemTraits       = ::nmsp::JobSystemTraits;
using NmspFileIoTraits          = ::nmsp::FileIoTraits;
using NmspLogTraits             = ::nmsp::LogTraits;
using NmspMathTraits            = ::nmsp::MathTraits;
using NmspProfilerTraits        = ::nmsp::ProfilerTraits;

struct OsTraits : public NmspJobSystemTraits
{

};

}
#endif