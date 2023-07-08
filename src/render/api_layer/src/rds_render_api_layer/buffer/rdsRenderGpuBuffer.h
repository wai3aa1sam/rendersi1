#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuBuffer-Decl ---
#endif // 0
#if 1


#define RenderGpuBufferType_ENUM_LIST(E) \
	E(None, = 0) \
	E(Vertex,) \
	E(Index,) \
	E(Const,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderGpuBufferType, u8);

struct RenderGpuBuffer_CreateDesc
{

};

class RenderGpuBuffer : public RefCount_Base
{
public:
	using Base = RefCount_Base;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	using Util = RenderApiUtil;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static CreateDesc				makeCDesc();
	static SPtr<RenderGpuBuffer>	make(const CreateDesc& cDesc);

public:
	RenderGpuBuffer();
	virtual ~RenderGpuBuffer();

	void create(const CreateDesc& cDesc);
	void destroy();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

protected:

};

#endif
}

