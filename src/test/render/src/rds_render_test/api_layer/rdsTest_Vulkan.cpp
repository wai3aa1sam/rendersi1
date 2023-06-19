#include "rds_render_test-pch.h"

#include "rds_render_api_layer.h"

namespace rds 
{

class Test_Vulkan : public UnitTest
{
public:
	void test()
	{
		
	}

	virtual void onSetup() override
	{
	}

	virtual void onTest() override
	{

	}

	virtual void onBenchmark() override
	{

	}

private:
};
RDS_REGISTER_UNIT_TEST_CLASS(Test_Vulkan);

}

void test_Vulkan()
{
	RDS_PROFILE_SECTION("test_Vulkan");

	using namespace rds;
	RDS_TEST_CASE(Test_Vulkan, test());
}




