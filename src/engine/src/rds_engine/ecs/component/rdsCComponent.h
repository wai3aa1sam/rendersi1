#pragma once

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsCComponent-Decl ---
#endif // 0
#if 1

class Entity;

/*
	no destroy pattern here, see CSystem<T>
*/

class CComponent : public Object
{
	friend class Entity;
	template<class T, class VALUE> friend class EcsVectorTable;
public:
	virtual ~CComponent();


public:
	EngineContext&	engineContext();
	Entity&			entity();

	EntityId		id();

protected:
	void create(Entity* entity);
	//void destroy();

	virtual void onCreate(Entity* entity);
	//virtual void onDestroy();

	//virtual void onAddComponent();
	//virtual void onRemoveComponent();

protected:
	//EngineContext*	_engCtx = nullptr;
	Entity*			_entity = nullptr;
};

template<class T>
class CComponentT : public CComponent
{
	
};

inline Entity&			CComponent::entity()		{ return *_entity; }


#endif

}