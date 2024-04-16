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

const char* 
RenderApiUtil::toVkShaderStageProfile(ShaderStageFlag v)
{
	using SRC = rds::ShaderStageFlag;
	switch (v)
	{
		case SRC::Vertex:		{ return "vs_1.5"; } break;
		case SRC::Pixel:		{ return "ps_1.5"; } break;
		case SRC::Compute:		{ return "cs_1.5"; } break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
}

#endif
}


