#pragma once

#include "rds_demo/common/rds_demo_common.h"


#include "rds_demo/editor/rdsDemoEditoMainWindow.h"
#include "rds_demo/editor/rdsDemoEditorLayer.h"
#include "rds_demo/editor/rdsDemoEditorApp.h"

#include "rds_demo/rdsGraphicsDemo.h"

inline 
int
runDemoApp()
{
	using namespace rds;
	using T = DemoEditorApp;

	MemoryLeakReportScope reportScope;
	int exitCode;
	{
		MemoryContext::init();
		{
			auto cDesc = T::makeCDesc();
			T app;
			exitCode = app.run(cDesc);
		}
		MemoryContext::terminate();
	}
	return exitCode;
}

int main(int argc_, char** argv_)
{
	return runDemoApp();
}

#define RDS_DEMO(T) 											\
inline															\
UPtr<GraphicsDemo> 												\
makeDemo()														\
{																\
	DemoEditorApp::instance()->mainWindow().setWindowTitle(#T);	\
	return makeUPtr<T>();										\
}																\
// ---