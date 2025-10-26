#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"

#include "rdsRenderer.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice_CreateDesc::RenderDevice_CreateDesc()
{
	apiType				= RenderApiType::Vulkan;
	isPresent			= true;
	isMultithread		= true;
	isCompileShaderMode = false;
	
	isDebug = RDS_DEBUG;
}

bool 
RenderDevice_CreateDesc::isShaderCompileMode() const
{
	return isCompileShaderMode;
}

#endif

#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

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
	_rdDev	 = this;

	onCreate(cDesc);

	if (cDesc.isShaderCompileMode())
		return;

	RDS_CORE_ASSERT(_bindlessRscs,	"");
	RDS_CORE_ASSERT(_tsfCtx,		"");
	
	for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
	{
		auto o = makeUPtr<RenderJob>();
		_freeRdJobs.push(rds::move(o));
	}

	if (cDesc.isMultithread)
	{
		auto rdThreadCDesc = RenderThread::makeCDesc(this, JobSystem::instance());
		_rdThread.create(rdThreadCDesc);
	}

	_shaderStock.create(this);
	_textureStock.create(this);

	RDS_TODO("remove temp, save it on RenderJob");
	_rdGraph = makeUPtr<RenderGraph>();
}

void 
RenderDevice::destroy()
{
	if (!hasCreated())
		return;

	{
		RDS_TODO("remove _rdGraph, RenderGraph should be re-design and save in RenderJob");
		_rdThread.waitGpuIdle();
		_rdGraph.reset(nullptr);
	}
	
	_rdThread.destroy();
	
	_shaderStock.destroy();
	_textureStock.destroy();

	if (_tsfCtx)
	{
		_tsfCtx->destroy();
		_tsfCtx = nullptr;
	}

	if (_bindlessRscs)
	{
		_bindlessRscs->destroy();
		_bindlessRscs = nullptr;
	}

	onDestroy();

	Base::destroy();

	RDS_CORE_ASSERT(!_bindlessRscs,			"forgot to call destroy() _bindlessRscs");
	RDS_CORE_ASSERT(!_tsfCtx,				"forgot to call destroy() _tsfCtx");
	//RDS_CORE_ASSERT(_rdFrames.is_empty(),	"forgot to clear RenderFrame in derived class");
	//RDS_CORE_ASSERT(_tsfFrames.is_empty(),	"forgot to clear TransferFrame in derived class");
}

UPtr<RenderJob> 
RenderDevice::newRenderJob(RenderContext* rdCtx, u64 frameCount)
{
	UPtr<RenderJob> o;
	for (;;)
	{
		_freeRdJobs.try_pop(o);
		if (o)
			break;
		OsUtil::sleep_ms(1); RDS_TODO("pass a param here");
	}
	o->_renderGraph = _rdGraph;
	o->reset(this, rdCtx, frameCount);
	return o;
}

void 
RenderDevice::_internal_freeRenderJob(UPtr<RenderJob> rdJob)
{
	//transferContext()._internal_freeTransferFrame(rds::move(rdJob->transferFrame));
	_freeRdJobs.push(rds::move(rdJob));
}

void 
RenderDevice::submitRenderJob(UPtr<RenderJob> rdJob)
{
	_tsfCtx->submit(rdJob);
	if (adapterInfo().isMultiThread)
	{
		_rdThread.requestRender(rds::move(rdJob));
	}
	else
	{
		_rdThread.render(rds::move(rdJob));
	}
}

void 
RenderDevice::onCreate(const CreateDesc& cDesc)
{
	_adapterInfo.isDebug		= cDesc.isDebug;
	_adapterInfo.isMultiThread	= cDesc.isMultithread;
}

void 
RenderDevice::onDestroy()
{
	
}

void 
RenderDevice::reset(u64 frameCount)
{
	RDS_TODO("remove, make a Base class FrameParam and separate frame, also, do not use this frame for Material");

	checkRenderThreadExclusive(RDS_SRCLOC);

	renderFrameParam().reset(frameCount);
	onResetFrame(frameCount);
}

void
RenderDevice::resetEngineFrame(u64 engineFrameCount)
{
	RDS_TODO("remove, make a Base class FrameParam and separate frame, also, do not use this frame for Material");

	checkMainThreadExclusive(RDS_SRCLOC);

	auto&	rdFrameParam	= renderFrameParam();
	auto	frameCount		= engineFrameCount;
	//auto	frameIdx		= Traits::rotateFrame(frameCount);
	rdFrameParam.setEngineFrameCount(frameCount);
}

void 
RenderDevice::waitIdle()
{
	_rdThread.waitIdle();
}

void 
RenderDevice::waitCpuIdle()
{
	while (_freeRdJobs.size() != s_kMaxFrameAheadCountHardLimit)
	{
		OsUtil::sleep_ms(1);
	};
}

void
RenderDevice::waitGpuIdle()
{
	_rdThread.waitGpuIdle();
}

void 
RenderDevice::waitRenderThreadIdle()
{
	_rdThread.waitCpuIdle();
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

TransferRequest&		RenderDevice::transferRequest()				{ checkMainThreadExclusive(RDS_SRCLOC); return transferFrame().transferRequest(); }
TransferFrame&			RenderDevice::transferFrame()				{ checkMainThreadExclusive(RDS_SRCLOC);	return transferContext().transferFrame(); }

}
