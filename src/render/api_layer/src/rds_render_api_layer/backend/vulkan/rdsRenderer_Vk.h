#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rdsVk_MemoryContext.h"
#include "rdsVk_RenderFrame.h" 

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderer_Vk-Decl ---
#endif // 0
#if 1

class Renderer_Vk : public Renderer
{
	friend class RenderContext_Vk;
public:
	using Base = Renderer;
	using Util = Vk_RenderApiUtil;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static Renderer_Vk* instance();

public:
	Renderer_Vk();
	virtual ~Renderer_Vk();

	void waitIdle();

public:
	Vk_MemoryContext*				memoryContext();
	const VkAllocationCallbacks*	allocCallbacks();

	Vk_Instance_T*			vkInstance();
	Vk_PhysicalDevice*		vkPhysicalDevice();
	Vk_Device*				vkDevice();

	const Vk_ExtensionInfo&				extInfo()					const;
	const Vk_SwapchainAvailableInfo&	swapchainAvailableInfo()	const;
		  Vk_SwapchainAvailableInfo&	swapchainAvailableInfo();
	QueueFamilyIndices&					queueFamilyIndices();

public:

protected:
	virtual SPtr<RenderContext>			onCreateContext				(const RenderContext_CreateDesc&	cDesc)	override;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer		(const RenderGpuBuffer_CreateDesc&	cDesc)	override;
	virtual SPtr<Texture2D>				onCreateTexture2D			(const Texture2D_CreateDesc&		cDesc)	override;
	virtual SPtr<Shader>				onCreateShader				(const Shader_CreateDesc&			cDesc)	override;
	virtual SPtr<Material>				onCreateMaterial			()											override;
	virtual SPtr<Material>				onCreateMaterial			(Shader*							shader) override;

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

private:
	void createVkInstance();
	void createVkDebugMessenger();
	void createVkPhyDevice(const CreateDesc& cDesc);
	void createVkDevice();

	i64	 _rateAndInitVkPhyDevice(RenderAdapterInfo& out, const CreateDesc& cDesc, Vk_PhysicalDevice* vkPhyDevice, Vk_Surface* vkSurface);
	i64	 _rateVkPhyDevice		(const RenderAdapterInfo& info);

private:
	Vk_MemoryContext			_memoryContextVk;

	Vk_ExtensionInfo			_extInfo;
	Vk_SwapchainAvailableInfo	_swapchainAvaliableInfo;
	Vector<VkQueueFamilyProperties, QueueFamilyIndices::s_kQueueTypeCount> _queueFamilyProperties;
	QueueFamilyIndices _queueFamilyIndices;

	Vk_Instance						_vkInstance;
	//VkPtr<Vk_Instance_T>			_vkInstance;
	VkPtr<Vk_DebugUtilsMessenger>	_vkDebugMessenger;
	VkPtr<Vk_PhysicalDevice>		_vkPhysicalDevice;
	VkPtr<Vk_Device>				_vkDevice;
	//VkPtr<Vk_Queue>				_vkGraphicsQueue;
	//VkPtr<Vk_Queue>				_vkPresentQueue;
};

#endif

#if 0
#pragma mark --- rdsRenderer_Vk-Impl ---
#endif // 0
#if 1

inline Renderer_Vk* Renderer_Vk::instance() { return sCast<Renderer_Vk*>(s_instance); }

inline Vk_MemoryContext*			Renderer_Vk::memoryContext()	{ return &_memoryContextVk; }
inline const VkAllocationCallbacks*	Renderer_Vk::allocCallbacks()	{ return _memoryContextVk.allocCallbacks(); }

inline Vk_Instance_T*		Renderer_Vk::vkInstance()				{ return _vkInstance.hnd(); }
inline Vk_PhysicalDevice*	Renderer_Vk::vkPhysicalDevice()			{ return _vkPhysicalDevice; }
inline Vk_Device*			Renderer_Vk::vkDevice()					{ return _vkDevice; }

inline const Vk_ExtensionInfo&			Renderer_Vk::extInfo()					const { return _extInfo; }
inline const Vk_SwapchainAvailableInfo& Renderer_Vk::swapchainAvailableInfo()	const { return _swapchainAvaliableInfo; }
inline Vk_SwapchainAvailableInfo&		Renderer_Vk::swapchainAvailableInfo()		  { return _swapchainAvaliableInfo; }

inline QueueFamilyIndices&		Renderer_Vk::queueFamilyIndices()	{ return _queueFamilyIndices; }

#endif








}

#endif // RDS_RENDER_HAS_VULKAN


