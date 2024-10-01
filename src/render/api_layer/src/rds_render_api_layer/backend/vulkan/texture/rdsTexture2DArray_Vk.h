#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/texture/rdsTexture2DArray.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTexture2DArray_Vk-Decl ---
#endif // 0
#if 1

class Texture2DArray_Vk : public Texture_Vk<Texture2DArray>
{
	friend class	RenderContext_Vk;
	friend class	Vk_Swapchain;
	friend struct	Vk_Texture;
public:
	using Base = Texture_Vk<Texture2DArray>;
	using Base::size;

public:
	Texture2DArray_Vk();
	virtual ~Texture2DArray_Vk();

public:
	Vk_ImageView_T* srvLayerVkImageViewHnd(u32 layerIndex);

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

public:
	void createRenderResource( const RenderFrameParam& rdFrameParam);
	void destroyRenderResource(const RenderFrameParam& rdFrameParam);

	virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

protected:
	Vector<Vk_ImageView, 4>	_srvLayerVkImageViews;
	//Vector<Vk_ImageView, 4>	_uavLayerVkImageViews;
};

#endif

}
#endif