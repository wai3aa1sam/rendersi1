#include "rds_engine-pch.h"
#include "rdsEngineFrameParam.h"

#include "rds_render_api_layer/thread/rdsRenderThreadQueue.h"

namespace rds
{

//EngineFrameParamScope::EngineFrameScope(EngineFrameParam* egFrameParam)
//{
//	_egFrameParam = egFrameParam;
//}
//
//EngineFrameParamScope::~EngineFrameScope()
//{
//	egFrameParam->endFrame();
//}

#if 0
#pragma mark --- rdsEngineFrameParam-Impl ---
#endif // 0
#if 1

//EngineFrameParamScope
//EngineFrameParam::makeFrameScope(RenderContext* rdCtx)
//{
//	beginFrame(rdCtx);
//	return EngineFrameParamScope{this};
//}

void 
EngineFrameParam::reset(RenderContext* rdCtx, RenderThreadQueue* renderThreadQueue)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	_frameCount++;
	auto frameCount = this->frameCount();
	wait(frameCount, rdCtx, renderThreadQueue, true);

	auto* rdDev = Renderer::renderDevice();
	rdDev->resetEngineFrame(frameCount);		// next frame here will clear those in Layer::onCreate()
	//Renderer::renderDevice()->waitIdle();
}

void 
EngineFrameParam::commit()
{
	
}

void 
EngineFrameParam::wait(u64 frameCount,RenderContext* rdCtx, RenderThreadQueue* renderThreadQueue, bool isWaitGpu)
{
	RDS_PROFILE_SCOPED();

	checkMainThreadExclusive(RDS_SRCLOC);
	
	#if 1
	{
		{
			RDS_PROFILE_DYNAMIC_FMT("wait render thread i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);
			// TODO: pick a small job instead of waiting, but maybe loop is better, if so measure the time, and call sleep if the job is too small
			renderThreadQueue->waitFrame(frameCount, 0); 
		}

		{
			RDS_PROFILE_DYNAMIC_FMT("wait gpu i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);

			// temp sol, should wait in rdDev
			// if all gpu stuff is only in RenderThread, currently cpu (main thread) has something (tsf buffer) that is related to gpu, not yet all command is impl
			// but, we must wait here, if not, engine will lead Render x frames until it finish
			// for simple, just wait here now, if want to lead frame 
			// (SafeFrame = inFlight + 1 shd ok, > this also need to wait gpu, but could delay to wait in Render, instead of here)
			// , then all things submit to Render need follow the pattern in TransferFrame
			// , eg. (RenderGraph, etc... RenderGpuMultiBuffer?, seems all buffer rotate stuff in cpu need follow that pattern)
			// , since gpu is consumer, inFlightFrameCount buffer is ok, if gpu also wait utill submit, then also need to use TransferFrame pattern
			rdCtx->waitFrameFinished(frameCount);
			RDS_TODO("could do cpu job here, btw, cannot wait on above, we must ensure Render Thread not using same frame index fence");
		}
	}

	// testing for check vk descr alloc
	#if 0
	{
		Vector<u8> data;
		{
			FileStream fs;
			fs.openWrite(ProjectSetting::instance()->shaderRecompileListPath(), false);
			auto lock = fs.scopedLock();
			data.resize(fs.fileSize());
			fs.readBytes(data);

			StrView str = "asset/shader/lighting/rdsDefaultLighting.shader";
			fs.writeBytes(makeByteSpan(str));
		}

		ShaderCompileRequest::hotReload(Renderer::instance(), JobSystem::instance(), ProjectSetting::instance());

		//OsUtil::sleep_ms(1);
	}
	#endif // 0

	#endif // 0
}

u64 
EngineFrameParam::frameCount() const 
{ 
	checkMainThreadExclusive(RDS_SRCLOC);
	return _frameCount; 
}

u32
EngineFrameParam::frameIndex() const
{
	return sCast<u32>(RenderTraits::rotateFrame(frameCount()));
}


#endif

}