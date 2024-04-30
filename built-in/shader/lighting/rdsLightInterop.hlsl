#ifndef __rdsLightInterop_HLSL__
#define __rdsLightInterop_HLSL__

//#include "rdsLighting_Common.hlsl"

#if 0
#pragma mark --- LightInterop-Impl ---
#endif
#if 1

RDS_OUT(float) Light_getSpotAngle(RDS_OUT(Light) light)
{
	return light.param.x;
}

/*
	set cos radian in app side
*/
RDS_OUT(float) Light_getSpotInnerCosAngle(RDS_OUT(Light) light)
{
	return light.param.y;
}

#endif

#endif