#include "rds_core_base-pch.h"

#include "rdsLogger.h"


namespace rds
{

#if 0
#pragma mark --- rdsLogger-Impl ---
#endif // 0
#if 1

Logger* Logger::s_instance = nullptr;

Logger::CreateDesc Logger::makeCDesc() { return CreateDesc{}; }

Logger::Logger()
{

}

Logger::~Logger()
{
	RDS_CORE_LOG_DEBUG("spdlog will leak 1280 bytes");
}

Logger::Base* Logger::base() { return sCast<Base*>(instance()); }



#endif

}