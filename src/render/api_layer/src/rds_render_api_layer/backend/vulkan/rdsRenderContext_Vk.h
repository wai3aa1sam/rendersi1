#pragma once

#include "rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderContext
{
public:
	using Base = RenderContext;

	using Util = RenderApiUtil_Vk;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();


private:

};

#endif
}

#endif // RDS_RENDER_HAS_VULKAN


