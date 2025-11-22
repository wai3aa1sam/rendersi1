#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

class RdgPass;

#if 0
#pragma mark --- rdsObjectAllocator-Decl ---
#endif // 0
#if 1

template<class T>
class ObjectAllocator : public NC_RenderApiLayerCommon_Base
{
public:
	class Allocation
	{

		T _obj;
	};

};

class PooledObjectAllocator : public NC_RenderApiLayerCommon_Base
{
public:

	class Allocation_Base
	{
	public:
		virtual ~Allocation_Base() {}
	};

	template<class T>
	class Allocation : public Allocation_Base
	{

		Vector<T> _objs;
	};

};



#endif

}