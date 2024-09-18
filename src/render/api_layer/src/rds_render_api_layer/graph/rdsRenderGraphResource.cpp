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

RdgResource::RdgResource(const RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported, bool isExported)
//: _name(name), _id(id), _type(type), _isImported(isImported), _isExported(isExported)
{
	create(rdGraph, type, name, id, isImported, isExported);
}

RdgResource::~RdgResource()
{

}

void 
RdgResource::create(const RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported, bool isExported)
{
	#if RDS_DEVELOPMENT
	fmtTo(_name, "{}-f{}", name, rdGraph.frameIndex());
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

}