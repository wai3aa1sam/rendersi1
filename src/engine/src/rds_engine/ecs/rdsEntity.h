#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "component/rdsCTransform.h"
#include "component/rdsCRenderable.h"

/*
	TODO: remove after having typeInfo
*/
#include "system/rdsCSystem.h"
#include "system/rdsCTransformSystem.h"
#include "system/rdsCRenderableSystem.h"
#include "system/rdsCLightSystem.h"

namespace rds
{

#if 0
#pragma mark --- rdsEntity-Decl ---
#endif // 0
#if 1

class Scene;

class Entity : public Object
{
	RDS_ENGINE_COMMON_BODY();
	friend class Scene;
public:
	using Components = Vector<SPtr<CComponent/*, DefaultDestructor<CComponent> */>, 4>;

public:
	Entity();
	~Entity();

	template<class T, class... ARGS>	T*	 addComponent(ARGS&&... args);
	//template<class T>					void removeComponent();
	//									void removeComponent(CComponent* component);
	template<class T>					T*	 getComponent();

	void setName(StrView name);

public:
	EngineContext&		engineContext();
	Scene&				scene();
	EntityId			id()	const;
	const String&		name()	const;
	CTransform&			transform();

	Span<		SPtr<CComponent> > components();
	Span<const	SPtr<CComponent> > components() const;

protected:
	void create(Scene& scene, EntityId id);
	void destroy();

protected:
	Scene*		_scene = nullptr;
	EntityId	_id;
	String		_name;
	Components	_components;
};

template<class T, class... ARGS> inline
T* 
Entity::addComponent(ARGS&&... args)
{
	auto& sys = T::getSystem(engineContext());
	T* comp = sys.newComponent<T>(this, rds::forward<ARGS>(args)...);
	comp->create(this);
	_components.emplace_back(comp);
	return comp;
}

template<class T> inline
T* 
Entity::getComponent()
{
	// needs type id (not c++ type id) / reflection
	auto& sys = T::getSystem(engineContext());
	return sCast<T*>(sys.findComponent(id()));
}

inline Scene&				Entity::scene()				{ return *_scene; }
inline EntityId				Entity::id()		const	{ return _id; }
inline const String&		Entity::name()		const	{ return _name; }
inline CTransform&			Entity::transform()			{ return *getComponent<CTransform>(); }

inline Span<		SPtr<CComponent> > Entity::components()			{ return _components; }
inline Span<const	SPtr<CComponent> > Entity::components() const	{ return spanConstCast<const SPtr<CComponent> >(_components); }

#endif

}