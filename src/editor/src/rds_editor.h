#pragma once

#include "rds_editor/common/rds_editor_common.h"

#include "rds_editor/rdsEditorContext.h"
#include "rds_editor/ui/rdsEditorUiDrawRequest.h"
#include "rds_editor/ui/rdsEditorUi.h"

#include "rds_editor/ui/window/rdsEditorHierarchyWindow.h"
#include "rds_editor/ui/window/rdsEditorInspectorWindow.h"
#include "rds_editor/ui/window/rdsEditorProjectWindow.h"
#include "rds_editor/ui/window/rdsEditorConsoleWindow.h"
#include "rds_editor/ui/window/rdsEditorViewportWindow.h"

#include "rds_editor/app/rdsEditorLayer.h"
#include "rds_editor/app/rdsEditorApp.h"


namespace rds
{

template<class T> inline 
i32
runApp()
{
	i32 exitCode;
	MemoryContext::init();
	{
		auto cdesc = T::makeCDesc();
		T app;
		exitCode = app.run(cdesc);
	}
	MemoryContext::terminate();
	return exitCode;
}

}