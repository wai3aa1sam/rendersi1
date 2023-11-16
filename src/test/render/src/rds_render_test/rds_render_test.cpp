#include "rds_render_test-pch.h"


void run_temp_test()
{
	RDS_RUN_TEST(test_Vulkan);
}

// TODO: add a test manager class, then no need add to here
void run_all_test()
{

}

#if !NMSP_ENABLE_FUZZ_TESTING

int main(int argc, char* argv[])
{
	using namespace rds;

	int exitCode = 0;
	{
		#if 1
		TestScope testScope {run_temp_test, false, RDS_ENABLE_BENCHMARK, argc, argv };
		#else
		TestScope testScope {run_all_test, false, RDS_ENABLE_BENCHMARK, argc, argv };
		#endif // 0
	}
	
	return exitCode;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size) 
{
	return 0;
}

#endif // !NMSP_ENABLE_FUZZ_TESTING


