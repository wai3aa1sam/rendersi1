#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1

MaterialPool::MaterialPool()
{
	_data.resize(RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit);
	RDS_TODO("this class also in RenderUiContext, please separate it as a file");
	RDS_TODO("this will trigger un - freed block in vma in mt mode, please check later");
}

MaterialPool::~MaterialPool()
{

}

SPtr<Material> 
MaterialPool::newObject(Shader* shader)
{
	return data().newObject(shader);
}

//void 
//MaterialPool::deleteObject(SPtr<Material> obj)
//{
//	data().deleteObject(obj);
//}

void 
MaterialPool::reset()
{
	data().reset();
}

SPtr<Material> 
MaterialPool::Data::newObject(Shader* shader)
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

//void 
//MaterialPool::Data::deleteObject(SPtr<Material> obj)
//{
//	RDS_TODO("check object ptr is valid, eg. within the pool");
//	_freedObjs.emplace_back(obj);
//}

void 
MaterialPool::Data::reset()
{
	for (auto& e : _objs)
	{
		_freedObjs.emplace_back(e);
	}
	_objs.clear();
}
#endif

}