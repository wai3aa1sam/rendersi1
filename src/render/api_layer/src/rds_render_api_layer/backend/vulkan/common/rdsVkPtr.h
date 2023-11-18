#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderApi_Include_Vk.h"
#include "rdsVk_RenderApiPrimitive.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{
#if 0
#pragma mark --- rdsVkPtr-Decl ---
#endif // 0
#if 0

template<class T>
class VkPtr : public Vk_RenderApiPrimitive<T>
{
public:
	VkPtr() = default;

	VkPtr(T* p)					RDS_NOEXCEPT	{ reset(p); }
	VkPtr(const VkPtr& r)		RDS_NOEXCEPT	{ reset(r._p); }
	VkPtr(VkPtr&& r)			RDS_NOEXCEPT	{ _p = r._p; r._p = VK_NULL_HANDLE; }

	void operator=(T* p)			RDS_NOEXCEPT { reset(p); }
	void operator=(const VkPtr&  r)	RDS_NOEXCEPT { reset(r.ptr()); }
	void operator=(VkPtr&& r)		RDS_NOEXCEPT { reset(VK_NULL_HANDLE); _p = r._p; r._p = VK_NULL_HANDLE; }

	~VkPtr() RDS_NOEXCEPT { reset(VK_NULL_HANDLE); }

			T* operator->()       RDS_NOEXCEPT	{ return _p; }
	const	T* operator->() const RDS_NOEXCEPT	{ return _p; }

	operator       T*()       RDS_NOEXCEPT		{ return _p; }
	operator const T*() const RDS_NOEXCEPT		{ return _p; }

			T* ptr()       RDS_NOEXCEPT			{ return _p; }
	const	T* ptr() const RDS_NOEXCEPT			{ return _p; }
	
	const	T** address() const RDS_NOEXCEPT	{ return sCast<const T**>(&_p); }

	void reset(T* p) RDS_NOEXCEPT 
	{
		//static_assert(std::is_base_of<RefCount_Base, T>::value, "class T is not base on class RefCount_Base");
		if (p == _p) 
			return;
		if (_p) 
		{
			auto c = --_refCount;
			if (c <= 0)
			{
				destroy();
			}
		}
		_p = p;
		if (_p) 
		{
			++_refCount;
		}
	}

	//T* release() RDS_NOEXCEPT { T* o = _p; _p = VK_NULL_HANDLE; return o; }

	T** ptrForInit() RDS_NOEXCEPT { reset(VK_NULL_HANDLE); ++_refCount; return sCast<T**>(&_p); }
};

#endif
}
#endif