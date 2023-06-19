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
		vkDestroyInstance(_p, MemoryContext_Vk::instance()->allocationCallbacks());
	}
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
		vkDestroyDevice(_p, MemoryContext_Vk::instance()->allocationCallbacks());
	}
};

#endif

#endif

#if 0
#pragma mark --- rdsRenderer_Vk-Decl ---
#endif // 0
#if 1

class Renderer_Vk : public Renderer
{
public:
	using Base = Renderer;
	using Util = RenderApiUtil_Vk;

public:
	static Renderer_Vk* instance();

public:
	Renderer_Vk();
	virtual ~Renderer_Vk();

	MemoryContext_Vk* memoryContext();

	Vk_Instance*			vkInstance();
	Vk_PhysicalDevice*		vkphysicalDevice();
	Vk_Device*				vkDevice();

public:

protected:
	virtual SPtr<RenderContext> onCreateContext(const RenderContext_CreateDesc& cDesc);

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

private:
	void createVkInstance();

private:
	MemoryContext_Vk	_memoryContext;
	ExtensionInfo_Vk	_extInfo;

	VkPtr<Vk_Instance>			_vkInstance;
	VkPtr<Vk_PhysicalDevice>	_vkPhysicalDevice;
	VkPtr<Vk_Device>			_vkDevice;
};


#endif


#if 0
#pragma mark --- rdsRenderer_Vk-Impl ---
#endif // 0
#if 1

Renderer_Vk* Renderer_Vk::instance() { return sCast<Renderer_Vk*>(s_instance); }

MemoryContext_Vk* Renderer_Vk::memoryContext() { return &_memoryContext; }

Vk_Instance*			Renderer_Vk::vkInstance()		{ return _vkInstance; }
Vk_PhysicalDevice*		Renderer_Vk::vkphysicalDevice()	{ return _vkPhysicalDevice; }
Vk_Device*				Renderer_Vk::vkDevice()			{ return _vkDevice; }

#endif



}

#endif // RDS_RENDER_HAS_VULKAN


