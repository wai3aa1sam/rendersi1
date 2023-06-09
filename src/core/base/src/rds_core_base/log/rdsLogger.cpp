#include "rds_core_base-pch.h"

#include "rdsLogger.h"


namespace rds
{

#if 0
#pragma mark --- rdsLogger-Impl ---
#endif // 0
#if 1

Logger* Logger::s_instance = nullptr;

void 
Logger::create(const CreateDesc& cDesc)
{
	RDS_MALLOC_NEW(Logger)();
	base()->create(cDesc);
}

void 
Logger::destroy()
{
	base()->destroy();
	RDS_MALLOC_DELETE(instance());
}

Logger::CreateDesc Logger::makeCDesc() { return CreateDesc{}; }

Logger::Base* Logger::base() { return sCast<Base*>(instance()); }

#endif

}