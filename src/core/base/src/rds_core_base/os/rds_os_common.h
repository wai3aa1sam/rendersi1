#pragma once

#include "rds_core_base/common/rds_core_base_common.h"
#include <nmsp_os/atomic/extra/nmspMutexProtected.h>
#include <nmsp_os/common/nmspOsUtil.h>
#include <nmsp_os/atomic/extra/nmspSynchronize.h>

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{



}
#endif


#if 0
#pragma mark --- rds_os-Impl ---
#endif // 0
#if 1
namespace rds
{

using OsUtil = ::nmsp::OsUtil;

#if 0
#pragma mark --- rds_os_app-Impl ---
#endif // 0
#if 1

using App_Base			= ::nmsp::AppBase_T;
using ConsoleApp_Base	= ::nmsp::ConsoleApp_T;
using ProjectSetting	= ::nmsp::ProjectSetting_T;

#endif

#if 0
#pragma mark --- rds_os_atomic-Impl ---
#endif // 0
#if 1

using ::nmsp::NativeThread_CreateDesc;
using NativeThread = ::nmsp::NativeThread_T;

template<class T> using Atm = ::nmsp::Atm_T<T>;

template<class T> using ThreadLS = ::nmsp::ThreadLS_T<T>;

using Mutex		= ::nmsp::Mutex_T;
using SMutex	= ::nmsp::SMutex_T;

template<class T> using ULock = ::nmsp::ULock_T<T>;
template<class T> using SLock = ::nmsp::SLock_T<T>;

using CondVar	= ::nmsp::CondVar_T;
using CondVarA	= ::nmsp::CondVarA_T;

template<class T> using MutexProtected	= ::nmsp::MutexProtected_T<T>;
template<class T> using SMutexProtected	= ::nmsp::SMutexProtected_T<T>;

using CallOnce = ::nmsp::CallOnce_T;

template<class T> using Future	= ::nmsp::Future_T<T>;
template<class T> using Promise = ::nmsp::Promise_T<T>;

using ::nmsp::AtmQueueTraits;
template<class T, size_t N = 256, class ALLOC = DefaultAllocator, class TRAITS = AtmQueueTraits<N, ALLOC> > using AtmQueue			= ::nmsp::AtmQueue_T<T, N, ALLOC, TRAITS>;
template<class T, class QUEUE = AtmQueue<T> >																using AtmStealQueue		= ::nmsp::AtmStealQueue_T<T>;
template<class T, size_t N_PRIORITY, class PRIORITY, class QUEUE = AtmStealQueue<T> >						using AtmPriorityQueue	= ::nmsp::AtmPriorityQueue_T<T, N_PRIORITY, PRIORITY, QUEUE>;


#if 0
#pragma mark --- rds_os_sync-Impl ---
#endif // 0
#if 1

using CheckOnce				= ::nmsp::CheckOnce_T;
using CheckExclusive		= ::nmsp::CheckExclusive_T;
using ScopedCheckExclusive	= ::nmsp::ScopedCheckExclusive_T;

#if RDS_ENABLE_ASSERT

#define RDS_CHECK_ONCE_VAR(VAR_NAME)	CheckOnce VAR_NAME
#define RDS_CHECK_ONCE(VAR_NAME)		VAR_NAME.check()

#define RDS_CHECK_EXCLU_BEGIN_V(VAR)	VAR.acquire()
#define RDS_CHECK_EXCLU_END_V(VAR)		VAR.release()
#define RDS_CHECK_EXCLU_SCOPED_V(VAR)	ScopedCheckExclusive(VAR)

inline CheckExclusive g_checkExclu;
#define RDS_CHECK_EXCLU_BEGIN()		RDS_CHECK_EXCLU_BEGIN_V(g_checkExclu)
#define RDS_CHECK_EXCLU_END()		RDS_CHECK_EXCLU_END_V(g_checkExclu)
#define RDS_CHECK_EXCLU_SCOPED()	RDS_CHECK_EXCLU_SCOPED_V(&g_checkExclu)

#else

#define RDS_CHECK_ONCE_VAR(VAR_NAME)	
#define RDS_CHECK_ONCE(VAR_NAME)		

#define RDS_CHECK_EXCLU_BEGIN_V(VAR)	
#define RDS_CHECK_EXCLU_END_V(VAR)		
#define RDS_CHECK_EXCLU_SCOPED_V(VAR)	

#define RDS_CHECK_EXCLU_BEGIN()		
#define RDS_CHECK_EXCLU_END()		
#define RDS_CHECK_EXCLU_SCOPED()	

#endif // RDS_ENABLE_ASSERT


#endif

#endif

#if 0
#pragma mark --- rds_os_file-Impl ---
#endif // 0
#if 1

using ::nmsp::File;
using ::nmsp::FileMode;
using ::nmsp::FileAccess;
using ::nmsp::FileShareMode;

using FileStream_CreateDesc = ::nmsp::FileStream_CreateDesc;
using FileStream            = ::nmsp::FileStream_T;

using MemMapFile_CreateDesc = ::nmsp::MemMapFile_CreateDesc;
using MemMapFile            = ::nmsp::MemMapFile_T;

using ::nmsp::Path;
using ::nmsp::Directory;

#endif

#if 0
#pragma mark --- rds_os_input-Impl ---
#endif // 0
#if 1

using UiEventModifier       = ::nmsp::UiEventModifier;
using UiKeyboardEventType   = ::nmsp::UiKeyboardEventType;
using UiKeyboardEvent       = ::nmsp::UiKeyboardEvent;
using UiKeyboardEventButton = ::nmsp::UiKeyboardEventButton;

using UiMouseEventType      = ::nmsp::UiMouseEventType;
using UiMouseEventButton    = ::nmsp::UiMouseEventButton;
using UiMouseEvent          = ::nmsp::UiMouseEvent;

#endif

#if 0
#pragma mark --- rds_os_native_ui-Impl ---
#endif // 0
#if 1

using NativeUIWindow_CreateDesc = ::nmsp::NativeUIWindow_CreateDesc;
using NativeUIWindow            = ::nmsp::NativeUIWindow_T;

using NativeUIApp_CreateDesc    = ::nmsp::NativeUIApp_CreateDesc;
using NativeUIApp				= ::nmsp::NativeUIApp_T;

#endif


#if 0
#pragma mark --- rds_os_native_ui-Impl ---
#endif // 0
#if 1

using CmdLine_CreateDesc    = ::nmsp::CmdLine_CreateDesc;
using CmdLine               = ::nmsp::CmdLine_T;

using Process_CreateDesc    = ::nmsp::Process_CreateDesc;
using Process               = ::nmsp::Process_T;

#endif

}
#endif