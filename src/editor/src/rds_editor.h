#pragma once

#include "rds_editor/common/rds_editor_common.h"


#include <rds_editor/app/rdsEditorLayer.h>
#include <rds_editor/app/rdsEditorApp.h>

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