#include "rds_editor_app-pch.h"

#include "rds_editor_app.h"

int main(int argc, char* argv[])
{
	using namespace rds;

	::rds::MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		auto cdesc = EditorApp::makeCDesc();
		EditorApp app;
		app.run(cdesc);
	}

	return exitCode;
}