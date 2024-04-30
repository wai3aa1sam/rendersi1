#pragma once

#include "rds_engine/common/rds_engine_common.h"
//#include "rds_engine/ecs/rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsCSystem-Decl ---
#endif // 0
#if 1

class CSystem : public NonCopyable
{
	RDS_ENGINE_COMMON_BODY();
public:
	virtual ~CSystem() = default;

	void create(EngineContext* egCtx);
	void destroy();

public:
	EngineContext& engineContext();


protected:
	EngineContext* _egCtx = nullptr;
};

inline EngineContext& CSystem::engineContext() { return *_egCtx; }


#endif

#if 0
#pragma mark --- rdsCSystem-Decl ---
#endif // 0
#if 1


/*
	System holds the ownership but with a raw ptr instead (so it will not increase refCount), since it is hard to determine when to remove from the system,
	Entity also holds the ownership but with SPtr (refCount = 1), Component dtor (refCount == 0) will remove from the system
*/

template<class T>
class CSystemT : public CSystem
{
	friend class Entity;
public:
	using Base = CSystem;
	using System = typename T::System;

public:
	void create(EngineContext* egCtx);
	void destroy();


public:
	template<class U, class... ARGS>	U*		newComponent(Entity* entity, ARGS&&... args);
										void	deleteComponent(EntityId id);
	template<class U>					U*		findComponent(EntityId	id);
	T*											findComponent(EntityId	id);

public:
			Vector<T*>& components();
	const	Vector<T*>& components() const;


protected:
	template<class U> void onNewComponent(	 Entity*	entity, U* component) {}
					  void onDeleteComponent(EntityId	id) {}

protected:
	EcsAllocator			_ecsAlloc;
	EcsVectorTable<T, T*>	_componentVecTable;
};

template<class T> inline
void 
CSystemT<T>::create(EngineContext* egCtx)
{
	Base::create(egCtx);
	_componentVecTable.create(&_ecsAlloc);
}

template<class T> inline
void 
CSystemT<T>::destroy()
{
	_componentVecTable.destroy();
	Base::destroy();
}

template<class T>
template<class U, class... ARGS> inline
U*		
CSystemT<T>::newComponent(Entity* entity, ARGS&&... args)
{
	U* p = _componentVecTable.newElement<U>(entity->id(), rds::forward<ARGS>(args)...);
	return p;
}

template<class T> inline
void	
CSystemT<T>::deleteComponent(EntityId id)
{
	_componentVecTable.removeElement(id);
}

template<class T> 
template<class U> inline
U*	
CSystemT<T>::findComponent(EntityId id)
{
	return sCast<U*>(findComponent(id));
}

template<class T> inline
T*	
CSystemT<T>::findComponent(EntityId id)
{
	return _componentVecTable.findElement(id);
}

template<class T> inline		Vector<T*>& CSystemT<T>::components()		{ return _componentVecTable.elements(); }
template<class T> inline const	Vector<T*>& CSystemT<T>::components() const { return _componentVecTable.elements(); }


#endif

}