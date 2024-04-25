#ifndef __rdsGeometryUtil_HLSL__
#define __rdsGeometryUtil_HLSL__

#include "rdsGeometryPrimitive.hlsl"

#if 0
#pragma mark --- GeometryUtil-Impl ---
#endif
#if 1

Plane GeometryUtil_makePlane(float3 pt0, float3 pt1, float3 pt2)
{
    Plane plane;

    float3 vec10 = pt1 - pt0;
    float3 vec20 = pt2 - pt0;

    plane.normal    = normalize(cross(vec10, vec20));
    plane.distance  = dot(plane.normal, pt0);

    return plane;
}



#endif

#endif