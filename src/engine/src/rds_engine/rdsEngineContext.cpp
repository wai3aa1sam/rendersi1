#include "rds_engine-pch.h"
#include "rdsEngineContext.h"

#include "ecs/system/rdsCTransformSystem.h"
#include "ecs/system/rdsCRenderableSystem.h"

namespace rds
{

#if 0
#pragma mark --- rdsEngineContext-Impl ---
#endif // 0
#if 1

EngineContext::EngineContext()
{

}

EngineContext::~EngineContext()
{
	destroy();
}

void 
EngineContext::create()
{
	_transformSystem	= registerSystem(newSystem<CTransformSystem>());
	_renderableSystem	= registerSystem(newSystem<CRenderableSystem>());
}

void 
EngineContext::destroy()
{
	for (auto& e : _systems)
	{
		deleteT(e);
	}
	_systems.clear();
}

void 
EngineContext::_registerSystem(CSystem* system)
{
	_systems.emplace_back(system);
}


#endif

}