#include "rds_render_api_layer-pch.h"
#include "rdsRenderFrameParam.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderFrameParam-Impl ---
#endif // 0
#if 1

void 
RenderFrameParam::reset(u64 engineFrameCount)
{
	// TODO: reset in RenderThread only
	checkRenderThreadExclusive(RDS_SRCLOC);
	_frameCount		= engineFrameCount;
}

void 
RenderFrameParam::commit()
{
	
}

void 
RenderFrameParam::setEngineFrameCount(u64 frameCount)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	_egFrameCount = frameCount;
}

void 
RenderFrameParam::setFrameCount(u64 frameCount)
{
	_frameCount = frameCount;
}

u64 
RenderFrameParam::engineFrameCount() const
{
	checkMainThreadExclusive(RDS_SRCLOC);
	return _egFrameCount;
}

u64 
RenderFrameParam::frameCount() const
{
	checkRenderThreadExclusive(RDS_SRCLOC);
	return _frameCount;
}

u32
RenderFrameParam::frameIndex() const
{
	return sCast<u32>(RenderApiLayerTraits::rotateFrame(frameCount()));
}


#endif

}