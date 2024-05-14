#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/texture/rdsTexture3D.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTexture3D_Vk-Decl ---
#endif // 0
#if 1

class Texture3D_Vk : public Texture_Vk<Texture3D>
{
	friend class	RenderContext_Vk;
	friend class	Vk_Swapchain;
	friend struct	Vk_Texture;
public:
	using Base = Texture_Vk<Texture3D>;
	using Base::size;

public:
	Texture3D_Vk();
	virtual ~Texture3D_Vk();

public:
	virtual void setDebugName(StrView name) override;
	virtual void setNull() override;

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;
};

#endif

}
#endif