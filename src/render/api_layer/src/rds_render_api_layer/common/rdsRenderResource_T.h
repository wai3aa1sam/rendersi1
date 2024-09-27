#pragma once

#include "rdsRenderResource.h"

namespace rds
{

#if 0
template<class T>
struct RenderResouceTraits_T
{
	static constexpr RenderResourceType s_kRenderResourceType = RenderResourceType::None;
};
#endif // 0

#if 0
#pragma mark --- rdsRenderResource-Decl ---
#endif // 0
#if 1

template<class T, RenderResourceType TYPE>
class RenderResource_T : public RenderResource
{
public:
	template<class U, class ENABLE> friend struct RdsDeleter;

public:
	RenderResource_T();
	virtual ~RenderResource_T();
};


template<class T, RenderResourceType TYPE> inline
RenderResource_T<T, TYPE>::RenderResource_T()
{
	this->_rdRscType = TYPE;
}

template<class T, RenderResourceType TYPE> inline
RenderResource_T<T, TYPE>::~RenderResource_T()
{

}

#endif

}