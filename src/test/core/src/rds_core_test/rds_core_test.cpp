#include "rds_core_test-pch.h"

#if !RDS_ENABLE_FUZZ_TESTING

int main(int argc, char* argv[])
{
	using namespace rds;

	::nmsp::MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		//_log("Hello World123!");
		std::cout << "Hello World!\n";
		
		{
			MemoryContext mc;
			mc.create();

			JobSystem jsys;
			jsys.create(sCast<int>(OsTraits::logicalThreadCount()));

			Logger logger;
			logger.create(Logger::makeCDesc());
			//RDS_LOG("{}, {}", "Hello World", 10);
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