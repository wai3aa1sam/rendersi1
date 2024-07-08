#include "rds_engine-pch.h"
#include "rdsCLight.h"
#include "rds_engine/rdsEngineContext.h"
#include "rds_engine/ecs/system/rdsCLightSystem.h"

#include "rds_engine/ecs/rdsEntity.h"

#include <rds_render_api_layer/shader/rdsShaderInterop.h>

namespace rds
{
namespace shaderInterop
{
#include <../../../../built-in/shader/lighting/rdsLight_ShaderInterop.hlsl>
}
}

namespace rds
{

#if 0
#pragma mark --- rdsCLight-Impl ---
#endif // 0
#if 1

CLight::System&	CLight::getSystem(EngineContext& egCtx)	{ return egCtx.lightSystem(); }

CLight::CLight()
{
	isLight = true;

}

CLight::~CLight()
{
	auto& sys	 = getSystem(engineContext());

	// swap last to current index
	auto  lastIdx		= sys.components().size() - 1;
	auto& lightParamBuf = sys._lightParamBuf;
	if (_lightIdx < lightParamBuf.size() && lightParamBuf.size() > 1)
	{
		lightParamBuf.at(_lightIdx) = lightParamBuf.at(lastIdx);
	}
	lightParamBuf.popBack();
	
	sys.deleteComponent(id());
}

void 
CLight::onCreate(Entity* entity)
{
	Base::onCreate(entity);

	auto& sys			= getSystem(entity->engineContext());
	auto& lightParamBuf = sys._lightParamBuf;
	lightParamBuf.add();
	_lightIdx = sCast<u32>(lightParamBuf.size() - 1);

	setType( LightType::Point);
	setRange(1.0f);
	setIntensity(1.0f);
	setColor(Vec4f::s_one());
}

void 
CLight::setType(LightType type)
{
	lightData().type = enumInt(type);
	_setDirty();
}

void 
CLight::setColor(const Vec4f& v)
{
	lightData().color = v;
	_setDirty();
}

void 
CLight::setColor(float r, float g, float b, float a)
{
	Vec4f o;
	o.set(r, g, b, a);
	setColor(o);
}

void 
CLight::setColor(const Color4f& v)
{
	setColor(v.r, v.g, v.b, v.a);
}

void CLight::setRange(float v)
{
	lightData().range= v;
	_setDirty();
}

void CLight::setIntensity(float v)
{
	lightData().intensity = v;
	_setDirty();
}

void 
CLight::setSpotAngle(float radian)
{
	auto& spotAngle = shaderInterop::Light_getSpotAngle(lightData());
	spotAngle = radian;
	_setDirty();
}

void 
CLight::setSpotInnerAngle(float radian)
{
	auto& spotInnerCosAngle = shaderInterop::Light_getSpotInnerCosAngle(lightData());
	spotInnerCosAngle = math::cos(radian);
	_setDirty();
}

void 
CLight::setPositionVs(const Vec4f& v) 
{ 
	lightData().positionVs = v; 
	_setDirty();
}

void 
CLight::setPositionVs(const Vec3f& v) 
{
	setPositionVs(Vec4f{ v.x, v.y, v.z, lightType() == LightType::Directional ? 0.0f : 1.0f });
}

void 
CLight::setPositionVs(float x, float y, float z, float w) 
{ 
	setPositionVs(Vec4f{x, y, z, w}); 
}

void 
CLight::setDirectionVs(const Vec4f& v) 
{ 
	lightData().directionVs = v; 
	_setDirty(); 
}

void 
CLight::setDirectionVs(const Vec3f& v) 
{
	setDirectionVs(Vec4f{ v.x, v.y, v.z, 0.0f });
}

void 
CLight::setDirectionVs(float x, float y, float z, float w) 
{ 
	setDirectionVs(Vec4f{ x, y, z, w });
}

void 
CLight::setPositionWs(const Vec3f&  v)
{
	lightData().positionWs = Vec4f{v, 1.0};
	_setDirty(); 
}

void 
CLight::setDirectionWs(const Vec3f&  v)
{
	lightData().directionWs = Vec4f{v, 1.0};
	_setDirty(); 
}

void 
CLight::resetDirty()
{
	_isDirty = false;
}

CLight::Light& 
CLight::lightData()
{
	auto& sys			= getSystem(engineContext());
	auto& lightParamBuf = sys._lightParamBuf;
	return lightParamBuf.at(_lightIdx);
}

const CLight::Light& 
CLight::lightData() const
{
	return constCast(*this).lightData();
}

Vec3f CLight::positionVs()	const { const auto& pos = lightData().positionVs;	return Vec3f{ pos.x, pos.y, pos.z }; }
Vec3f CLight::directionVs()	const { const auto& dir = lightData().directionVs;	return Vec3f{ dir.x, dir.y, dir.z }; }

Vec3f CLight::positionWs()	const { const auto& pos = lightData().positionWs;	return Vec3f{ pos.x, pos.y, pos.z }; }
Vec3f CLight::directionWs()	const { const auto& dir = lightData().directionWs;	return Vec3f{ dir.x, dir.y, dir.z }; }

float CLight::spotAngle()			const	{ RDS_CORE_ASSERT(lightType() == LightType::Spot, ""); return shaderInterop::Light_getSpotAngle(constCast(lightData())); }
float CLight::spotInnerAngle()		const	{ RDS_CORE_ASSERT(lightType() == LightType::Spot, ""); return acos(shaderInterop::Light_getSpotInnerCosAngle(constCast(lightData()))); }

//void 
//CLight::onDestroy()
//{
//	
//}

void 
CLight::_setDirty() 
{ 
	_isDirty = true;
}

#endif

}