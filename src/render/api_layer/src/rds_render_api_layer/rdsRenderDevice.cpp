#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"

#include "rdsRenderer.h"


namespace rds
{
#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice_CreateDesc::RenderDevice_CreateDesc()
{
	apiType			= RenderApiType::Vulkan;
	isPresent		= true;
	
	isDebug = RDS_DEBUG;
}

bool 
RenderDevice_CreateDesc::isShaderCompileMode() const
{
	return sCast<const Renderer_CreateDesc&>(*this).isUsingForCompileShader;
}


RenderDevice::CreateDesc RenderDevice::makeCDesc() { return CreateDesc{}; }

RenderDevice::RenderDevice()
	: Base()
{
}

RenderDevice::~RenderDevice()
{
	//RDS_LOG_DEBUG("~RenderDevice()");
}

void 
RenderDevice::create(const CreateDesc& cDesc)
{
	_apiType = cDesc.apiType;

	onCreate(cDesc);

	if (cDesc.isShaderCompileMode())
		return;

	for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		_tsfCtx->transferRequest(i).reset(_tsfCtx, &_tsfFrames[i]);
	}

	_shaderStock.create(this);
	_textureStock.create(this);

	RDS_CORE_ASSERT(_bindlessRscs,	"");
	RDS_CORE_ASSERT(_tsfCtx,		"");
	RDS_CORE_ASSERT(_rdRscsCtx,		"");
}

void 
RenderDevice::destroy()
{
	waitIdle();

	_shaderStock.destroy();
	_textureStock.destroy();

	onDestroy();

	RDS_CORE_ASSERT(!_bindlessRscs,			"forgot to call destroy() _bindlessRscs");
	RDS_CORE_ASSERT(!_tsfCtx,				"forgot to call destroy() _tsfCtx");
	RDS_CORE_ASSERT(!_rdRscsCtx,			"forgot to call destroy() _rdRscsCtx");
	RDS_CORE_ASSERT(_rdFrames.is_empty(),	"forgot to clear RenderFrame in derived class");
	RDS_CORE_ASSERT(_tsfFrames.is_empty(),	"forgot to clear TransferFrame in derived class");
}

void 
RenderDevice::onCreate(const CreateDesc& cDesc)
{
	_adapterInfo.isDebug = cDesc.isDebug;

	_rdFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _rdFrames)
	{
		e.create();
	}
	_tsfFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _tsfFrames)
	{
		e.create();
	}
}

void 
RenderDevice::onDestroy()
{
}

void 
RenderDevice::resetEngineFrame(u64 engineFrameCount)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto&	rdFrameParam	= renderFrameParam();
	auto	frameCount		= engineFrameCount;
	auto	frameIdx		= Traits::rotateFrame(frameCount);

	if (bool isFirstFrame = (frameCount <= 1))
	{
		auto& tsfCtx	= transferContext();
		tsfCtx.transferRequest(0).commit(rdFrameParam, true);		// wait here so that ok to commit here, first frame all committed to frame 0, but queue frame is start from 1
	}

	_rdFrames[frameIdx].reset();
	_tsfFrames[frameIdx].reset(_tsfCtx);
	onResetFrame(frameCount);
	
	rdFrameParam.setEngineFrameCount(frameCount);
}

void 
RenderDevice::waitIdle()
{

}

SPtr<Texture2D>	
RenderDevice::createSolidColorTexture2D(const Color4b& color)
{
	return _textureStock.createSolidColorTexture2D(color);
}

SPtr<Texture2D>	
RenderDevice::createCheckerboardTexture2D(const Color4b& color)
{
	return _textureStock.createCheckerboardTexture2D(color);

}

void 
RenderDevice::onResetFrame(u64 frameCount)
{
}

#endif


}
