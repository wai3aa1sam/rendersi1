#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"

#include "rdsRenderer.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

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

	auto bindlessRscVk_cDesc = BindlessResources::makeCDesc();
	_bindlessRscs = createBindlessResources(bindlessRscVk_cDesc);

	auto TransferContext_cDesc = TransferContext::makeCDesc();
	_tsfCtx = createTransferContext(TransferContext_cDesc);

	if (cDesc.isShaderCompileMode())
		return;
	
	for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
	{
		RenderJob_CreateDesc rdJob_cDesc = {};
		rdJob_cDesc.renderDevice = this;
		auto o = createRenderJob(rdJob_cDesc);
		_freeRdJobs.push(rds::move(o));
	}

	auto rdThreadCDesc = RenderThread::makeCDesc(this, JobSystem::instance());
	_rdThread.create(rdThreadCDesc);
	if (!cDesc.isMultithread)
	{
		_rdThread.quit();
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

	_shaderStock.destroy();
	_textureStock.destroy();

	_debug.rdRscs.clear();

	_rdThread.destroy();

	#if 1
	Vector< UPtr<RenderJob> > v;
	for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
	{
		v.emplace_back(_rdDev->newRenderJob(nullptr, i));
	}
	v.clear();
	#endif // 0

	
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
RenderDevice::_internal_createRenderResource(RenderResource* rdRsc)
{
	_debug.rdRscs.emplace_back(rdRsc);
}

void 
RenderDevice::submitRenderJob(UPtr<RenderJob> rdJob)
{
	RDS_TODO("**** must wait all async upload stuff when submit");
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

void 
RenderDevice::onResetFrame(u64 frameCount)
{
}

#endif

#if 0
#pragma mark --- rdsRenderDevice::RenderResource-Impl ---
#endif // 0
#if 1

UPtr<RenderJob> 
RenderDevice::createRenderJob(RenderJob_CreateDesc& cDesc)
{
	auto p = onCreateRenderJob(cDesc);
	p->create(cDesc);
	return p;
}

#endif // 1

TransferRequest&		RenderDevice::transferRequest()				{ checkMainThreadExclusive(RDS_SRCLOC); return transferFrame().transferRequest(); }
TransferFrame&			RenderDevice::transferFrame()				{ checkMainThreadExclusive(RDS_SRCLOC);	return transferContext().transferFrame(); }

}
