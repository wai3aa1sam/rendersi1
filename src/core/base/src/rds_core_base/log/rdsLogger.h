#pragma once

#include "rds_log_common.h"

#if RDS_DEBUG
	#define RDS_CORE_LOG(...)			do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Info,		__VA_ARGS__); } while(false)
	#define RDS_CORE_LOG_DEBUG(...)	    do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Debug,	    __VA_ARGS__); } while(false)
	#define RDS_CORE_LOG_WARN(...)		do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Warning,	__VA_ARGS__); } while(false)
	#define RDS_CORE_LOG_ERROR(...)	    do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Error,	    __VA_ARGS__); } while(false)

	#define RDS_LOG(...)				do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Info,	    __VA_ARGS__); } while(false)
	#define RDS_LOG_DEBUG(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Debug,	__VA_ARGS__); } while(false)
	#define RDS_LOG_WARN(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Warning,  __VA_ARGS__); } while(false)
	#define RDS_LOG_ERROR(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Error,	__VA_ARGS__); } while(false)
#else
	#define RDS_CORE_LOG(...)			
	#define RDS_CORE_LOG_DEBUG(...)	
	#define RDS_CORE_LOG_WARN(...)		
	#define RDS_CORE_LOG_ERROR(...)	

	#define RDS_LOG(...)				
	#define RDS_LOG_DEBUG(...)			
	#define RDS_LOG_WARN(...)			
	#define RDS_LOG_ERROR(...)			
#endif // rds_DEBUG

#if 0
#pragma mark --- rdsLogger-Impl ---
#endif // 0
#if 1
namespace rds
{

struct Logger_CreateDesc : public Logger_Base_CreateDesc
{

};

class Logger : public Logger_Base
{
public:
	using CreateDesc = Logger_CreateDesc;

public:
	Logger(const CreateDesc& cd);
};

}
#endif

