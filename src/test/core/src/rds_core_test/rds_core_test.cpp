#include "rds_core_test-pch.h"

#include <rds_core_base/job_system/rds_job_system.h>

#if !RDS_ENABLE_FUZZ_TESTING

int main(int argc, char* argv[])
{
	using namespace rds;

	::rds::MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		//_log("Hello World123!");
		std::cout << "Hello World!\n";
		
		{
			MemoryContext::init();

			JobSystem::init();
			JobSystem::instance()->create(sCast<int>(OsTraits::logicalThreadCount()));

			
			Logger::init();
			Logger::instance()->create(Logger::makeCDesc());
			//RDS_LOG("{}, {}", "Hello World", 10);

			{
				auto p = makeUPtr<int>();
				_NMSP_DUMP_VAR(*p);
			}
			Logger::terminate();
		}

	}

	return exitCode;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
	return 0;
}

#endif // !PROJNAMESPACE_ENABLE_FUZZ_TESTING