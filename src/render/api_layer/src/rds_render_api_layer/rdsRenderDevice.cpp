#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"

#include "rdsRenderer.h"
#include "rdsRenderContext.h"

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
	_rdDev	 = this;

	onCreate(cDesc);

	if (cDesc.isShaderCompileMode())
		return;

	/*for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		_tsfCtx->transferRequest(i).reset(_tsfCtx, &_tsfFrames[i]);
	}*/
	//_tsfFrame = transferContext().allocTransferFrame();

	_shaderStock.create(this);
	_textureStock.create(this);

	RDS_CORE_ASSERT(_bindlessRscs,	"");
	RDS_CORE_ASSERT(_tsfCtx,		"");

	if (cDesc.isMultithread)
	{
		auto rdThreadCDesc = RenderThread::makeCDesc(JobSystem::instance());
		_rdThread.create(rdThreadCDesc);
	}
}

void 
RenderDevice::destroy()
{
	if (!hasCreated())
		return;

	waitIdle();

	_shaderStock.destroy();
	_textureStock.destroy();

	//_rdFrames.clear();
	//_tsfFrames.clear();
	//_tsfFrame.reset(nullptr);

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

void 
RenderDevice::onCreate(const CreateDesc& cDesc)
{
	_adapterInfo.isDebug		= cDesc.isDebug;
	_adapterInfo.isMultiThread	= cDesc.isMultithread;

	#if 0
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
	#endif // 0
}

void 
RenderDevice::onDestroy()
{
	
}

void 
RenderDevice::reset(u64 frameCount)
{
	checkRenderThreadExclusive(RDS_SRCLOC);

	renderFrameParam().reset(frameCount);
	onResetFrame(frameCount);
}

void 
RenderDevice::createRenderJob(u64 frameCount, bool isRetry)
{
	{
		_rdJob = _rdThread.newRenderJob(this, frameCount);

		// retry if cannot get
		if (!_rdJob && isRetry)
		{
			waitCpuIdle(0);
			_rdJob = _rdThread.newRenderJob(this, frameCount);
		}
	}

	{
		auto&	rdFrameParam	= renderFrameParam();
		//auto	frameIdx		= Traits::rotateFrame(frameCount);
		rdFrameParam.setEngineFrameCount(frameCount);

		// RDS_TODO("maybe no need to wait if all gpu stuff is a command");
		/*
		* seems must wait regardless			
		* all gpu cmd or not, since				
		* we must wait gpu prev frame to finish	
		*/

		//_rdFrames[frameIdx].reset();
		//_tsfFrames[frameIdx].reset(_tsfCtx);
		//onResetFrame(frameCount);

		//auto tsfFrameCDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
		//_tsfFrame = createTransferFrame(tsfFrameCDesc);
		//RDS_ASSERT(!_tsfFrame, "not yet submit transferFrame ?");
		//_tsfFrame = transferContext().allocTransferFrame();
	}
}

void
RenderDevice::submitRenderJob()
{
	_rdThread.requestRender(rds::move(_rdJob));
}

void
RenderDevice::waitCpuIdle(int sleepMs) const
{
	//RDS_PROFILE_DYNAMIC_FMT("wait render thread i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);
	if (adapterInfo().isMultiThread)
	{
		OsUtil::sleep_ms(sleepMs);
	}
}

//void 
//RenderDevice::waitGpuIdle(RenderContext* rdCtx, u64 frameCount, int sleepMs) const
//{
//	if (rdCtx)
//	{
//		rdCtx->waitFrameFinished(frameCount);
//	}
//}

void 
RenderDevice::waitIdle()
{
	waitCpuIdle(0);
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
