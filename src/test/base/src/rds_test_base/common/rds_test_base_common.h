#pragma once

#include <nmsp_test_base.h>

#include <rds_core_base.h>

#include "rds_test_base-config.h"

#define RDS_RUN_TEST(FUNC) NMSP_RUN_TEST(FUNC)

#define	RDS_TEST_CASE(TEST_CLASS, TEST_FUNC)	NMSP_TEST_CASE(TEST_CLASS, TEST_FUNC)
#define RDS_TEST_CHECK(A)						NMSP_TEST_CHECK(A)
#define RDS_TEST_CHECK_SLIENT(A)				NMSP_TEST_CHECK_SLIENT(A)

#define RDS_REGISTER_UNIT_TEST_CLASS(X, ...) NMSP_REGISTER_UNIT_TEST_CLASS(X, __VA_ARGS__)

namespace rds
{

using ::nmsp::UnitTest;
using ::nmsp::UnitTestManager;

class TestScope
{
public:
	template<class FUNC>
	TestScope(FUNC&& func, bool enableLog, bool enableBenchmark, int& argc, char* argv[])
	{
		_enableLog = enableLog;
		_enableProjectSetting = false;
		MemoryLeakReportScope reportScope;
		create();
		{
			RDS_PROFILE_FRAME();
			{
				func();
			}
			if (enableBenchmark)
			{
				UnitTestManager::instance()->benchmark(&argc, argv);
				_log("UnitTestManager will leak up to 1784 bytes");
			}
		}
		destroy();
	}

	void create()
	{
		_isDestroyed = false;

		MemoryContext::init();
		

		if (_enableLog)
		{
			Logger::init();
			Logger::instance()->create(Logger::makeCDesc());
		}

		if (_enableProjectSetting)
		{
			//ProjectSetting_T::init();

			App_Base app;
			String file = app.getExecutableFilename();
			String path = Path::dirname(file);

			auto proj = ProjectSetting_T {};

			path.append("/../../../../../..");
			proj.setProjectRoot(path);

			path.append("/example/Test000");
			proj.setProjectRoot(path);
		}
		
		_unitTestManager.create();
	}

	void destroy()
	{
		if (_isDestroyed)
		{
			return;
		}

		_log("Profiler will continue increase process memory, Sanitizer will use many cpu rate");

		_unitTestManager.destroy();
		/*if (_enableProjectSetting)
			ProjectSetting_T::terminate();*/
		if (_enableLog)
			Logger::terminate();
		MemoryContext::terminate();
		_isDestroyed = true;
	}

	void setEnableLog(bool v) { _enableLog = v; }

	~TestScope()
	{
		
		destroy();
	}

private:
	bool _isDestroyed			: 1;
	bool _enableLog				: 1;
	bool _enableProjectSetting	: 1;
	UnitTestManager _unitTestManager;
};



}