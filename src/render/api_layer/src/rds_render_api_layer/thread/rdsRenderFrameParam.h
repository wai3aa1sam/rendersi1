#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class RenderContext;

#if 0
#pragma mark --- rdsRenderFrameParam-Decl ---
#endif // 0
#if 1

class RenderFrameParam : public NonCopyable
{
public:
	void reset(u64 engineFrameCount);
	void commit();

public:
	void setEngineFrameCount(u64 frameCount);
	void setFrameCount(u64 frameCount);

public:
	u64 engineFrameCount()	const;
	u64 frameCount()		const;
	u64 frameIndex()		const;

	/*
	* only use in engine thread
	*/
private:
	u64 _egFrameCount = 0;

private:
	u64 _frameCount = 0;
};


#endif
}