#pragma once

#include "rds_core_base/common/rds_core_base_common.h"


#if 0
#pragma mark --- rds_job_system-Impl ---
#endif // 0
#if 1
namespace rds
{

using JobSystem             = ::nmsp::JobSystem_T;
using JobSystem_CreateDesc  = ::nmsp::JobSystem_CreateDesc;

using JobArgs			= ::nmsp::JobArgs;
using Job_Base			= ::nmsp::Job_Base;
using JobFor_Base		= ::nmsp::JobFor_Base;
using JobParFor_Base	= ::nmsp::JobParFor_Base;

using JobCluster = ::nmsp::JobCluster;
template<class JOB_BASE> using JobCluster_Base = ::nmsp::JobCluster_Base<JOB_BASE>;
using ::nmsp::dispatchJobCluster;
using ::nmsp::prepareJobCluster;

using Job           = ::nmsp::Job_T;
using JobHandle     = ::nmsp::JobHandle_T;

using JobFlow		= ::nmsp::JobFlow_T;

}
#endif


