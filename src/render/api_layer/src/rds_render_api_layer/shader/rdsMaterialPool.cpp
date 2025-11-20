#include "rds_render_api_layer-pch.h"
#include "rdsMaterialPool.h"
#include "rdsMaterial.h"
#include "rdsShader.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1

MaterialPool::MaterialPool()
{

}

MaterialPool::~MaterialPool()
{
	destroy();
}

void 
MaterialPool::destroy()
{
	_objs.clear();
	_freedObjs.clear();
}

SPtr<Material> 
MaterialPool::newObject(Shader* shader)
{
	RDS_CORE_ASSERT(shader);
	auto* rdDev = shader->renderDevice();

	if (!_freedObjs.is_empty())
	{
		auto obj = _objs.emplace_back(_freedObjs.moveBack());
		if (shader != obj->shader())
		{
			obj->setShader(shader);
		}
		return obj;
	}
	else
	{
		auto& newObj = _objs.emplace_back();
		newObj = rdDev->createMaterial(shader);
		return newObj;
	}
}

void 
MaterialPool::reset()
{
	for (auto& e : _objs)
	{
		_freedObjs.emplace_back(e);
	}
	_objs.clear();
}

#endif

#if 0
#pragma mark --- rdsMutliMaterialPool-Decl ---
#endif // 0
#if 1

MutliMaterialPool::MutliMaterialPool()
{
	_pools.resize(RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit);
}

MutliMaterialPool::~MutliMaterialPool()
{
	destroy();
}

void 
MutliMaterialPool::destroy()
{
	_pools.clear();
}

SPtr<Material> 
MutliMaterialPool::newObject(Shader* shader)
{
	//_rdDev = shader->renderDevice();
	return currentPool().newObject(shader);
}

void 
MutliMaterialPool::reset()
{
	_poolIdx = (_poolIdx + 1) % RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit;
	currentPool().reset();
}

MaterialPool& 
MutliMaterialPool::currentPool() 
{ 
	//auto* rdDev = _rdDev;
	return _pools[_poolIdx]; 
}

#endif

}