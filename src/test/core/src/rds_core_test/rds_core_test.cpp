#include "rds_core_test-pch.h"



#if !RDS_ENABLE_FUZZ_TESTING

int main(int argc, char* argv[])
{
	using namespace rds;

	MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		_log("Hello World123!");
		std::cout << "Hello World!\n";
		DefaultAllocator a;
	}

	return exitCode;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
	return 0;
}

#endif // !PROJNAMESPACE_ENABLE_FUZZ_TESTING