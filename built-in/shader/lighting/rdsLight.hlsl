#ifndef __rdsLight_HLSL__
#define __rdsLight_HLSL__

#include "rdsLighting_Common.hlsl"
#include "rdsLight_ShaderInterop.hlsl"
#include "built-in/shader/geometry/rdsGeometryPrimitive.hlsl"

#if 0
#pragma mark --- Light-Impl ---
#endif
#if 1

bool Light_isEnabled(Light light)
{
	return light.type != rds_LightType_None;
}

bool Light_isPointLight(Light light)
{
	return light.type == rds_LightType_Point;
}

bool Light_isSpotLight(Light light)
{
	return light.type == rds_LightType_Spot;
}

bool Light_isDirectionalLight(Light light)
{
	return light.type == rds_LightType_Directional;
}

float Light_getSpotCosAngle(Light light)
{
	return cos(light.param.x);
}

Sphere Light_makeSphere(Light light)
{
	Sphere sphere;
	sphere.center = light.positionVs.xyz;
	sphere.radius = light.range;
	return sphere;
}

Cone Light_makeCone(Light light)
{
	Cone cone;
	cone.tip 		= light.positionVs.xyz;
	cone.hieght 	= light.range;
	cone.direction 	= light.directionVs.xyz;
	cone.radius		= tan(Light_getSpotAngle(light)) * light.range;
	return cone;
}

#endif

#endif