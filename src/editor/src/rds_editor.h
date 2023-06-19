#pragma once

#include "rds_editor/common/rds_editor_common.h"


#include <rds_editor/app/rdsEditorLayer.h>
#include <rds_editor/app/rdsEditorApp.h>

namespace rds
{

inline 
i32
runApp()
{
	i32 exitCode;
	MemoryContext::init();
	{
		auto cdesc = EditorApp::makeCDesc();
		EditorApp app;
		exitCode = app.run(cdesc);
	}
	MemoryContext::terminate();
	return exitCode;
}

}