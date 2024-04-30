#ifndef __rdsLight_HLSL__
#define __rdsLight_HLSL__

#include "rdsLighting_Common.hlsl"
#include "rdsLightInterop.hlsl"
#include "built-in/shader/geometry/rdsGeometryPrimitive.hlsl"

#if 0
#pragma mark --- Light-Impl ---
#endif
#if 1

bool Light_isEnabled(Light light)
{
	return light.type != rds_LightType_None;
}

float Light_getSpotCosAngle(Light light)
{
	return cos(light.param.x);
}

float Light_computeAttenuation(Light light, float distance)
{
	float lightRange = light.range;
	return 1.0f - smoothstep(lightRange * 0.75, lightRange, distance);
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