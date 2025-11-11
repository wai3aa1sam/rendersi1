#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphResource.h"
#include "rdsRenderGraphPass.h"
#include "rdsRenderGraph.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgResource-Impl ---
#endif // 0
#if 1

RdgResource::RdgResource(RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported, bool isExported)
//: _name(name), _id(id), _type(type), _isImported(isImported), _isExported(isExported)
{
	create(rdGraph, type, name, id, isImported, isExported);
}

RdgResource::~RdgResource()
{
	
}

void 
RdgResource::create(RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported, bool isExported)
{
	#if RDS_DEVELOPMENT
	fmtTo(_name, "{}", name);
	#else
	_name		= name;
	#endif

	_id			= id;
	_type		= type;
	_isImported = isImported;
	_isExported = isExported;
}

void 
RdgResource::addProducer(RdgPass* producer)
{
	// TODO: render graph memory aliasing will help the vram bandwith;
	//RDS_CORE_ASSERT(_producers.size() == 0, "only 1 write is supported, please create new resource for further process");
	RDS_CORE_ASSERT(isUniqueProducer(producer), "duplicated producer: {}", producer->name());
	_producers.emplace_back(producer);
}

bool 
RdgResource::isUniqueProducer(RdgPass* producer) const
{
	//bool isUnique = true;
	for (auto* pass : _producers)
	{
		if (pass == producer)
			return false;
	}
	return true;
}

#endif

#if 1

RdgResourceHnd::~RdgResourceHnd()
{
	checkMainThreadExclusive(RDS_SRCLOC);		// if not main exclusive, then no need to use ObjectPool, or thread safe ObjectPool

	if (_weakBlock)
	{
		auto* p = _weakBlock.release();
		if (p->isRefCount0())
		{
			_rdGraph->_rdgHndPool.deleteObject(p);
		}
	}
}

void 
RdgResourceHnd::reset(RdgResource* rdgRsc, RenderGraph* rdGraph)
{
	_rdGraph = rdGraph;

	 if (!rdgRsc)
		return _weakBlock.reset(nullptr);

	auto* p = rdgRsc->_weakBlock.ptr();		// *** must copy
	if (p)
	{
		_weakBlock = p;
		return;
	}

	if (_rdGraph)
	{
		_weakBlock = _rdGraph->_rdgHndPool.newObject();
		rdgRsc->_weakBlock.reset(_weakBlock);

		auto data = _weakBlock->data.scopedULock();
		data->obj = rdgRsc;
	}
}

RdgResource* 
RdgResourceHnd::get()			
{ 
	checkMainThreadExclusive(RDS_SRCLOC);		// if not main exclusive, then no need to use ObjectPool, or thread safe ObjectPool
	if (!_weakBlock /*|| _rdGraph->isExecuted()*/) 
		return nullptr; 
	auto data = _weakBlock->data.scopedULock();				
	return sCast<RdgResource*>(data->obj); 
}

RdgResource* 
RdgResourceHnd::get() const	
{ 
	checkMainThreadExclusive(RDS_SRCLOC);		// if not main exclusive, then no need to use ObjectPool, or thread safe ObjectPool
	if (!_weakBlock /*|| _rdGraph->isExecuted()*/)
		return nullptr; 
	auto data = constCast(_weakBlock)->data.scopedULock();	
	return sCast<RdgResource*>(data->obj); 
}

#endif // 1

template class RdgResourceHndT<RdgResource_BufferT>;
template class RdgResourceHndT<RdgResource_TextureT>;

}