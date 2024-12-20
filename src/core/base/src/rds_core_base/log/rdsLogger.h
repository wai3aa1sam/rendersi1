#pragma once

#include "rds_log_common.h"

#include "rds_core_base/utility/rdsSingleton.h"


#define RDS_CORE_LOG(...)			do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Info,		__VA_ARGS__); } while(false)
#define RDS_CORE_LOG_WARN(...)		do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Warning,	__VA_ARGS__); } while(false)
#define RDS_CORE_LOG_ERROR(...)	    do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Error,	    __VA_ARGS__); } while(false)

#define RDS_LOG(...)				do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Info,	    __VA_ARGS__); } while(false)
#define RDS_LOG_WARN(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Warning,  __VA_ARGS__); } while(false)
#define RDS_LOG_ERROR(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Error,	__VA_ARGS__); } while(false)

#define RDS_DUMP_VAR_1(v0)					do{ RDS_LOG("DUMP_VAR: {}=[{}]",										#v0, (v0)); } while(false)
#define RDS_DUMP_VAR_2(v0, v1)				do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}]",								#v0, (v0), #v1, (v1)); } while(false)
#define RDS_DUMP_VAR_3(v0, v1, v2)			do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}]",						#v0, (v0), #v1, (v1), #v2, (v2)); } while(false)
#define RDS_DUMP_VAR_4(v0, v1, v2, v3)		do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}]",				#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3)); } while(false)
#define RDS_DUMP_VAR_5(v0, v1, v2, v3, v4)	do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]",	#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3), #v4, (v4)); } while(false)

#define RDS_DUMP_VAR_6(v0, v1, v2, v3, v4, v5)			do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]",						\
														#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3), #v4, (v4), #v5, (v5)); } while(false)
#define RDS_DUMP_VAR_7(v0, v1, v2, v3, v4, v5, v6)		do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]",				\
														#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3), #v4, (v4), #v5, (v5), #v6, (v6)); } while(false)
#define RDS_DUMP_VAR_8(v0, v1, v2, v3, v4, v5, v6, v7)	do{ RDS_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]",		\
														#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3), #v4, (v4), #v5, (v5), #v6, (v6), #v7, (v7)); } while(false)

#define RDS_DUMP_VAR_SELECT(COUNT) RDS_DUMP_VAR_##COUNT
#define RDS_DUMP_VAR(...) RDS_IDENTITY(RDS_CALL(RDS_DUMP_VAR_SELECT, RDS_VA_ARGS_COUNT(__VA_ARGS__) (__VA_ARGS__)))

#define RDS_WARN_ONCE(...)		do { static bool RDS_VAR_NAME(_temp) = false; if (!RDS_VAR_NAME(_temp)) { RDS_CORE_LOG_WARN(__VA_ARGS__); RDS_VAR_NAME(_temp) = true; } } while(false)
#define RDS_TODO(...)			do { static bool RDS_VAR_NAME(_temp) = false; if (!RDS_VAR_NAME(_temp)) { RDS_CORE_LOG_DEBUG("{}", _todo(RDS_SRCLOC, RDS_ARGS(__VA_ARGS__))); RDS_VAR_NAME(_temp) = true; } } while(false)

#if RDS_DEBUG || RDS_DEVELOPMENT
	#define RDS_CORE_LOG_DEBUG(...)	    do{ ::rds::Logger::instance()->coreLog(::rds::LogLevel::Debug,	    __VA_ARGS__); } while(false)

	#define RDS_LOG_DEBUG(...)			do{ ::rds::Logger::instance()->clientLog(::rds::LogLevel::Debug,	__VA_ARGS__); } while(false)
#else
	/*#define RDS_CORE_LOG(...)			
	#define RDS_CORE_LOG_DEBUG(...)	
	#define RDS_CORE_LOG_WARN(...)		
	#define RDS_CORE_LOG_ERROR(...)	

	#define RDS_LOG(...)				
	#define RDS_LOG_DEBUG(...)			
	#define RDS_LOG_WARN(...)			
	#define RDS_LOG_ERROR(...)		*/

	#define RDS_CORE_LOG_DEBUG(...)	
	#define RDS_LOG_DEBUG(...)			

#endif // RDS_DEBUG


namespace rds
{
#if 0
#pragma mark --- rdsLogger-Decl ---
#endif // 0
#if 1

struct Logger_CreateDesc : public Logger_Base_CreateDesc
{

};

class Logger: public Singleton<Logger, Logger_Base>
{
public:
	using Base			= Logger_Base;
	using CreateDesc	= Logger_CreateDesc;

public:
	static CreateDesc makeCDesc();

public:
	Logger();
	~Logger();

protected:
	static Base* base();

protected:

};

#endif

#if 0
#pragma mark --- rdsLogger-Impl ---
#endif // 0
#if 1



#endif
}

