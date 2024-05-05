#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCComponent.h"
#include <rds_render_api_layer/shader/rdsShaderInterop.h>

namespace rds
{

class CLightSystem;


#define LightType_ENUM_LIST(E) \
	\
	E(Point,		= RDS_LIGHT_TYPE_POINT) \
	E(Spot,			= RDS_LIGHT_TYPE_SPOT) \
	E(Directional,	= RDS_LIGHT_TYPE_DIRECTIONAL) \
	E(Area,			= RDS_LIGHT_TYPE_AREA) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(LightType, u8);

#if 0
#pragma mark --- rdsCLight-Decl ---
#endif // 0
#if 1

class CLight : public CComponent
{
	friend class CLightSystem;
public:
	using Base		= CComponent;
	using System	= CLightSystem;

	using Light	= shaderInterop::Light;

public:
	static System& getSystem(EngineContext& egCtx);

public:
	CLight();
	virtual ~CLight();

	/*
		use CTransform::position() instead
	*/
public:
	void setType(LightType type);

	void setColor(const Vec4f&	 v);
	void setColor(float r, float g, float b, float a);
	void setColor(const Color4f& v);

	void setRange(	  float v);
	void setIntensity(float v);

	void setSpotAngle(	   float radian);
	void setSpotInnerAngle(float radian);

public:
	LightType	lightType()		const;
	Color4f		color()			const;
	float		range()			const;
	float		intensity()		const;

	float		spotAngle()			const;
	float		spotInnerAngle()	const;

	bool		isDirty() const;

protected:
	void setPositionVs	(const Vec4f&  v);
	void setPositionVs	(const Vec3f&  v);
	void setPositionVs	(float x, float y, float z, float w = 1.0f);

	void setDirectionVs	(const Vec4f&  v);
	void setDirectionVs	(const Vec3f&  v);
	void setDirectionVs	(float x, float y, float z, float w = 1.0f);

	Vec3f positionVs()	const;
	Vec3f directionVs()	const;

	void resetDirty();

protected:
			Light& lightData();
	const	Light& lightData() const;

	void _setDirty();

protected:
	void onCreate(Entity* entity)	override;
	//void onDestroy()				override;

protected:
	// store Light* will gg, since each frame is different and the vector will resize...
	u32 _lightIdx = 0;

	bool _isDirty = false;
};


inline LightType	CLight::lightType()		const		{ return sCast<LightType>(lightData().type); }

inline Color4f		CLight::color()			const		{ const auto& color = lightData().color; return Color4f{color.x, color.y, color.z, color.w}; }
inline float		CLight::range()			const		{ return lightData().range; }
inline float		CLight::intensity()		const		{ return lightData().intensity; }

inline bool			CLight::isDirty()		const		{ return _isDirty; }

inline 
void 
CLight::_setDirty() 
{ 
	_isDirty = true;
	if (_isDirty)
	{
		_isDirty	= false;
	}
}


#endif

}