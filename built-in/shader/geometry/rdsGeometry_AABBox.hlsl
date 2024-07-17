#ifndef __rdsGeometry_HLSL__
#define __rdsGeometry_HLSL__

/*
    references:
    ~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "rdsGeometryPrimitive.hlsl"

#if 0
#pragma mark --- Geometry-Impl ---
#endif
#if 1

AABBox 
makeAABBox(float3 min_, float3 max_)
{
	AABBox o;
	o.min = min_;
	o.max = max_;
	return o;
}

AABBox 
makeAABBoxByCenter(float3 center, float3 extent)
{
	AABBox o;
	o.min = center - extent;
	o.max = center + extent;
	return o;
}

bool 
Geometry_isOverlapped_AABBoxAABBox(AABBox a, AABBox b)
{
	//return true;
	return (   (a.max.x >= b.min.x) && (a.min.x <= b.max.x)
			&& (a.max.y >= b.min.y) && (a.min.y <= b.max.y)
			&& (a.max.z >= b.min.z) && (a.min.z <= b.max.z));
}

#endif

#endif