#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGraphResource.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgResourcePool-Decl ---
#endif // 0
#if 1

class RdgResourcePool : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize = 24;

public:
	template<class T> using RscTraitsT = typename RdgResourceTraits<T>;

	template<class T> using CreateDescT = typename RscTraitsT<T>::CreateDesc;
	using Texture_CreateDesc = CreateDescT<RdgResource_TextureT>;
	using  Buffer_CreateDesc = CreateDescT<RdgResource_BufferT>;

	template<class T> using RenderResouceT = typename RscTraitsT<T>::RenderResource;

public:
	RdgResourcePool();
	~RdgResourcePool();

	void create (/*RenderGraph* rdGraph*/);
	void destroy();

	void reset();

	RdgResource_TextureT*	createTexture	(Texture_CreateDesc&	cDesc, RenderDevice* rdDev);
	RdgResource_BufferT*	createBuffer	(Buffer_CreateDesc&		cDesc, RenderDevice* rdDev);

protected:
	//template<class T> RenderResouceT<T>* createResource(CreateDescT<T>& cDesc, RenderDevice* rdDev);

protected:
	Vector<SPtr<RdgResource_TextureT>,	s_kLocalSize> _texs;
	Vector<SPtr<RdgResource_BufferT>,	s_kLocalSize> _bufs;

	//Vector<UPtr<RdgResource>,		s_kLocalSize> _rdgRscs;
};

//template<class T> inline
//RdgResourcePool::RenderResouceT<T>* 
//RdgResourcePool::createResource(CreateDescT<T>& cDesc, RenderDevice* rdDev)
//{
//	auto& rdRsc		= _rdRscs.emplace_back();
//	//auto& rdgRsc	= _rdgRscs.emplace_back();
//
//	if constexpr (IsSame<T, Texture2D>)
//	{
//		rdRsc	= rdDev->createTexture2D(cDesc);
//	}
//	else if constexpr(IsSame<T, RenderGpuBuffer>)
//	{
//		rdRsc	= rdDev->createRenderGpuBuffer(cDesc);
//	}
//	return rdRsc.ptr();
//}



#endif

}