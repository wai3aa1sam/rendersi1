#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class Material;

#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1

class MaterialPool
{
public:
	MaterialPool();
	~MaterialPool();

	void destroy();

public:
	void			reset();
	SPtr<Material>	newObject(Shader* shader);
	//void			deleteObject(SPtr<Material> obj);

public:
	Vector<SPtr<Material>, 16> _freedObjs;
	Vector<SPtr<Material>, 16> _objs;
};

#endif

#if 0
#pragma mark --- rdsMutliMaterialPool-Decl ---
#endif // 0
#if 1

class MutliMaterialPool
{
public:
	MutliMaterialPool();
	~MutliMaterialPool();

	void destroy();

public:
	void			reset();
	SPtr<Material>	newObject(Shader* shader);
	//void			deleteObject(SPtr<Material> obj);

private:
	MaterialPool& currentPool();

private:
	using Pools = Vector<MaterialPool, RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit>;

	RenderDevice* _rdDev = nullptr;
	
	Atm<i32>	_poolIdx = 0;
	Pools		_pools;
};

#endif

}