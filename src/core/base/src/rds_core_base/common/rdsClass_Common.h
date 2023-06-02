#pragma once

#include "rdsNmsplib_Common.h"

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{
    
using NonCopayble   = ::nmsp::NonCopyable;
using SrcLoc        = ::nmsp::SrcLoc;
using RefCount_Base = ::nmsp::RefCount_Base;

template<class FUNC_BEGIN, class FUNC_END>  using ScopedAction      = ::nmsp::ScopedAction<FUNC_BEGIN, FUNC_END>;
template<class FUNC_END>                    using LeaveScopeAction  = ::nmsp::LeaveScopeAction<FUNC_END>;
using ::nmsp::makeLeaveScopeAction;

}
#endif