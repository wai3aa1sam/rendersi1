#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class RenderFrameParam;
class RenderDevice;

struct RenderResourcesContext_CreateDesc
{
	RenderDevice* renderDevice = nullptr;
};

#if 0
#pragma mark --- rdsUploadContext-Decl ---
#endif // 0
#if 1

class RenderResourcesContext : public NonCopyable
{
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderResourcesContext_CreateDesc;

	using CreateQueue	= Vector<SPtr<RenderResource>>;
	using DestroyQueue	= Vector<RenderResource*>;

public:
	static CreateDesc makeCDesc(RenderDevice* rdDev);

public:
	RenderResourcesContext();
	virtual ~RenderResourcesContext();

	void create(const CreateDesc& cDesc);
	void destroy();

	void commit(const RenderFrameParam& rdFrameParam);

	void createRenderResource( RenderResource* rdResource);
	void destroyRenderResoruce(RenderResource* rdResource);

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onCommit(const RenderFrameParam& rdFrameParam);

protected:
	template<class CTX> void _dispatch_RenderResource_Create(	CTX* ctx, RenderResource* rdRsc);
	template<class CTX> void _dispatch_RenderResource_Destroy(	CTX* ctx, RenderResource* rdRsc);

protected:
	RenderDevice*	_rdDev		= nullptr;
	u32				_frameIdx	= 0;

	MutexProtected<CreateQueue>		_createQueue;
	MutexProtected<DestroyQueue>	_destroyQueue;
};

template<class CTX> inline
void 
RenderResourcesContext::_dispatch_RenderResource_Create(CTX* ctx, RenderResource* rdRsc)
{
	using SRC = RenderResourceType;

	#define _DISPACH_CMD_CASE(TYPE) case SRC::TYPE: { ctx->RDS_CONCAT(RDS_CONCAT(onRenderResource_, Create_), TYPE)(sCast<TYPE*>(rdRsc)); } break
	switch (rdRsc->renderResourceType())
	{
		_DISPACH_CMD_CASE(RenderGpuBuffer);
		_DISPACH_CMD_CASE(Texture);
		default: { throwError("undefined render resource type"); } break;
	}
	#undef _DISPACH_CMD_CASE
}

template<class CTX> inline
void 
RenderResourcesContext::_dispatch_RenderResource_Destroy(CTX* ctx, RenderResource* rdRsc)
{
	using SRC = RenderResourceType;

	#define _DISPACH_CMD_CASE(TYPE) case SRC::TYPE: { ctx->RDS_CONCAT(RDS_CONCAT(onRenderResource_, Destroy_), TYPE)(sCast<TYPE*>(rdRsc)); } break
	switch (rdRsc->renderResourceType())
	{
		_DISPACH_CMD_CASE(RenderGpuBuffer);
		_DISPACH_CMD_CASE(Texture);
		default: { throwError("undefined render resource type"); } break;
	}
	#undef _DISPACH_CMD_CASE
}

#endif

}