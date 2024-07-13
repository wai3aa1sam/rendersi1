#ifndef __rdsLight_ShaderInterop_HLSL__
#define __rdsLight_ShaderInterop_HLSL__

//#include "rdsLighting_Common.hlsl"

#if 0
#pragma mark --- Light_ShaderInterop-Impl ---
#endif
#if 1

/*
	set cos radian in app side
*/

RDS_OUT(float) Light_getSpotCosAngle(RDS_OUT(Light) light)
{
	return light.param.x;
}

RDS_OUT(float) Light_getSpotInnerCosAngle(RDS_OUT(Light) light)
{
	return light.param.y;
}

RDS_OUT(float) Light_getSpotAngle(RDS_OUT(Light) light)
{
	return light.param.z;
}

#endif

#endif