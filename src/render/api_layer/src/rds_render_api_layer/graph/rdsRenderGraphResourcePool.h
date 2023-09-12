#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGraphResource.h"

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
	using TextureCreateDesc = RdgResourceTrait<RdgResource_TextureT>::CreateDesc;
	using BufferCreateDesc	= RdgResourceTrait<RdgResource_BufferT>::CreateDesc;
	template<class T> using CreateDescT = typename RdgResourceTrait<T>::CreateDesc;


public:
	RdgResource_TextureT*	createTexture	(const	TextureCreateDesc&	cDesc);
	RdgResource_TextureT*	createBuffer	(const	BufferCreateDesc&	cDesc);

protected:
	template<class T> RdgResourceT<T>* createResource(const CreateDescT<T>& cDesc);

protected:
	Vector<SPtr<RenderResource>,	s_kLocalSize> _renderResources;
	Vector<RdgResource*,			s_kLocalSize> _rdgResource;
};


#endif

}