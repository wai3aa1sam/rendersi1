#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"


namespace rds
{
#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice_CreateDesc::RenderDevice_CreateDesc()
{
	apiType		= RenderApiType::Vulkan;
	isPresent	= true;

	isDebug = RDS_DEBUG;
}

RenderDevice::CreateDesc RenderDevice::makeCDesc() { return CreateDesc{}; }

RenderDevice::RenderDevice()
	: Base()
{
}

RenderDevice::~RenderDevice()
{
	RDS_LOG_DEBUG("~RenderDevice()");
}

void 
RenderDevice::create(const CreateDesc& cDesc)
{
	_apiType = cDesc.apiType;

	onCreate(cDesc);

	_tsfCtx->transferRequest().reset(_tsfCtx);

	_shaderStock.create(this);
	_textureStock.create(this);

	RDS_CORE_ASSERT(_bindlessRscs, "");
}

void 
RenderDevice::destroy()
{
	waitIdle();

	_shaderStock.destroy();
	_textureStock.destroy();

	onDestroy();

	RDS_LOG_DEBUG("{}", RDS_SRCLOC);

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
RenderDevice::onNextFrame()
{
}

void 
RenderDevice::nextFrame()
{
	_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;

	// throwIf(true, "all reset should after beginRender, as it will wait that frame to finish");

	renderFrame().reset();
	transferFrame().reset();
	_tsfReq.reset(_tsfCtx);

	onNextFrame();
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


#endif


}
