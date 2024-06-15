#include "rds_engine-pch.h"
#include "rdsCTransform.h"
#include "rds_engine/rdsEngineContext.h"
#include "rds_engine/ecs/system/rdsCTransformSystem.h"

namespace rds
{

#if 0
#pragma mark --- rdsCTransform-Impl ---
#endif // 0
#if 1

CTransform::System&	CTransform::getSystem(EngineContext& egCtx)	{ return egCtx.transformSystem(); }

CTransform::CTransform()
{
	isTransform = true;
}

CTransform::~CTransform()
{
	getSystem(engineContext()).deleteComponent(id());
}

void 
CTransform::onCreate(Entity* entity)
{
	Base::onCreate(entity);
}

//void 
//CTransform::onDestroy()
//{
//	
//}

void 
CTransform::setLocalTRS(const Vec3f& t, const Quat4f& r, const Vec3f& s)
{
	_localPosition	= t;
	_localRotation	= r;
	_localScale		= s;
	_setDirty();
}

void 
CTransform::setLocalTRS(const Vec3f& t, const Vec3f& r, const Vec3f& s)
{
	Quat4f quat;
	quat.setEuler(r);
	setLocalTRS(t, quat, s);
}

void 
CTransform::_computeLocalMatrix()
{
	_matrixLocal = Mat4f::s_TRS(_localPosition, _localRotation, _localScale);
}


Vec3f CTransform::forward() const
{
	auto v = localRotation() * Vec3f::s_forward();
	//RDS_CORE_ASSERT(math::equals(v.magnitude(), 1.0f), "forawrd is not normalized yet");
	return v.normalize();
}


#endif

}