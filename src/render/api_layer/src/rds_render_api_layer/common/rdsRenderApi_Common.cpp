#include "rds_render_api_layer-pch.h"
#include "rdsRenderApi_Common.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiUtil-Impl ---
#endif // 0
#if 1

void 
RenderApiUtil::createTempWindow(NativeUIWindow& out)
{
	auto cDesc = NativeUIWindow::makeCDesc();
	cDesc.isMainWindow	= false;
	cDesc.isVisible		= false;
	out.create(cDesc);
}

#endif
}


