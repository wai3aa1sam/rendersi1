#pragma once

#include "rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rdsMemoryContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Instance>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Instance> : public RenderApiPrimitive_Base_Vk<Vk_Instance>
{
public:
	void destroy()
	{
		vkDestroyInstance(_p, MemoryContext_Vk::instance()->allocCallbacks());
	}
};


#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_DebugUtilsMessenger> : public RenderApiPrimitive_Base_Vk<Vk_DebugUtilsMessenger>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_PhysicalDevice> : public RenderApiPrimitive_Base_Vk<Vk_PhysicalDevice>
{
public:
	void destroy()
	{

	}
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Device>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Device> : public RenderApiPrimitive_Base_Vk<Vk_Device>
{
public:
	void destroy()
	{
		vkDestroyDevice(_p, MemoryContext_Vk::instance()->allocCallbacks());
	}
};

#endif

template<> class RenderApiPrimitive_Vk<Vk_Surface>;
template<> class RenderApiPrimitive_Vk<Vk_Queue>;

#endif

#if 0
#pragma mark --- rdsRenderer_Vk-Decl ---
#endif // 0
#if 1

class Renderer_Vk : public Renderer
{
	friend class RenderContext_Vk;
public:
	using Base = Renderer;
	using Util = RenderApiUtil_Vk;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static Renderer_Vk* instance();

public:
	Renderer_Vk();
	virtual ~Renderer_Vk();

	MemoryContext_Vk*				memoryContext();
	const VkAllocationCallbacks*	allocCallbacks();

	Vk_Instance*			vkInstance();
	Vk_PhysicalDevice*		vkphysicalDevice();
	Vk_Device*				vkDevice();

	const ExtensionInfo_Vk&				extInfo()					const;
	const SwapchainAvailableInfo_Vk&	swapchainAvailableInfo()	const;
	QueueFamilyIndices&					queueFamilyIndices();

public:

protected:
	virtual SPtr<RenderContext> onCreateContext(const RenderContext_CreateDesc& cDesc);

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
	MemoryContext_Vk			_memoryContext;

	ExtensionInfo_Vk			_extInfo;
	SwapchainAvailableInfo_Vk	_swapchainAvaliableInfo;
	Vector<VkQueueFamilyProperties, QueueFamilyIndices::s_kQueueTypeCount> _queueFamilyProperties;
	QueueFamilyIndices _queueFamilyIndices;

	VkPtr<Vk_Instance>				_vkInstance;
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

inline MemoryContext_Vk*			Renderer_Vk::memoryContext()	{ return &_memoryContext; }
inline const VkAllocationCallbacks*	Renderer_Vk::allocCallbacks()	{ return _memoryContext.allocCallbacks(); }

inline Vk_Instance*			Renderer_Vk::vkInstance()			{ return _vkInstance; }
inline Vk_PhysicalDevice*	Renderer_Vk::vkphysicalDevice()		{ return _vkPhysicalDevice; }
inline Vk_Device*			Renderer_Vk::vkDevice()				{ return _vkDevice; }

inline const ExtensionInfo_Vk&			Renderer_Vk::extInfo()					const { return _extInfo; }
inline const SwapchainAvailableInfo_Vk& Renderer_Vk::swapchainAvailableInfo()	const { return _swapchainAvaliableInfo; }

inline QueueFamilyIndices&		Renderer_Vk::queueFamilyIndices()	{ return _queueFamilyIndices; }


#endif








}

#endif // RDS_RENDER_HAS_VULKAN

