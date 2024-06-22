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

/*
	later, the after set TRS, the matrix will calculate immediately, after upload to ParamBuffer, then resetDirty ?
	need to think more, since the position also will affect CLight
*/

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

	void setLocalPosition(float x, float y, float z);
	void setLocalPosition(const Vec3f&  v);
	void setLocalRotation(const Quat4f& v);
	void setLocalRotation(const Vec3f&  eulerRad);
	void setLocalScale(	  const Vec3f&  v);
	void setLocalTRS(	  const Vec3f&  t, const Quat4f& r, const Vec3f&  s);
	void setLocalTRS(	  const Vec3f&  t, const Vec3f&  r, const Vec3f&  s);

public:
	const Mat4f& localMatrix();
	const Mat4f& worldMatrix();

	const Vec3f&	localPosition() const;
	const Vec3f&	localScale()	const;
	const Quat4f&	localRotation() const;

	Vec3f forward() const;

	bool			isDirty() const;

protected:
	void _setDirty();
	void _computeLocalMatrix();

protected:
	void onCreate(Entity* entity)	override;
	//void onDestroy()				override;

protected:
	Vec3f	_localPosition		= Vec3f {0, 0, 0};
	Vec3f	_localScale			= Vec3f {1, 1, 1};
	Quat4f	_localRotation		= Quat4f{0, 0, 0, 1};

	Mat4f	_matrixLocal = Mat4f::s_identity();
	Mat4f	_matrixWorld = Mat4f::s_identity();

	bool _isDirty = false;
};

inline void CTransform::setLocalPosition(float x, float y, float z)		{ setLocalPosition(Vec3f(x,y,z)); }
inline void CTransform::setLocalPosition(const Vec3f&  v)				{ _localPosition = v;					_setDirty(); }
inline void CTransform::setLocalRotation(const Quat4f& v)				{ _localRotation = v;					_setDirty(); }
inline void CTransform::setLocalRotation(const Vec3f&  eulerRad)		{ _localRotation.setEuler(eulerRad);	_setDirty(); }
inline void CTransform::setLocalScale(	 const Vec3f&  v)				{ _localScale = v;						_setDirty(); }

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
	return _matrixLocal;
}

inline
const Mat4f& 
CTransform::worldMatrix()
{
	return localMatrix();
}

inline const Vec3f&		CTransform::localPosition() const { return _localPosition; }
inline const Vec3f&		CTransform::localScale()	const { return _localScale; }
inline const Quat4f&	CTransform::localRotation() const { return _localRotation; }

inline bool				CTransform::isDirty()		const { return _isDirty; }

#endif

}