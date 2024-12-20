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
	return 16;
}

BindlessResources::SizeType     
BindlessResources::createSamplerListTable(SamplerStateListTable& o)
{
	supportSamplerCount();

	auto& table = o;
	{

		SamplerState defaultSampler = SamplerState::makeLinearRepeat();
		table.add(defaultSampler);

		bool hasAddedDefault = false;

		SamplerState s;
		for (size_t i = 1; i < enumInt(SamplerFilter::_kCount); i++)
		{
			for (size_t j = 1; j < enumInt(SamplerWrap::_kCount); j++)
			{
				s.minFliter = sCast<SamplerFilter>(j);
				s.magFliter = sCast<SamplerFilter>(j);

				s.wrapU		= sCast<SamplerWrap>(i);
				s.wrapV		= sCast<SamplerWrap>(i);
				s.wrapS		= sCast<SamplerWrap>(i);

				if (!hasAddedDefault && defaultSampler == s)
				{
					hasAddedDefault = true;
					continue;
				}

				table.add(s);
			}
		}
	}

	RDS_CORE_ASSERT(supportSamplerCount() == table.size(), "invalid support count, supportSamplerCount(): {}, samplerListTable count: {}", supportSamplerCount(), table.size());

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
	return Renderer::renderDevice()->adapterInfo().feature.hasAccelerationStruct;

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

	auto data = _bufAlloc.scopedULock();
	data->totalResourcesCount();
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
	return alloc(_bufAlloc, rsc, 1, BindlessResourceType::Buffer);
}

BindlessResourceHandle
BindlessResources::allocTexture(Texture* rsc)
{
	return alloc(_texAlloc, rsc, 1, BindlessResourceType::Texture);
}

BindlessResourceHandle 
BindlessResources::allocImage(Texture* rsc)
{
	return alloc(_imgAlloc, rsc, rsc->mipCount(), BindlessResourceType::Image);
}

void
BindlessResources::freeBuffer (RenderGpuBuffer* rsc)
{
	free(_bufAlloc, rsc->bindlessHandle(), 1, BindlessResourceType::Buffer);
}

void
BindlessResources::freeTexture(Texture* rsc)
{
	free(_texAlloc, rsc->bindlessHandle(), 1, BindlessResourceType::Texture);
}

void 
BindlessResources::freeImage(Texture* rsc)
{
	free(_imgAlloc, rsc->uavBindlessHandle(), rsc->mipCount(), BindlessResourceType::Image);
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
	//Vector<SPtr<RSC>, ResourceAlloc::s_kLocalSize> rscs;
	{
		//auto data = alloc.scopedULock();
		//rscs.reserve(data->rscs.size());
		//rscs = rds::move(data->rscs);		
		//data->rscs.clear();		// move is not work...
	}
	//buf.clear();
}

void 
BindlessResources::onDestroy()
{
	ResourceAlloc_destroy(_bufAlloc);
	ResourceAlloc_destroy(_texAlloc);
	ResourceAlloc_destroy(_imgAlloc);
	_samplerStateListTable.clear();

	// this will lock twice
	//{ auto data = _bufAlloc.scopedULock(); data->rscs.clear(); }
	//{ auto data = _texAlloc.scopedULock(); data->rscs.clear(); }
}

void 
BindlessResources::onCommit()
{

}

u32 
BindlessResources::findSamplerIndex(const SamplerState& samplerState) const 
{ 
	auto idx = _samplerStateListTable.findIndex(samplerState);

	/*
	RDS_DUMP_VAR(idx);
	RDS_DUMP_VAR(_samplerStateListTable.size());
	RDS_DUMP_VAR(samplerState.minFliter, samplerState.magFliter);
	RDS_DUMP_VAR(samplerState.wrapU, samplerState.wrapV, samplerState.wrapS);
	RDS_DUMP_VAR(samplerState.minLod, samplerState.maxLod);
	*/

	RDS_CORE_ASSERT(idx != _samplerStateListTable.s_kInvalid, "not found");
	return sCast<u32>(idx);
}

template<class T>
BindlessResourceHandle 
BindlessResources::alloc(MutexProtected<ResourceAlloc<T> >& alloc, T* rsc, u32 count, BindlessResourceType type)
{
	auto oHnd = BindlessResourceHandle{};
	{
		auto data = alloc.scopedULock();
		oHnd = data->alloc(rsc, this, count, type);
	}
	return oHnd;
}

template<class T>
void 
BindlessResources::free(MutexProtected<ResourceAlloc<T> >& alloc, BindlessResourceHandle hnd, u32 count, BindlessResourceType type)
{
	if (!hnd.isValid())
		return;
	{ 
		auto data = alloc.scopedULock(); 
		data->free(hnd, count, type); 
	}
}

template<class T>
u32 
BindlessResources::_totalResourcesCount(MutexProtected<ResourceAlloc<T> >& alloc) const
{ 
	auto data = alloc.scopedULock();
	return data->totalResourcesCount();
}

BindlessResources::SizeType 
BindlessResources::totalResourcesCount() const
{
	SizeType n = 0;
	n += _totalResourcesCount(constCast(_bufAlloc));
	n += _totalResourcesCount(constCast(_texAlloc));
	n += _totalResourcesCount(constCast(_imgAlloc));
	return n;
}

#endif

}