#include "rds_editor_app-pch.h"

#include "rds_editor_app.h"

int main(int argc, char* argv[])
{
	using namespace rds;

	::rds::MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		MemoryContext::init();
		{
			auto cdesc = EditorApp::makeCDesc();
			EditorApp app;
			exitCode = app.run(cdesc);
		}
		MemoryContext::terminate();
	}

	return exitCode;
}