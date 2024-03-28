#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCComponent.h"

namespace rds
{

class CTransformSystem;

#if 0
#pragma mark --- rdsCTransform-Decl ---
#endif // 0
#if 1

class CTransform : public CComponent
{
public:
	using Base		= CComponent;
	using System	= CTransformSystem;

public:
	static System& getSystem(EngineContext& egCtx);

public:
	CTransform();
	virtual ~CTransform();

	void setLocalPosition	(float x, float y, float z);
	void setLocalPosition	(const Vec3f&  v);
	void setLocalRotation	(const Quat4f& v);
	void setLocalScale		(const Vec3f&  v);

public:
	const Mat4f& localMatrix();
	const Mat4f& worldMatrix();

protected:
	void _setDirty();
	void _computeLocalMatrix();

protected:
	void onCreate(Entity* entity)	override;
	//void onDestroy()				override;

protected:
	Vec3f	_localPosition;
	Vec3f	_localScale;
	Quat4f	_localRotation;

	Mat4f	_matLocal;
	Mat4f	_matWorld;

	bool _isDirty = false;
};

inline void CTransform::setLocalPosition	(float x, float y, float z) { setLocalPosition(Vec3f(x,y,z)); }
inline void CTransform::setLocalPosition	(const Vec3f&  v)			{ _localPosition = v;	_setDirty(); }
inline void CTransform::setLocalRotation	(const Quat4f& v)			{ _localRotation = v;	_setDirty(); }
inline void CTransform::setLocalScale		(const Vec3f&  v)			{ _localScale = v;		_setDirty(); }

inline void CTransform::_setDirty() 
{ 
	_isDirty = true;
	if (_isDirty)
	{
		_isDirty	= false;
		_computeLocalMatrix();
	}
}

inline
const Mat4f& 
CTransform::localMatrix()
{
	return _matLocal;
}

inline
const Mat4f& 
CTransform::worldMatrix()
{
	return localMatrix();
}

#endif

}