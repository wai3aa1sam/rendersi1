#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rdsVk_MemoryContext.h"
#include "rdsVk_RenderFrame.h" 
#include "transfer/rdsTransferContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderDevice_Vk-Decl ---
#endif // 0
#if 1

class RenderDevice_Vk : public RenderResource_Vk<RenderDevice>
{
	friend class RenderContext_Vk;
public:
	using Base = RenderResource_Vk<RenderDevice>;
	using Util = Vk_RenderApiUtil;

	using Vk_QueueFamilyProperties = Vector<VkQueueFamilyProperties, QueueFamilyIndices::s_kQueueTypeCount>;

public:
	RenderDevice_Vk();
	virtual ~RenderDevice_Vk();

	virtual void waitIdle() override;

public:
	Vk_MemoryContext*				memoryContext();
	const VkAllocationCallbacks*	allocCallbacks();

	TransferContext_Vk&				transferContextVk();

	Vk_Instance_T*			vkInstance();
	Vk_PhysicalDevice_T*	vkPhysicalDevice();
	Vk_Device_T*			vkDevice();

	const	Vk_ExtensionInfo&			extInfo()					const;
	const	Vk_SwapchainAvailableInfo&	swapchainAvailableInfo()	const;
			Vk_SwapchainAvailableInfo&	swapchainAvailableInfo();
			QueueFamilyIndices&			queueFamilyIndices();

			RenderDevice_Vk* renderDeviceVk() { RDS_TODO("remove"); return this; }

public:

protected:
	virtual SPtr<RenderContext>			onCreateContext				(const	RenderContext_CreateDesc&	cDesc)	override;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer		(		RenderGpuBuffer_CreateDesc&	cDesc)	override;
	virtual SPtr<Texture2D>				onCreateTexture2D			(		Texture2D_CreateDesc&		cDesc)	override;
	virtual SPtr<Shader>				onCreateShader				(const	Shader_CreateDesc&			cDesc)	override;
	virtual SPtr<Material>				onCreateMaterial			(const	Material_CreateDesc&		cDesc)	override;

protected:
	virtual void onCreate	(const CreateDesc& cDesc) override;
	virtual void onDestroy	()	override;
	virtual void onNextFrame()	override;

private:
	void createVkInstance();
	void createVkPhyDevice(const CreateDesc& cDesc);
	void createVkDevice();

	i64	 _rateAndInitVkPhyDevice(RenderAdapterInfo& out, const CreateDesc& cDesc, Vk_PhysicalDevice_T* vkPhyDevHnd, Vk_Surface_T* vkSurfHnd);
	i64	 _rateVkPhyDevice		(const RenderAdapterInfo& info);

	void loadVkInstFn	(Vk_ExtensionInfo& vkExtInfo);
	void loadVkDevFn	(Vk_ExtensionInfo& vkExtInfo);

protected:
	void _setDebugName();

private:
	Vk_MemoryContext				_vkMemoryContext;

	Vk_ExtensionInfo				_vkExtInfo;
	Vk_SwapchainAvailableInfo		_vkSwapchainAvaliableInfo;
	Vk_QueueFamilyProperties		_vkQueueFamilyProps;
	QueueFamilyIndices				_queueFamilyIndices;

	Vk_Instance				_vkInstance;
	Vk_DebugUtilsMessenger	_vkDebugMessenger;
	Vk_PhysicalDevice		_vkPhysicalDevice;
	Vk_Device				_vkDevice;

	TransferContext_Vk		_transferCtxVk;
};

#endif

#if 0
#pragma mark --- rdsRenderDevice_Vk-Impl ---
#endif // 0
#if 1

inline			Vk_MemoryContext*				RenderDevice_Vk::memoryContext()					{ return &_vkMemoryContext; }
inline const	VkAllocationCallbacks*			RenderDevice_Vk::allocCallbacks()					{ return _vkMemoryContext.allocCallbacks(); }

inline			TransferContext_Vk&				RenderDevice_Vk::transferContextVk()				{ return _transferCtxVk; }

inline			Vk_Instance_T*					RenderDevice_Vk::vkInstance()						{ return _vkInstance.hnd(); }
inline			Vk_PhysicalDevice_T*			RenderDevice_Vk::vkPhysicalDevice()					{ return _vkPhysicalDevice.hnd(); }
inline			Vk_Device_T*					RenderDevice_Vk::vkDevice()							{ return _vkDevice.hnd(); }

inline const	Vk_ExtensionInfo&				RenderDevice_Vk::extInfo()					const	{ return _vkExtInfo; }
inline const	Vk_SwapchainAvailableInfo&		RenderDevice_Vk::swapchainAvailableInfo()	const	{ return _vkSwapchainAvaliableInfo; }
inline			Vk_SwapchainAvailableInfo&		RenderDevice_Vk::swapchainAvailableInfo()			{ return _vkSwapchainAvaliableInfo; }

inline			QueueFamilyIndices&				RenderDevice_Vk::queueFamilyIndices()				{ return _queueFamilyIndices; }

#endif








}

#endif // RDS_RENDER_HAS_VULKAN


