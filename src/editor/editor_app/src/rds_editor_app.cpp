#include "rds_editor_app-pch.h"

#include "rds_editor_app.h"

int main(int argc, char* argv[])
{
	using namespace rds;

	::rds::MemoryLeakReportScope reportScope;

	int exitCode;
	{
		exitCode = runApp<EditorApp>();
	}

	return exitCode;
}