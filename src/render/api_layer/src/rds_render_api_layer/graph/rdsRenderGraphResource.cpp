#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphResource.h"
#include "rdsRenderGraphPass.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgResource-Impl ---
#endif // 0
#if 1

void 
RdgResource::addProducer(RdgPass* producer)
{
	// TODO: render graph memory aliasing will help the vram bandwith;
	//RDS_CORE_ASSERT(_producers.size() == 0, "only 1 write is supported, please create new resource for further process");
	RDS_CORE_ASSERT(isUniqueProducer(producer), "duplicated producer: {}", producer->name());
	_producers.emplace_back(producer);
}

#endif

}