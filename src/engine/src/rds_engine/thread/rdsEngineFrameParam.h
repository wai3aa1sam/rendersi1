#pragma once

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{

class RenderContext;
class EngineFrameParam;

//class EngineFrameParamScope
//{
//public: 
//	EngineFrameParamScope(EngineFrameParam* egFrame);
//	~EngineFrameParamScope();
//
//private:
//	EngineFrameParam* _egFrameParam = nullptr;
//};

#if 0
#pragma mark --- rdsEngineFrameParam-Decl ---
#endif // 0
#if 1

/*
* TODO: RenderContextManager for each RenderDevice, instead of pass a render context
*/
class EngineFrameParam : public NonCopyable
{
public:
	//RDS_NODISCARD EngineFrameParamScope makeScope(RenderContext* rdCtx);

public:
	void reset(RenderContext* rdCtx);
	void commit();

public:
	u64 frameCount() const;

private:
	u64 _frameCount = 0;
};


#endif
}