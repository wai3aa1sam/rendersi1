#pragma once

#include <rds_demo.h>

#include "rds_fluid_simulation-config.h"
#include "rds_fluid_simulation_traits.h"

#include "rdsColorGradient.h"

namespace rds
{


template<class T> inline Vec2<T>	Rect2T_center(		const Rect2<T>& v)		{ return Vec2<T>{ v.pos } + Vec2<T>{ v.size } / sCast<T>(2.0); }
template<class T> inline Vec3<T>	AABBox3T_center(	const AABBox3<T>& v)	{ return (Vec3<T>{ v.max } + Vec3<T>{ v.min }) / sCast<T>(2.0); }
template<class T> inline Vec3<T>	AABBox3T_size(		const AABBox3<T>& v)	{ return (Vec3<T>{ v.max } - Vec3<T>{ v.min }); }
template<class T> inline Vec3<T>	AABBox3T_halfSize(	const AABBox3<T>& v)	{ return AABBox3T_size(v) / sCast<T>(2.0); }
template<class T> inline Rect2<T>	AABBox3T_toRect2(const AABBox3<T>& v)		{ Rect2<T> o; o.set(v.min.toVec2(), AABBox3T_size(v).toVec2()); return o; }

#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1

class MaterialPool
{
public:
	MaterialPool();
	~MaterialPool();

public:
	void			reset();
	SPtr<Material>	newObject(Shader* shader);		// general objectPool should return *
	//void			deleteObject(SPtr<Material> obj);

private:
	struct Data
	{
	public:
		void			reset();
		SPtr<Material>	newObject(Shader* shader);		// general objectPool should return *
		//void			deleteObject(SPtr<Material> obj);

	public:
		Vector<SPtr<Material>, 16> _freedObjs;
		Vector<SPtr<Material>, 16> _objs;
	};
	FramedT<Data> _data;

protected:
	Data& data() { return _data[Renderer::renderDevice()->engineFrameIndex()]; }
};
#endif

}
