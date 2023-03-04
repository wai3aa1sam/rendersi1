#include <rds_base_test-pch.h>

#if RDS_OS_WINDOWS

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _CLIENT_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#endif // RDS_OS_WINDOWS


int main(int argc, char* argv[])
{
	#if RDS_OS_WINDOWS
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	_CrtMemState checkPoint1; (void)(checkPoint1);
	_CrtMemCheckpoint(&checkPoint1);
	#endif // RDS_OS_WINDOWS

	int exitCode = 0;
	{
		std::cout << "Hello World!\n";
		nmsp::log("Hello World!\n");
	}

	#if RDS_OS_WINDOWS
	_CrtMemState checkPoint2; (void)(checkPoint2);
	_CrtMemCheckpoint(&checkPoint2);

	_CrtMemState diff; (void)(diff);
	if (_CrtMemDifference(&diff, &checkPoint1, &checkPoint2)) {
		_CrtMemDumpStatistics(&diff);
	}
	#endif // RDS_OS_WINDOWS
	
	return exitCode;
}