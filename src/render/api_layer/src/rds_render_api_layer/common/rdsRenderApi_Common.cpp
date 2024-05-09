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
		case SRC::Vertex:					{ return "vs_1_5"; }	break;
		case SRC::TessellationControl:		{ return "tesc_1_5"; }	break;
		case SRC::TessellationEvaluation:	{ return "tese_1_5"; }	break;
		case SRC::Geometry:					{ return "geom_1_5"; }	break;
		case SRC::Pixel:					{ return "ps_1_5"; }	break;
		case SRC::Compute:					{ return "cs_1_5"; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
}

const char* 
RenderApiUtil::toDx12ShaderStageProfile(ShaderStageFlag v)
{
	using SRC = ShaderStageFlag;
	switch (v)
	{
		case SRC::Vertex:					{ return "vs_6_0"; } break;
		case SRC::TessellationControl:		{ return "hs_6_0"; } break;
		case SRC::TessellationEvaluation:	{ return "ds_6_0"; } break;
		case SRC::Geometry:					{ return "gs_6_0"; } break;
		case SRC::Pixel:					{ return "ps_6_0"; } break;
		case SRC::Compute:					{ return "cs_6_0"; } break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
}

const char* 
RenderApiUtil::toShaderFormat(RenderApiType v)
{
	using SRC = RenderApiType;

	//const char* stageProfile = nullptr;
	switch (v)
	{
		//case SRC::OpenGL:	{} break;
		//case SRC::Dx11:		{} break;
		//case SRC::Metal:	{} break;
		case SRC::Vulkan:	{ return "spirv"; } break;
		case SRC::Dx12:		{ return "dx12"; }	break;
		default: { RDS_THROW("getShaderFormat failed"); } break;
	}
}

#endif
}


