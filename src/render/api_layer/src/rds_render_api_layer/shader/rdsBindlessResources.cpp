#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResources.h"

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
BindlessResources::allocTexture(Texture*            rsc)
{
	auto oHnd = BindlessResourceHandle{};
	{
		auto data = _texAlloc.scopedULock();
		oHnd = data->alloc(rsc, this);
	}
	return oHnd;
}

BindlessResourceHandle
BindlessResources::allocBuffer(RenderGpuBuffer*    rsc)
{
	auto oHnd = BindlessResourceHandle{};
	{
		auto data = _bufAlloc.scopedULock();
		oHnd = data->alloc(rsc, this);
	}
	return oHnd;
}

void 
BindlessResources::free(BindlessResourceHandle hnd, RenderResourceType type)
{
	using SRC = RenderResourceType;

	if (!hnd.isValid())
		return;

	switch (type)
	{
		case SRC::RenderGpuBuffer:	{ auto data = _bufAlloc.scopedULock(); data->free(hnd); } break;
		case SRC::Texture:			{ auto data = _texAlloc.scopedULock(); data->free(hnd); } break;
	}
}

void BindlessResources::freeBuffer (RenderGpuBuffer*   rsc)
{
	free(rsc->bindlessHandle(), RenderResourceType::RenderGpuBuffer);
}

void BindlessResources::freeTexture(Texture* rsc)
{
	free(rsc->bindlessHandle(), RenderResourceType::Texture);
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

	{
		auto& table = _samplerStateListTable;

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