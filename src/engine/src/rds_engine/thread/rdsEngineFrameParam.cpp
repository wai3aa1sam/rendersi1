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
	_frameCount++;
	auto frameCount = this->frameCount();

	#if 1
	{
		{
			RDS_PROFILE_DYNAMIC_FMT("wait render thread i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);
			bool shdWaitRdThread = !renderThreadQueue->isSignaled(frameCount);
			bool shdWait = shdWaitRdThread;
			while (shdWait)
			{
				#if 0

				if (shdWaitRdThread)
				{
					RDS_CORE_LOG_ERROR("************ wait render thread - rdCurFrame: {}, engineFrame: {}", renderThreadQueue->currentFrameCount(), frameCount);
				}
				#endif // 0

				// TODO: pick a small job instead of waiting, if so measure the time, and call sleep if the job is too small
				OsUtil::sleep_ms(0);		// *** calling isFrameCompleted() frequently will have large overhead

				shdWaitRdThread = !renderThreadQueue->isSignaled(frameCount);
				shdWait = shdWaitRdThread; // !isRdCtxFinished;
			}
		}

		{
			RDS_PROFILE_DYNAMIC_FMT("wait gpu i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);
			rdCtx->waitFrameFinished(frameCount);		// temp sol, should wait in rdDev
		}

		auto* rdDev = Renderer::renderDevice();
		rdDev->resetEngineFrame(frameCount);		// next frame here will clear those in Layer::onCreate()
		//Renderer::renderDevice()->waitIdle();
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

void 
EngineFrameParam::commit()
{
	
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