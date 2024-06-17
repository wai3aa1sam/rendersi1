#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResources.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsBindlessResources-Impl ---
#endif // 0
#if 1

BindlessResources::CreateDesc BindlessResources::makeCDesc()
{
	return CreateDesc{};
}

BindlessResources::SizeType     
BindlessResources::supportSamplerCount()
{
	return 3;
}

BindlessResources::SizeType     
BindlessResources::createSamplerListTable(SamplerStateListTable& o)
{
	supportSamplerCount();
	{
		auto& table = o;

		SamplerState s;
		table.add(s);

		s = {};
		s.minFliter = SamplerFilter::Nearest;
		s.magFliter = SamplerFilter::Nearest;
		table.add(s);

		s = {};
		s.minFliter = SamplerFilter::Nearest;
		s.magFliter = SamplerFilter::Bilinear;
		table.add(s);
	}

	RDS_CORE_ASSERT(supportSamplerCount() == o.size(), "invalid support count");

	return supportSamplerCount();
}

BindlessResources::SizeType 
BindlessResources::bindlessTypeCount() 
{ 
	auto typeCount = enumInt(BindlessResourceType::_kCount); 
	return isSupportAccelerationStruct() ? typeCount : typeCount - 1;
}

bool
BindlessResources::isSupportAccelerationStruct()
{
	return Renderer::rdDev()->adapterInfo().feature.hasAccelerationStruct;

}

BindlessResources::BindlessResources()
{

}

BindlessResources::~BindlessResources()
{

}

void 
BindlessResources::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
BindlessResources::destroy()
{
	onDestroy();
	Base::destroy();
}

BindlessResourceHandle
BindlessResources::allocBuffer(RenderGpuBuffer* rsc)
{
	return alloc(rsc, _bufAlloc, 1, BindlessResourceType::Buffer);
}

BindlessResourceHandle
BindlessResources::allocTexture(Texture* rsc)
{
	return alloc(rsc, _texAlloc, 1, BindlessResourceType::Texture);
}

BindlessResourceHandle 
BindlessResources::allocImage(Texture* rsc)
{
	return alloc(rsc, _imgAlloc, rsc->mipCount(), BindlessResourceType::Image);
}

void
BindlessResources::freeBuffer (RenderGpuBuffer* rsc)
{
	free(rsc->bindlessHandle(), _bufAlloc, 1, BindlessResourceType::Buffer);
}

void
BindlessResources::freeTexture(Texture* rsc)
{
	free(rsc->bindlessHandle(), _texAlloc, 1, BindlessResourceType::Texture);
}

void 
BindlessResources::freeImage(Texture* rsc)
{
	free(rsc->uavBindlessHandle(), _imgAlloc, rsc->mipCount(), BindlessResourceType::Image);
}

void 
BindlessResources::commit()
{
	onCommit();
}

void 
BindlessResources::onCreate(const CreateDesc& cDesc)
{
	_size	= cDesc.size;

	createSamplerListTable(_samplerStateListTable);
}

template<class RSC>
void ResourceAlloc_destroy(MutexProtected<BindlessResources::ResourceAlloc<RSC> >& alloc)
{
	using ResourceAlloc = BindlessResources::ResourceAlloc<RSC>;
	Vector<SPtr<RSC>, ResourceAlloc::s_kLocalSize> buf;
	{
		auto data = alloc.scopedULock();
		buf.reserve(data->rscs.size());
		buf = rds::move(data->rscs);		
		data->rscs.clear();		// move is not work...
	}
	buf.clear();
}

void 
BindlessResources::onDestroy()
{
	ResourceAlloc_destroy(_bufAlloc);
	ResourceAlloc_destroy(_texAlloc);
	_samplerStateListTable.clear();

	// this will lock twice
	//{ auto data = _bufAlloc.scopedULock(); data->rscs.clear(); }
	//{ auto data = _texAlloc.scopedULock(); data->rscs.clear(); }
}

void 
BindlessResources::onCommit()
{

}


#endif

}