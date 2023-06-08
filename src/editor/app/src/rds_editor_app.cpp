#include "rds_editor_app-pch.h"

#include "rds_editor_app.h"

int main(int argc, char* argv[])
{
	using namespace rds;

	::rds::MemoryLeakReportScope reportScope;

	int exitCode = 0;
	{
		MemoryContext mc;
		mc.create();
		{
			auto cdesc = EditorApp::makeCDesc();
			EditorApp app;
			exitCode = app.run(cdesc);
		}
		mc.destroy();
	}

	return exitCode;
}