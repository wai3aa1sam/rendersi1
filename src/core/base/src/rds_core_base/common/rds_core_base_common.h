#pragma once

#include "rds_core_base-config.h"

#include "../detect_platform/rds_detect_platform.h"

#include "rdsDataType_Common.h"
#include "rdsError_Common.h"

#if 0
#pragma mark --- XXXX-Decl
#endif // 0
#if 1

#endif

#if 0
#pragma mark --- XXXX-Impl
#endif // 0
#if 1


#endif

namespace rds {


using ::nmsp::CoreBaseTraits;
using ::nmsp::StlTraits;
using ::nmsp::OsTraits;
using ::nmsp::JobSystemTraits;

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
using NonCopayble   = ::nmsp::NonCopyable;
using SrcLoc        = ::nmsp::SrcLoc;
using RefCount_Base = ::nmsp::RefCount_Base;

template<class FUNC_BEGIN, class FUNC_END>  using ScopedAction      = ::nmsp::ScopedAction<FUNC_BEGIN, FUNC_END>;
template<class FUNC_END>                    using LeaveScopeAction  = ::nmsp::LeaveScopeAction<FUNC_END>;
using ::nmsp::makeLeaveScopeAction;
#endif

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
using ::nmsp::BitUtil;
#endif

using DefaultAllocator = ::nmsp::DefaultAllocator;

}