#pragma once

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{

class CSystem;

class CTransformSystem;
class CRenderableSystem;
class CLightSystem;

#if 0
#pragma mark --- rdsEngineContext-Decl ---
#endif // 0
#if 1


//template<class T>
//struct EngineContext_Deleter : public NonCopyable
//{
//	void operator() (T* p)
//	{
//		if (!p)
//			return;
//		auto& egCtx = p->engineContext();
//		//p->~T();
//		egCtx.deleteT(p);
//	}
//};

class EngineContext : public NonCopyable
{
	RDS_ENGINE_COMMON_BODY();
	//template<class T> friend struct EngineContext_Deleter;
public:
	static constexpr SizeType s_kSystemLocalSize = 32;

public:
	//template<class T> using UPtr = UPtr<T, EngineContext_Deleter<T> >;
	//template<class T> using UPtr = UPtr<T>;
	//using Systems = Vector<UPtr<CSystem>, s_kSystemLocalSize>;
	using Systems = Vector<CSystem*, s_kSystemLocalSize>;

public:
	EngineContext();
	~EngineContext();

	void create();
	void destroy();

	template<class T> T* registerSystem(T* system);

	template<class T> T& getSystem();

public:
	CTransformSystem&	transformSystem();
	CRenderableSystem&	renderableSystem();
	CLightSystem&		lightSystem();

public:
	EcsAllocator& entityAllocator();

protected:
	template<class T, class... ARGS>	T*		newT(ARGS&&... args);
	template<class T>					void	deleteT(T* p);

	template<class T, class... ARGS>	T*		newSystem(ARGS&&... args);
	void _registerSystem(CSystem* system);


protected:
	LinearAllocator	_alloc;
	Systems			_systems;

	EcsAllocator	_entitiyAlloc;

	// since no reflection now, but as a cache is also ok
	CTransformSystem*	_transformSystem	= nullptr;
	CRenderableSystem*	_renderableSystem	= nullptr;
	CLightSystem*		_lightSystem		= nullptr;
};

inline CTransformSystem&	EngineContext::transformSystem()	{ return *_transformSystem; }
inline CRenderableSystem&	EngineContext::renderableSystem()	{ return *_renderableSystem; }
inline CLightSystem&		EngineContext::lightSystem()		{ return *_lightSystem; }

template<class T> inline T* EngineContext::registerSystem(T* system) { _registerSystem(system); return system; }

template<class T> inline 
T& 
EngineContext::getSystem()
{
	return T::getSystem(*this);
}


inline EcsAllocator& EngineContext::entityAllocator() { return _entitiyAlloc; }


#if 1

template<class T, class... ARGS> inline
T*
EngineContext::newT(ARGS&&... args)
{
	auto* p = _alloc.alloc(sizeof(T));
	T* obj = new(p) T(rds::forward<ARGS>(args)...);
	return obj;
}

template<class T> inline
void
EngineContext::deleteT(T* p)
{
	p->~T();
	_alloc.free(p);
}

template<class T, class... ARGS> inline
T*
EngineContext::newSystem(ARGS&&... args)
{
	auto* sys = newT<T>(rds::forward<ARGS>(args)...);
	//auto sys = makeUPtr<T>(rds::forward<ARGS>(args)...);
	sys->create(this);
	return sys;
}

#endif // 1


#endif


#if 1

class CRenderable;


// temporary solution for fast testing
struct CSystemUtil
{
public:
	/*using Component = COMPONENT;
	using System	= typename Component::System;*/
public:
	template<class COMPONENT> static typename COMPONENT::System& getSystem(EngineContext& egCtx);
};

//template<class COMPONENT> inline typename COMPONENT::System& CSystemUtil::getSystem(EngineContext& egCtx);


//template<>
//struct CSystemUtil<CRenderable>
//{
//public:
//	using Component = COMPONENT;
//	using System	= typename Component::System;
//public:
//	static System& getSystem();
//};

#endif // 1

}